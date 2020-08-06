#include "server_window.h"
#include "./ui_server_window.h"
ServerWindow::ServerWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ServerWindow)
{
    ui->setupUi(this);
}

ServerWindow::~ServerWindow()
{
    delete ui;
}

void ServerWindow::on_ComboBox_bitrate_currentIndexChanged(const QString &arg1)
{

}

void ServerWindow::on_ComboBox_resolution_currentIndexChanged(const QString &arg1)
{

}

void ServerWindow::on_pushButton_start_clicked()
{
    _server.start_by_step();
    //完成参数设置则退出设置界面
    *_psocket = _server.get_socket();
    _p_app->exit(0);
}

void ServerWindow::on_pushButton_stop_clicked()
{
    _server.stop_server();
    _p_app->exit(0); //退出当前界面
}

void ServerWindow::on_pushButton_refresh_clicked()
{
    //获取器件名称
    string device_name;
    _server.get_device_name(device_name);
    QString show = QString::fromStdString(device_name);
    ui -> lineEdit_device -> setText(show);
}

void ServerWindow::on_pushButton_find_config_clicked()
{

}

void ServerWindow::on_pushButton_apply_clicked()
{

}

void ServerWindow::on_comboBox_game_currentIndexChanged(const QString &arg1)
{

}

void ServerWindow::set_app(QApplication* p_app){
    _p_app = p_app;
}

int ServerWindow::get_socket(int* socket){
    _psocket = socket;
}