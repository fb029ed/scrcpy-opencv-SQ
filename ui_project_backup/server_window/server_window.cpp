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

}

void ServerWindow::on_pushButton_stop_clicked()
{

}

void ServerWindow::on_pushButton_refresh_clicked()
{

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
