#include "server_interface.h"

ServerInterface::ServerInterface(){
}

ServerInterface::~ServerInterface(){
    if(_p_server_window != nullptr)
        delete _p_server_window;
}

bool ServerInterface::init(){
    int argc = 0;
    char* argv[1];
    QApplication* p_app = new QApplication(argc,argv);
    _p_server_window = new ServerWindow;
    _p_server_window->set_app(p_app);
    //传入引用,在槽函数实现赋值
    _p_server_window->get_socket(&_socket_id);
    _p_server_window->show();
    p_app->exec();
    delete p_app;
    return true;
}

int ServerInterface::get_socket(){
    return _socket_id;
}
