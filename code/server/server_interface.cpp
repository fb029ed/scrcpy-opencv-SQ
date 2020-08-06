#include "server_interface.h"

ServerInterface::ServerInterface(){
}

ServerInterface::~ServerInterface(){
}

bool ServerInterface::init(){
    int argc = 0;
    char* argv[1];
    QApplication* p_app = new QApplication(argc,argv);
    _server_window.set_app(p_app);
    _server_window.show();
    p_app->exec();
    delete p_app;
    return true;
}

int ServerInterface::get_socket(){
    return _server_window.get_socket();
}
