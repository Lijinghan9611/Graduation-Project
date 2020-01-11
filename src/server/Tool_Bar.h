#ifndef TOOL_BAR_H
#define TOOL_BAR_H

#include "Global.h"
#include "CLassManage.h"
#include "CmdWidget.h"
#include "LogIn.h"
#include "AdminOption.h"
#include "MyButton.h"
#include "TranportFileWidget.h"
#include "SignIn.h"
#include <QWidget>
#include <QPaintEvent>
#include <QPushButton>
#include <QMouseEvent>

class Tool_Bar : public QWidget
{
    Q_OBJECT
public:
    explicit Tool_Bar(QWidget *parent=0);
    ~Tool_Bar();
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent*event);
    void mouseReleaseEvent(QMouseEvent*event);
    void mouseDoubleClickEvent(QMouseEvent *event);


public slots:
    void onButtonClick();
    void onSingleCmd(const QString&, const QString &userName, const QString &userID);  //cmd for only one
    void onClicked();
    void onMenuClick();
    void LogOut();  //退出登录
private:
    QPoint m_DragPosition;
    bool ispress;
    bool maxIsSet,normalIsSet;  //break death lock

    MyButton * pClassButton;
    MyButton * pCmdButton;
    MyButton * pScreenBrocastButton;
    MyButton * pTransportFile;
    MyButton * recieveFileButton;
    MyButton * pLogout;
    MyButton * editTeacherInfoBtn;


    CLassManage * pClassManage;
    CmdWidget * pCmdWidget;
    AdminOption * adminOption;
    TranportFileWidget *pTranportFileWidget;
    TranportFileWidget *recieveFileWidget;

    SignIn * editTeacherDialog;
    LogIn * login ;
    QAction * set ;
    QAction * skin;
    QAction * redBlack;
    QAction * blueWhite;
    QAction * logout;
    QAction * endProtect;
    QLabel *m_pIconLabel;
    QLabel *m_pTitleLabel;
    QPushButton *m_pMinimizeButton;
    QPushButton *m_pMaximizeButton;
    QPushButton *m_pCloseButton;
    QPushButton * m_pSetting;

signals:
    void receiveFile(const QString&,const QString &);  //收到学生端文件
    void startRecieveFileSignal();  //开始接收文件
    void stopRecieveFileSignal();  //结束接收文件

    void setViewToScreen();
    void changeLoginTName(const QString &);  //教师登录名
    void beginSign();  //sign
    void retrieveSign();
    void signMg(const QString&,const QString&,const QString&);
    void signMgToolTip(const QString&,const QString&,const QString&);

    void question(const QString&); //A&Q
    void getAnswer();
    void answerMg(const QString&,const QString&,const QString&,const QString&);

    void cmd(const QString&,bool,const QString&); //cmd
    void cmdResult(int);
    void cmdCallBack(const QString&);

    void brocastBegin();  //screen brocast
    void brocastEnd();

    void logoutSignal();
    void closeComputer(const QString&);  //one or all
    void reStartComputer(const QString&);
    void endProtect1();
    void skinChange();
    void hideTray();
    void showTray();
    void signEnd(const QString &, const QString &, const QString &, const QString &);//ini worklog

    void fileName(const QString &); //transport file
    void transportFileCallBack();

    void importPortrait();  //portrait
    void iniPortrait(const QString&,const QImage &);
    void beginComparePortrait(int);
    void comparePortrait(const QString&,const QImage &);

};

#endif // TOOL_BAR_H
