#ifndef SERVERWINDOW_H
#define SERVERWINDOW_H

#include <QMainWindow>
#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ServerWindow; }
QT_END_NAMESPACE

class ServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    ServerWindow(QWidget *parent = nullptr);
    ~ServerWindow();
    void set_app(QApplication* p_app);
    int get_socket(int* socket);
    
private slots:
    void on_ComboBox_bitrate_currentIndexChanged(const QString &arg1);

    void on_ComboBox_resolution_currentIndexChanged(const QString &arg1);

    void on_pushButton_start_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_refresh_clicked();

    void on_pushButton_find_config_clicked();

    void on_pushButton_apply_clicked();

    void on_comboBox_game_currentIndexChanged(const QString &arg1);


private:
    Ui::ServerWindow *ui;
    Server _server;
    //用于界面控制
    QApplication* _p_app;
    int* _psocket;
};
#endif // SERVERWINDOW_H
