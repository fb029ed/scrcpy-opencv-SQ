#include "server.h"

Server::Server(){
}

Server::~Server(){
}

bool Server::get_device_name(string& device_name){
    device_name = "no available devices";
    string terminal_out = run_shell("adb devices");
    //TODO:检验器件名称的正则表达式正确性
    regex devices_name_rule("\\n[a-z0-9]+\\s+");
    cmatch m;
    vector<string> devices_name_list;
    while(regex_search(terminal_out.c_str(), m, devices_name_rule)){
        devices_name_list.push_back(m.str());
        terminal_out = m.suffix().first;
    }
    if(devices_name_list.size() > 1){
        device_name = "more than 1 devices";
    }
    else if(devices_name_list.size() == 1){
        //去除id前后的不可见字符
        string blanks("\f\v\r\t\n ");
        devices_name_list[0].erase(0,devices_name_list[0].find_first_not_of(blanks));
        devices_name_list[0].erase(devices_name_list[0].find_last_not_of(blanks) + 1);
        device_name = devices_name_list[0];
        return true;
    }
    return false; 
}

bool Server::aftermath(){
    bool ret = false;
    switch(_server_stat){
        case INIT_STAT:
            break;
        case PUSH_SUCCESS:{
            remove_server_from_device("/data/local/tmp/scrcpy-server");
            break;
        }
        case REVERSE_SUCCESS:{
            remove_server_from_device("/data/local/tmp/scrcpy-server");
            remove_reverse("localabstract:scrcpy");
            break;
        }
        case RUN_SUCCESS:{
            /*
            remove_server_from_device("/data/local/tmp/scrcpy-server");
            remove_reverse("localabstract:scrcpy");
            */
            ret = true;
            break;
        }
    }
    return ret;
}

bool Server::start_by_step(){
    switch (_server_stat){
        case INIT_STAT:{
            if(!get_device_name(_device_name))
                break;
            //仅对在build下构建有效
            if(!push_server_to_device("../res/scrcpy-server"))
                break;
            _server_stat = PUSH_SUCCESS;
        }
        case PUSH_SUCCESS:{
            if(!reverse_config("localabstract:scrcpy","tcp:27183"))
                break;
            _server_stat = REVERSE_SUCCESS;
        }
        case REVERSE_SUCCESS:{
            if(!start_run_in_device())
                break;
            _server_stat = RUN_SUCCESS;
        }
    }
    return aftermath();
}

bool Server::stop_server(){
    return stop_run_in_device();
}

bool Server::update_bitrate(string bitrate){

}

bool Server::update_resolution(string resolution){

}

string Server::run_shell(string origin_cmd){
    string temp_path = " > temp.txt";
    system((origin_cmd + temp_path).c_str());
    ifstream terminal_out("temp.txt");
    string terminal_result;
    string tmp;
    while(getline(terminal_out,tmp)){
        terminal_result += "\n" + tmp;
    }
    system("rm -rf temp.txt");    
    return terminal_result;
}

bool Server::push_server_to_device(string server_path){
    string device_path = "/data/local/tmp/scrcpy-server";            //在手机上的推送地址使用定值
    string cmd = "adb -s " + _device_name + " push " + server_path + " " + device_path;
    string result = run_shell(cmd);
    regex result_rule("pushed");
    cmatch m;
    if(!regex_search(result.c_str(), m, result_rule)){
        return false;
    }
    return true;
}

 bool Server::remove_server_from_device(string server_path){
    string cmd = "adb -s " + _device_name + " shell rm -rf " + server_path;
    system(cmd.c_str());
    return true;
 }

bool Server::reverse_config(string domain_socket_name,string local_port){
    string cmd ="adb -s " + _device_name + " reverse "  
        + domain_socket_name + " " + local_port;
    system(cmd.c_str());
    //判断套接字是否建立成功
    string result = run_shell("adb -s " + _device_name + " reverse --list");
    regex result_rule(local_port);
    cmatch m;
    if(!regex_search(result.c_str(), m, result_rule)){
        return false;
    }
    return true;
}

bool Server::remove_reverse(string domain){
    string cmd = "adb -s " + _device_name + " reverse --remove domain";
    system(cmd.c_str());
    return true;
}

bool Server::start_run_in_device(){
    string cmd = "adb -s "+ _device_name 
        + " shell CLASSPATH=/data/local/tmp/scrcpy-server app_process \
        / com.genymobile.scrcpy.Server 1.14 info " + to_string(_resolution) + " " + to_string(_bitrate) +
        " 120 -1 false - true true 0 false false profile=1,level=1" + " > temp.txt";
    auto tmp = [](string cmd_use){
        //this_thread::sleep_for(chrono::milliseconds(5));
        system(cmd_use.c_str());
    };
    _device_server_thread = thread(tmp,cmd);

    //初始化本地socket
    string id ="127.0.0.1";//本地回环
    string port = "27183";
    if(!video_socket_init(id,port)){
        return false;
    }
    ifstream terminal_out("temp.txt");
    string terminal_result;
    string tmps;
    while(getline(terminal_out,tmps)){
        terminal_result += "\n" + tmps;
    }
    system("rm -rf temp.txt");    
    cout << terminal_result << endl;
    //判断是否启动成功
    //有ERROR出现说明启动失败
    regex result_rule("ERROR");
    cmatch m;
    if(regex_search(terminal_result.c_str(), m, result_rule)){
        return false;
    }
    return true;
}

//TODO:验证有效性
bool Server::stop_run_in_device(){
    //kill安卓端进程
    string cmd = "adb -s "+ _device_name  +  " shell ps | grep app_process";
    string t_out = run_shell(cmd);
    //获取pid
    regex pid_rule("[0-9]+");
    cmatch m;
    vector<string> pid_list;
   regex_search(t_out.c_str(), m, pid_rule);
   string pid = m.str();
   cmd = "adb -s "+ _device_name  +  " shell kill -9 " + pid; 
   system(cmd.c_str());    
    //释放socket端口
    shutdown(_video_socket,SHUT_RDWR);
    shutdown(_socket_fd,SHUT_RDWR);
    return true;
}

bool Server::video_socket_init(string id,string port){
    assert(! (id.empty() || port.empty()));
    //申请套接字描述符
    _socket_fd = socket(PF_INET, SOCK_STREAM, 0);//监听套接字
    if(_socket_fd < 0){
        cout << "socket创建失败" << endl;
        return false;
    }
    //将套接字描述符和端口进行绑定
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(port.c_str()));
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    const int on=1;
    //通过设置SO_REUSEADDR,可以在tcp连接断开之后立即重新连接
    setsockopt(_socket_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)); 
    
    if(bind(_socket_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        cout << "绑定失败" << endl;
        return false;
    }

    //监听对应端口信息
    if(listen(_socket_fd, 10) < 0){
        cout << "监听失败" << endl;
        return false;
    }

    //获取套接字
    struct sockaddr_in client_addr;
    socklen_t client_addrlength = sizeof(client_addr);
    _video_socket = accept(_socket_fd, (struct sockaddr*)&client_addr, &client_addrlength); //连接套接字
    if(_video_socket < 0){
        cout << "套接字获取失败" << endl;
        return false;
    }
    cout << "video socket初始化成功"<< endl << "连接客户端id为:" << inet_ntoa(client_addr.sin_addr) << endl;
    return true;
}

int Server::get_socket(){
    return _video_socket;
}


