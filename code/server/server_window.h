#ifndef SERVER_WINDOW_H
#define SERVER_WINDOW_H
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class GamePro; }
QT_END_NAMESPACE

class GamePro : public QMainWindow
{
    Q_OBJECT

public:
    GamePro(QWidget *parent = nullptr);
    ~GamePro();

private slots:
    void on_pushButton_clicked();

    void on_bitRateComboBox_currentIndexChanged(const QString &arg1);

    void on_resolutionComboBox_currentIndexChanged(const QString &arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    Ui::GamePro *ui;
};
#endif // GamePro_H
