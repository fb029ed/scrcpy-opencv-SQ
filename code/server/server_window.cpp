#include "server_window.h"
#include "ui_server_window.h"
GamePro::GamePro(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GamePro)
{
    ui->setupUi(this);
}

GamePro::~GamePro()
{
    delete ui;
}

void GamePro::on_pushButton_clicked()
{

}

void GamePro::on_bitRateComboBox_currentIndexChanged(const QString &arg1)
{

}

void GamePro::on_resolutionComboBox_currentIndexChanged(const QString &arg1)
{

}

void GamePro::on_pushButton_2_clicked()
{

}

void GamePro::on_pushButton_3_clicked()
{

}

void GamePro::on_pushButton_4_clicked()
{

}

void GamePro::on_pushButton_5_clicked()
{

}

void GamePro::on_comboBox_currentIndexChanged(const QString &arg1)
{

}
