#include "stream.h"
Stream::Stream(){
}

Stream::~Stream(){
}

//全局变量,signal无法传参数进去只能使用全局变量代替
int socket_fd_g = -1;
void Stream::interrupt_handle(int sig){
    //释放网络资源
    if(socket_fd_g != -1)
        shutdown(socket_fd_g,SHUT_RDWR);
    exit(0);
}

//初始化socket和相关解码器
bool Stream::stream_init(int video_socket){
    if(_video_socket < 0){
        cout << "无效套接字" << endl;
        return false;
    }
    _video_socket = video_socket;
    if(!get_device_info()){
        cout << "获取器件信息失败" << endl;
        return false;
    }
    //初始化ffmpeg
    if(!ffmpeg_init()){
        return false;
    }
    //初始化解码器
    if(!decoder_init()){
        cout << "解码器初始化失败" << endl;
        return false;
    }
    //初始化帧缓冲区
    _frame_buffer.init(_resolution_height,_resolution_width);
    _need_stop = false;
    _yuv_frame = av_frame_alloc();
    _recv_thread = thread(run_recv,this);
    return true;
}

//socket建立只初获取设备信息
bool Stream::get_device_info(){
    unsigned char buf[DEVICE_NAME_FIELD_LENGTH + 4];
    int n = recv(_video_socket, buf, DEVICE_NAME_FIELD_LENGTH + 4, 0);
    if(n != DEVICE_NAME_FIELD_LENGTH + 4){
        cout << "获取器件信息失败" << endl;
        return false;
    }
    buf[DEVICE_NAME_FIELD_LENGTH - 1] = '\0'; // in case the client sends garbage
    // strcpy is safe here, since name contains at least DEVICE_NAME_FIELD_LENGTH bytes
    // and strlen(buf) < DEVICE_NAME_FIELD_LENGTH
    _device_name = (char *)buf;
    _resolution_width = (buf[DEVICE_NAME_FIELD_LENGTH] << 8) | buf[DEVICE_NAME_FIELD_LENGTH + 1];
    _resolution_height = (buf[DEVICE_NAME_FIELD_LENGTH + 2] << 8) | buf[DEVICE_NAME_FIELD_LENGTH + 3];
    cout << "器件名称:"<< _device_name << endl;
    cout << "分辨率    :" << _resolution_width << " * "  << _resolution_height << endl;
    return true;
}

//初始化解码器
bool Stream::decoder_init(){
    //初始化解码器
    _codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if(!_codec){
        cout <<  "解码器初始化失败" <<endl;
       return false;
    }
    //初始化解码器上下文
    _codec_context = avcodec_alloc_context3(_codec);
    if (!_codec_context) {
        cout << "解码器上下文初始化失败" << endl;
        return false;
    }
    //打开解码器
    if (avcodec_open2(_codec_context, _codec, NULL) < 0) {
        cout << "解码器启动失败" << endl;
        return false;
    }
    //初始化解释器,用于把数据解释成单独的数据帧
    _parser = av_parser_init(AV_CODEC_ID_H264);
    if (!_parser) {
        cout << "解释器初始化失败" << endl;
        return false;
    }
    // We must only pass complete frames to av_parser_parse2()!
    // It's more complicated, but this allows to reduce the latency by 1 frame!
    _parser->flags |= PARSER_FLAG_COMPLETE_FRAMES;
    return true;
}

bool Stream::ffmpeg_init(){
    av_register_all();
    if(avformat_network_init()){            //返回0为失败
        cout << "ffmpeg初始化失败" << endl;
        return false;
    } 
    return true;
}
//以上程序为初始化相关程序

//运行yuv帧生成线程
void Stream::frame_creat_run(){
    while(!_need_stop){
        //读取原始h264码流包
        AVPacket packet;
        bool ok = recv_packet(&packet);
        if (!ok) {
            cout << "单帧数据获取失败" << endl;
            return;
        }
        ok = packet_manager(&packet);
        av_packet_unref(&packet);
        if(!ok){
            cout << "视频帧获取失败" << endl;
            return;
        }
    }
    //TODO:退出时通知读者退出
}

void Stream::run_recv(Stream* video_stream){
    signal(SIGINT, interrupt_handle);
    video_stream->frame_creat_run();
}

bool Stream::get_img(Mat& mat_out){
    return _frame_buffer.get_mat(mat_out);
}

//关停原始帧获取线程,释放相关资源
bool Stream::stop_frame_creat(){
    _need_stop = true;
}

//从socket中读取h264原始码流
bool Stream::recv_packet(AVPacket* packet){
    // The video stream contains raw packets, without time information. When we
    // record, we retrieve the timestamps separately, from a "meta" header
    // added by the server before each raw packet.
    //
    // The "meta" header length is 12 bytes:
    // [. . . . . . . .|. . . .]. . . . . . . . . . . . . . . ...
    //  <-------------> <-----> <-----------------------------...
    //        PTS        packet        raw packet
    //                    size
    //
    // It is followed by <packet_size> bytes containing the packet/frame.
    int r =  recv(_video_socket,_header, HEADER_SIZE,0);
    if(r < HEADER_SIZE){
        cout << "单帧头部信息获取失败" << endl;
       return false;
    }

    //获取时间戳
    uint64_t pts = buffer_read_64byte(_header);
    //获取帧长度信息
    uint32_t len = buffer_read_32byte((uint8_t*)&_header[8]);
    assert(pts == NO_PTS || (pts & 0x8000000000000000) == 0);
    assert(len);
    
    //为新接收的数据包分配空间
    if (av_new_packet(packet, static_cast<int>(len))) {
        cout << "包分配空间失败" << endl;
        return false;
    }
    //根据数据包长度读取数据
    int recv_now = 0;
    int len_all = static_cast<int>(len);
    while(recv_now < len_all){
        int need_len = len_all - recv_now; 
        r =  recv(_video_socket, packet->data + recv_now, need_len,0);
        recv_now += r;
    }
    packet->pts = pts != NO_PTS ? static_cast<int64_t>(pts) : static_cast<int64_t>(AV_NOPTS_VALUE);
    return true;
}

//工具函数,用于头部解码
uint32_t Stream::buffer_read_32byte(uint8_t *buf)
{
    return static_cast<uint32_t>((buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3]);
}

uint64_t Stream::buffer_read_64byte(uint8_t *buf)
{
    uint32_t msb = buffer_read_32byte(buf);
    uint32_t lsb = buffer_read_32byte(&buf[4]);
    return (static_cast<uint64_t>(msb) << 32) | lsb;
}


bool Stream::packet_manager(AVPacket* packet){
    bool is_config = packet->pts == AV_NOPTS_VALUE;
    //config数据包不能立即解码,不包含完整数据信息
    //需要配合之后的帧信息才能进行解码
    if(_has_pending || is_config){
        int  offset;
        //有推迟的情况,续接之前的缓冲帧
        if(_has_pending){
            offset = _pending.size;
            //只进行了内存空间的扩充未进行数据的填充
            if(av_grow_packet(&_pending,packet->size)){
                cout << "续接包失败" << endl;
                return false;
            }
        }
        //之前没有缓存帧的情况下,新建包
        else{
            offset = 0;
            if(av_new_packet(&_pending,packet->size)){
                cout << "新建包失败" << endl;
                return false;
            }
            _has_pending = true;
        }

        //将新读取的内容加到pending后面
        memcpy(_pending.data + offset,packet->data,static_cast<unsigned int>(packet->size));
        
        //非配置帧,续接完成后进行信息维护
        if(!is_config){
            _pending.pts = packet->pts;
            _pending.dts = packet -> dts;
            _pending.flags = packet->flags;
            packet = &_pending;
        }
    }

    if(!is_config){
        //有可用于包解析的数据产生
        bool ok = parse_packet_to_frame(packet,_yuv_frame);
        if(_has_pending){
            //此时还有pend非正常情况,出错释放资源
            _has_pending = false;
            av_packet_unref(&_pending);
        }
        if(!ok){
            cout << "从包解析图像帧错误" << endl;
            return false;
        }
    }
    return true;
}

//从数据包解析出视频帧数
bool Stream::parse_packet_to_frame(AVPacket* packet,AVFrame* decoding_frame){
    unsigned char* out_data = nullptr;
    int out_len = 0;
    //从原始数据中分割成一帧帧的数据
    int r = av_parser_parse2(_parser,_codec_context,&out_data,&out_len,packet->data,packet->size,AV_NOPTS_VALUE, AV_NOPTS_VALUE, -1);
    if(r != packet->size || packet->size != out_len){
        cout << "解析错误" << endl;
        return false;
    }
    if(_parser->key_frame == 1){
        packet->flags |= AV_PKT_FLAG_KEY;
    }

    //对提取出的单帧数据进行处理
    if(!_codec_context || !decoding_frame){
        cout << "不具备解码条件" << endl;
        return false;
    }
    //从h264解码到yuv和yuv转换为rgb,处理时间都较短不超过5ms
    //外部的图像处理时间消耗较多,因此解码过程全在该线程进行
    //h264 to yuv

    int ret = -1;
    ret = avcodec_send_packet(_codec_context,packet);
    if(ret < 0) return false;
    ret = avcodec_receive_frame(_codec_context,decoding_frame);
    if(ret < 0) return false;

    //yuv to rgb
    //TODO:运行时间优化
    AVFrame* rgb_frame = nullptr;
    rgb_frame = av_frame_alloc();
    uint8_t* output_buffer = (uint8_t*)_frame_buffer.get_buffer();
    avpicture_fill((AVPicture*)rgb_frame,output_buffer,AV_PIX_FMT_BGR24,_codec_context->width,_codec_context->height);
    SwsContext * img_convert_ctx = sws_getContext(_codec_context->width,_codec_context->height,_codec_context->pix_fmt, _codec_context->width, _codec_context->height,AV_PIX_FMT_BGR24, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    sws_scale(img_convert_ctx, decoding_frame->data, decoding_frame->linesize, 0, _codec_context->height, rgb_frame->data, rgb_frame->linesize); 
    _frame_buffer.push_frame(_codec_context->height,_codec_context->width);
    av_free(rgb_frame); 
    return true;
}
