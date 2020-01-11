#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <QDialog>
#include "CommonTitleBar.h"
#include <QLabel>
#include <QWidget>
#include <QTimer>
#include <QLineEdit>
#include <QComboBox>
namespace Ui {
class MyMessageBox;
}

class MyMessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit MyMessageBox(QWidget *parent = 0);
    ~MyMessageBox();
    //窗口类型
    enum BoxStyle{error,success,alert,info_no_beep,information};
    void iniMsgBox(const BoxStyle style=info_no_beep, const QString msg = "错误");
    void paintEvent(QPaintEvent*);
    void startHideTimer(int time);
    void isShowDialog(bool f);
    void setTitle(QString title);
    void setEnterDialog(bool flag);
    void setCbxDialig(bool flag, QStringList item, QString currentText = "");

    void setEchoModePwd();
    QString Msg;
    bool myDialogResult;
public:
    CommonTitleBar * bar;
    QLabel * pic;
    QLabel * text;

public slots:
    void onButtonClick();
    void onTimeOut();


private:
    Ui::MyMessageBox *ui;
    QPushButton * button;
    QPushButton * button2;
    QTimer *hideTimer;
    QLineEdit * leMsg;
    int totalTime;
    int timeOutTime;
    bool isEnterDialog;
    QComboBox *cbxValue;
    bool isCbxDialog;
};

#endif // MYMESSAGEBOX_H
