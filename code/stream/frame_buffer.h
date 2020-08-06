#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H
#include <iostream>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <vector>
#include <mutex>
#include <condition_variable>
using namespace std;
using namespace cv;
/*
需求分析
图像获取解码的典型时间消耗为5ms以下,图像处理时间最短也在30ms以上
*/
#define BUFFER_LEN 10
class FrameBuffer{
    public:
        FrameBuffer();
        ~FrameBuffer();

        //初始化内存区域
        bool init(int height,int width);
        //返回已经分配好的,当前应使用的内存的指针
        unsigned char* get_buffer();

        enum ReadMode{
            IN_ORDER,//顺序读取
            REAL_TIME//实时性优先,读取最晚产生的,并移动指针忽略较早产生的内容
        };
        //图像处理线程获取图像内容.默认读取策略为按照顺序读取
        bool get_mat(Mat& mat_out,ReadMode read_mode = IN_ORDER);
        //将解码完成的帧在缓冲区里进行标记,有分辨率信息是为了处理横竖屏转换
        bool push_frame(int height,int width);
    private:
        //获取当前可用帧的数量
        int get_useful_frame_number();
        //用于存储缓冲区信息
        vector<unsigned char*>  _buffer;
        //用于记录每个缓冲区的状态
        //0代表无效数据,1代表原始分频率数据,2代表原始分辨率旋转90之后的数据
        enum BufferStat{
            EMPTY,           //不可用
            ORIGIN,          //原始分辨率
            ROTATED       //图像经过旋转
        };
        vector<int> _buffer_stat;
        int _origin_height,_origin_width;

        //用于控制缓冲区为空的时候,等待生产者生成数据
        condition_variable _not_empty;
        //用于防止缓冲区信息被同时更改
        mutex _buffer_info_lock;
        //消费者读取位置和生产者写入位置
        int _read_point = -1,_write_point=0;
          //计时工具
        chrono::high_resolution_clock::time_point _this_time,_last_time;
};

#endif // !FRAME_BUFFER_H