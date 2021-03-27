#include "zhidao.h"
ZhiDao::ZhiDao(){
}

ZhiDao::~ZhiDao(){
}

void ZhiDao::init(string template_path){
    _template = imread(template_path);
    _last_time = chrono::high_resolution_clock::now();
}

//检测是否有提问框
bool ZhiDao::detect(Mat& inmat){
    int width = inmat.cols;
    int length = inmat.rows;
    double x_rate = 0.84 , y_rate=0.13;
    Mat roi(inmat, Rect( width * x_rate , length * y_rate , 34 , 34));
    Mat sub=roi-_template;
    imshow("af", sub);
    //用减法匹配模板
    Scalar mean;  //均值
    Scalar stddev;  //标准差
    meanStdDev( sub, mean, stddev );  //计算均值和标准差
    double mean_pxl = mean.val[0];  
    double stddev_pxl = stddev.val[0];  
    cout << mean_pxl << "\t" << stddev_pxl << endl;
    if(mean_pxl > 0.5 && mean_pxl < 1 && stddev_pxl > 1 && stddev_pxl < 3) return false;
    return true;
}

//处理提问框，继续播放视频
//两次触发时间间隔至少大于5分钟
bool ZhiDao::action(){
    auto now = chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = now - _last_time;
    cout<<"use "<<diff.count()<<" s" << endl;
    if(diff.count() < 60) return false;
    int y_start = 620 , y_end = 1870;
    int y_idx = y_start;
    string selete_cmd = "adb shell input tap 137 ";
    while (y_idx < y_end){
        string cmd = selete_cmd + to_string(y_idx);
        const char *p = cmd.data();
        system(p);
        y_idx+=40;
    }
    system("adb shell input tap  956 374 ");
    _last_time = chrono::high_resolution_clock::now();
}

