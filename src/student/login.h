#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();
protected slots:
    void onConnected();  //已连接上教师端

private slots:
    void on_radioButton_clicked(bool checked);


    void on_radioButton_2_clicked(bool checked);
    void onTimeout();

    void on_pushButton_clicked();

signals:
    void loginSignal(const QString &,const QString &);

private:
    Ui::Login *ui;
    bool connected;
    QTimer *timer;

    int timeOutCount;
};

#endif // LOGIN_H
