#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H
#include "server_window.h"
#include <QApplication>

class ServerInterface
{
public:
    ServerInterface();
    ~ServerInterface();
    bool init();
    int get_socket();
private:
    int _socket_id;
    ServerWindow* _p_server_window; 
};

#endif // !SERVER_INTERFACE_H