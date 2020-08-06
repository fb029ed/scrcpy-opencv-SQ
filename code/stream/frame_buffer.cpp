#include "frame_buffer.h"

//缓冲区的处理逻辑
//解码耗时很短,图像处理耗时较长
//仅设置了缓冲区为空的条件变量,在无图像时阻塞等待另一线程生成图像
//针对生产者,循环写入缓冲区,不设置缓冲区满的条件
//特殊处理,在生产者写入位置和消费者读取位置相同时,生产者跳过当前缓冲区写入下一个缓冲区

FrameBuffer::FrameBuffer(){
}
FrameBuffer::~FrameBuffer(){
}

//初始化内存区域
bool FrameBuffer::init(int height,int width){
    int size = height*width *3;//rgb
    _buffer.resize(BUFFER_LEN,nullptr);
    _buffer_stat.resize(BUFFER_LEN,EMPTY);
    for(int i =0;i<BUFFER_LEN;++i){
        _buffer[i] = new unsigned char[size];
    }
    _origin_height = height;
    _origin_width = width;
}

//生产者使用
//返回已经分配好的,当前应使用的内存的指针
unsigned char* FrameBuffer::get_buffer(){
    unique_lock<mutex> lock(_buffer_info_lock);
    //调用该函数之后,外部解码线程会将解码内容写入缓冲区,此时该内存不可用于图像处理
    assert(_buffer[_write_point] != nullptr);
    //将写入缓冲区和读者正在使用的缓冲区重合则写入下一个缓冲区
    if(_write_point == _read_point){
        _write_point = (_write_point+1)%BUFFER_LEN;
    }
    return _buffer[_write_point];
}

//TODO::考虑该模块是否需要加互斥锁
//条件变量
int FrameBuffer::get_useful_frame_number(){
    int len = 0;
    for(int i=0;i<BUFFER_LEN;++i){
        if(_buffer_stat[i] != EMPTY)
            len++;
    }
    return len;
}

//消费者使用
//图像处理线程获取图像内容.默认读取策略为按照顺序读取
bool FrameBuffer::get_mat(Mat& mat_out,ReadMode read_mode){
    unique_lock<mutex> lock(_buffer_info_lock);
    while(get_useful_frame_number() == 0){
        _not_empty.wait(lock);
    }
    
    //下一位有可能为空,此时可以确定有可用数据
    //按照生产者填充数据方向搜寻可用数据帧数
    do{
        _read_point = (_read_point + 1) % BUFFER_LEN;
    }while(_buffer_stat[_read_point] == EMPTY);

    //有数据可以获取了
    if(_buffer_stat[_read_point] == ORIGIN){
        Mat t(_origin_height,_origin_width,CV_8UC3);
        t.data = _buffer[_read_point];
        mat_out = t;
    }
    else if(_buffer_stat[_read_point] == ROTATED){
        Mat t(_origin_width,_origin_height,CV_8UC3);
        t.data = _buffer[_read_point];
        mat_out = t;
    }
    else{
        cout << "线程同步异常" << endl;
        return false;
    }
    _buffer_stat[_read_point] = EMPTY;
    return true;
}

//生产者使用
//将解码完成的帧在缓冲区里进行标记,有分辨率信息是为了处理横竖屏转换
//循环使用缓冲区,无缓冲区满的情况
bool FrameBuffer::push_frame(int height,int width){
    _this_time = chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> tm = _this_time - _last_time;	// 毫秒
	_last_time = _this_time;
    //cout << "单帧传输加解码时间" << tm.count() << "ms" << std::endl;
    {
        unique_lock<mutex> lock(_buffer_info_lock);
        if(height == _origin_height && width == _origin_width){
            //未旋转且图像可用
            _buffer_stat[_write_point] = ORIGIN;
        }
        else{
            //经过旋转图像可用
            _buffer_stat[_write_point] = ROTATED;
        }
        _write_point = (_write_point+1)%BUFFER_LEN;
    }
    //先解锁再通知,防止通知之后获取不到互斥锁再次休眠
    _not_empty.notify_all();
    return true;
}
