#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <fstream>
#include <regex>
#include <thread>
#include <arpa/inet.h>
#include <assert.h>
#include<sys/socket.h>
using namespace std;

//提供功能实现供外部调用
class Server{
public:
    Server();
    ~Server();
    //波特率,分辨率设置,负责合理性检查
    bool update_bitrate(string bitrate);
    bool update_resolution(string resolution); 
    //获取手机id
    bool get_device_name(string& device_name); 
    bool start_by_step();
    bool stop_server();
    int get_socket();

private:
    //推送安卓端到手机
    bool push_server_to_device(string server_path);
    bool remove_server_from_device(string server_path);
    //启动反向代理
    bool reverse_config(string domain_socket_name,string local_port);
    bool remove_reverse(string domain);
    //在安卓端运行
    bool start_run_in_device();
    bool stop_run_in_device();
    bool aftermath();
    //建立图像传输socket连接
    bool video_socket_init(string id,string port);

    //运行shell指令并获取输出结果
    string run_shell(string origin_cmd);
   
    enum ServerStat{
        INIT_STAT,                        //初始状态
        PUSH_SUCCESS,                  //成功推送服务端到手机
        REVERSE_SUCCESS,        //成功 启动反向代理
        RUN_SUCCESS                      //服务端程序已经在手机上运行
    };
    ServerStat _server_stat = INIT_STAT;
    string _device_name;

    //未人工设置情况下采用默认数值
    //TODO:完成参数的选取建议
    int _bitrate = 200000000;
    int _resolution = 720;
    thread _device_server_thread;
    int _video_socket;   //连接套接字
    int _socket_fd;          //监听套接字
};
#endif // !SERVER_H
