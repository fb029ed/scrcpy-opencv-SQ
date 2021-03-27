#ifndef ZHIDAO_H
#define ZHIDAO_H
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <chrono>
using namespace std;
using namespace cv;
//简单实现,类未设计
class  ZhiDao{
    public:
        ZhiDao();
        ~ZhiDao();
        bool detect(Mat& inmat);
        bool action();
        void init(string template_path);
    private:
        Mat _template;
        std::chrono::high_resolution_clock::time_point _last_time;
};
#endif // !ZHIDAO_H