#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include "server_window.h"
#include <QApplication>
using namespace std;
class Server{
public:
    Server();
    ~Server();
    //显示qt初始化参数选择界面,负责server的启动流程,供外部调用
    bool init();
private:
    
};
#endif // !SERVER_H
