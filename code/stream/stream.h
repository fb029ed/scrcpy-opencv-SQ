#ifndef STREAM_H
#define STREAM_H
#include <iostream>
#include <chrono>
#include <assert.h>
#include <arpa/inet.h>
#include <thread>
#include <signal.h>
#include<sys/socket.h>

#include "frame_buffer.h"

extern "C"{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libswscale/swscale.h>   
}
#define NO_PTS UINT64_MAX
#define HEADER_SIZE 12
#define DEVICE_NAME_FIELD_LENGTH 64
using namespace std;

//数据流处理类负责从socket原始数据到原始数据帧生成的部分
class Stream{
    public:
        Stream();
        ~Stream();
        //初始化socket和相关解码器
        bool stream_init(int video_socket);
        //运行yuv帧生成线程
        void frame_creat_run();
        static void run_recv(Stream* video_stream);
        //关停原始帧获取线程,释放相关资源
        bool stop_frame_creat();
        //供外部图像处理线程获取图像
        bool get_img(Mat& mat_out);

    private:
        bool ffmpeg_init();
        bool decoder_init();
        bool get_device_info();
        bool recv_packet(AVPacket* packet);
        //从数据包中获取视频帧
        bool packet_manager(AVPacket* packet);
        //从数据包中解析出图像帧
        bool parse_packet_to_frame(AVPacket* packet,AVFrame* decoding_frame);
        
        //单包数据头解析工具函数
        uint64_t buffer_read_64byte(uint8_t* buf);
        uint32_t buffer_read_32byte(uint8_t* buf);
        
        //针对ctrl c退出的情况,释放资源
        static void interrupt_handle(int sig);

        int _video_socket;
        string _device_name;
       int _resolution_width,_resolution_height;

       AVCodec* _codec = nullptr;                                                //解码器
       AVCodecContext* _codec_context= nullptr;               //解码器上下文
       AVCodecParserContext* _parser = nullptr;                  //用于数据拼接,获取单帧数据
       bool _need_stop;
       //单包数据的头部信息,会被频繁调用,为避免内存分配时间浪费提前分配
       unsigned char _header[HEADER_SIZE];
       
       //h264数据包需要拼接,使用该信息记录是否需要拼接
       bool _has_pending = false;
       //需要拼接情况下使用的缓冲区
       AVPacket _pending;
       //yuv整帧信息
       AVFrame* _yuv_frame = nullptr;
      
       FrameBuffer  _frame_buffer;

       //数据接收线程
       thread _recv_thread;
     
};

#endif // !STREAM_H