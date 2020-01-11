#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Sign.h"
#include "Question.h"
#include "BrocastLabel.h"
#include "stoast.h"
#include "MsgLabel.h"
#include <QMainWindow>
#include <QTcpSocket>
#include <QDebug>
#include <QImage>
#include <QTimer>
#include <QScreen>
#include <QHostAddress>
#include <QBuffer>
#include <QImageWriter>
#include <QDataStream>
#include <QCamera>
#include <QCameraImageCapture>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QLineEdit>
#include <QCloseEvent>
#include <QUdpSocket>
#include <QSystemTrayIcon>
#include <QMenu>
#include "login.h"
#include "fileinfodialog.h"

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#pragma comment(lib,"Advapi32.lib")
#include <windows.h>
#include <QImageReader>
#endif

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QTcpSocket *socket;
    QTimer * connectTimer;
    QTimer * screenTimer;
    QTimer * cameraTimer;
    QTimer * zoomTimer;
    QTimer * HostMgTimer;
    QTimer * protectTimer;
	QUdpSocket *findServerUdpSocket;  //查找服务器ip的udp
	int findServerPort;  //查找服务器ip的广播端口
    QString ip;
    quint16 port;
    QString position;
    quint32 dataSize,WM_User;
    QCamera * camera,*camera2,*camera3;
    QCameraImageCapture * cameraCapture,*cameraCapture2,*cameraCapture3;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     void sendPosition();
     void reSendPosition();
     void closeEvent(QCloseEvent*);
     void getAttribute();
     void sendTransportFileCallBack();
     void showLogin();

public slots:
    void onReadyRead();
    void onConnectToHost();
    void onConnected();
    void onDisConnect();
    void onError(QAbstractSocket::SocketError);
	void onRecieveServerIP();  //接收服务端ip
    void onTransportFile(const QString & str);

public slots:
    void onCatchScreen();
    void onCamera();
    void onCameraCapture(int,QImage);
    void onCameraCapture2(int,QImage);
    void onCameraCapture3(int,QImage);
    void onCameraError(QCamera::Error);
    void onZoom();
    void sendHostMg();   //make sure hostMg recieved on time by qtimer (no)
    void sendSignMg(QString="",QString="");
    void sendAnswer(QString="");
    void sendCmdResult(int); //all students
    void sendCmdCallBack(const QString&); // for one student
    void onProtect();
    void onTrayMenuClick(QAction*p);
    void onLogin(const QString& id, const QString& name);

signals:
    void cmd(bool,const QByteArray&); //send to cmdthread
    void connectToServer();
    void transportFileCallBack(); //文件发送结果
    void receiveFile(const QString &);

private:
    Ui::MainWindow *ui;
    Sign *sign;
    Question * question;
    BrocastLabel * brocastLabel;
    MsgLabel * msgLabel;
    bool endProtectFlag;

    QSystemTrayIcon *tray;
    QMenu *trayMenu;
    QAction *uploadFile;
    QAction *getFileAction;
    Login *login;

    QString studentId;
    QString studentName;
    FileInfoDialog *fileSendDialog;
    FileInfoDialog *fileGetDialog;

    bool isLostConnect;
};

#endif // MAINWINDOW_H
