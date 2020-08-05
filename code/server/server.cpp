#include "server.h"

Server::Server(){
}

Server::~Server(){
}

bool Server::init(){
    int argc = 0;
    char *argv[1];
    QApplication _server_app(argc,argv);
    GamePro _server_window;
    _server_window.show();
    _server_app.exec();
    return true;
}
