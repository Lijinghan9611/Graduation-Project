#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ScreenBrocast.h"
#include "ScreenBrocast2.h"
#include <QScrollArea>
#include <QLibrary>
#include <QDir>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include "MyMessageBox.h"

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#include <windows.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(500);
    animation->setStartValue(0);
    animation->setEndValue(1);
    //animation->start();

    tray = new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/images/images/else/icon1.png"));
    // tray->show();
    tray->setToolTip("电子教室教学管理软件");
    trayMenu=new QMenu(this);
    exitiAction = new QAction("退出",this);
    showMainAction = new QAction("显示主界面",this);
    logoutAction = new QAction("切换账号",this);
    showMainAction->setIcon(QIcon(":/images/images/else/icon1.png"));
    exitiAction->setIcon(QIcon(":/images/images/button/close.ico"));
    logoutAction->setIcon(QIcon(":/images/images/button/logout.ico"));

    trayMenu->addAction(showMainAction);
    trayMenu->addAction(logoutAction);
    trayMenu->addAction(exitiAction);
    //connect(exitiAction,SIGNAL(triggered(bool)),this,SLOT(onTrayMenuClick()));
    connect(trayMenu,SIGNAL(triggered(QAction*)),this,SLOT(onTrayMenuClick(QAction*)));
    tray->setContextMenu(trayMenu);
    connect(tray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayClick(QSystemTrayIcon::ActivationReason)));

    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    setAttribute(Qt::WA_QuitOnClose,true);
    setWindowTitle("电子教室教学管理软件");
    resize(1280, 768);

    QWidget * MainWidget = new QWidget(this);
    pToolBar = new Tool_Bar();
    pLabelStyle = new LabelStyle();
    pMornitorView = new MornitorView;
    QScrollArea *area = new QScrollArea;

    setCentralWidget(MainWidget);

    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    if(set->value("ui/direction").toString() == "horizon")
        pMornitorView->setMinimumSize(200*pMornitorView->xLabelCount,200*pMornitorView->yLabelCount);
    else
        pMornitorView->setMinimumSize(200*pMornitorView->yLabelCount,200*pMornitorView->xLabelCount);
    delete set;
    area->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    area->setWidget(pMornitorView);

    MainWidget->setObjectName("MainWidget");
    pToolBar->setObjectName("ToolBar");
    pMornitorView->setObjectName("MornitorView");
    pLabelStyle->setObjectName("LabelStyle");
    area->setObjectName("ScrollArea");
    ui->statusBar->setObjectName("statusbar");
    labTeacherName = new QLabel("教师：", this);

    ui->statusBar->addPermanentWidget(labTeacherName); //现实永久信息
    ui->statusBar->showMessage(QString("   在线主机数  %1").arg(QString::number(Global::hostNum)));

    QVBoxLayout *pLayout = new QVBoxLayout();
    pLayout->addWidget(pToolBar);
    pLayout->addWidget(pLabelStyle);
    pLayout->addWidget(area);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, 0, 0, 0);
    MainWidget->setLayout(pLayout);

    connect(pToolBar,SIGNAL(setViewToScreen()),pLabelStyle,SLOT(onViewToScreen()));

    //net begin
    Server *server = new Server(this);
   // ScreenBrocast * udpThread = new ScreenBrocast;
   // ScreenBrocast2 * screenBrocast = new ScreenBrocast2;
   // connect(udpThread,SIGNAL(finished()),udpThread,SLOT(deleteLater()));
    //connect(udpThread,SIGNAL(started()),screenBrocast,SLOT(onSetSocket()));
    //connect(server,SIGNAL(setsocket(QTcpSocket*)),screenBrocast,SLOT(SetSocket(QTcpSocket*)));

    //接收文件
    connect(pToolBar,SIGNAL(startRecieveFileSignal()),server,SIGNAL(startRecieveFileSignal()));
    connect(pToolBar,SIGNAL(stopRecieveFileSignal()),server,SIGNAL(stopRecieveFileSignal()));

    //收到文件
    connect(server,SIGNAL(receiveFile(QString,QString)),pToolBar,SIGNAL(receiveFile(QString,QString)));


    //screen brocast
    //connect(pToolBar,SIGNAL(brocastBegin()),screenBrocast,SLOT(onStartScreenBrocast()));
    connect(pToolBar,SIGNAL(brocastBegin()),server,SIGNAL(brocastBegin()));
    //connect(pToolBar,SIGNAL(brocastEnd()),screenBrocast,SLOT(onStopScreenBrocast()));
    connect(pToolBar,SIGNAL(brocastEnd()),server,SIGNAL(brocastEnd()));
    connect(pLabelStyle,SIGNAL(brocastQuality(int)),server,SIGNAL(brocastQuality(int)));
   // udpThread->start();

    //initialize the style of label //client come
    connect(server,SIGNAL(checkStyle(QString)),pLabelStyle,SLOT(onCheckStyle(QString)));
    connect(pLabelStyle,SIGNAL(getStyle(QString,quint32)),server,SLOT(onGetStyle(QString,quint32)));
    connect(pLabelStyle,SIGNAL(getStyle(QString,quint32)),pMornitorView,SLOT(onSetLabelStyle(QString,quint32)));
    connect(server,SIGNAL(checkStyle(QString)),this,SLOT(onShowSituation()));
    connect(server,SIGNAL(checkStyle(QString)),pMornitorView,SIGNAL(clientCome(QString)));

    //change the style of label
    connect(pLabelStyle,SIGNAL(styleChange(quint32)),server,SLOT(onStyleChange(quint32)));
    connect(pLabelStyle,SIGNAL(styleChange(quint32)),pMornitorView,SLOT(onSetAllLabel(quint32)));
    connect(server,SIGNAL(styleChange1(QString,quint32)),pMornitorView,SLOT(onSetLabelStyle(QString,quint32)));

    //image come
    connect(server,SIGNAL(imageChange(QString,QImage)),pMornitorView,SLOT(onImageChange(QString,QImage)));

    //client leave
    connect(server,SIGNAL(leave(QString)),pMornitorView,SLOT(onLeave(QString)));
    connect(server,SIGNAL(leave(QString)),this,SLOT(onShowSituation()));
    connect(server,SIGNAL(leave(QString)),pMornitorView,SIGNAL(clientLeave(QString)));

    //zoom
    connect(pMornitorView,SIGNAL(zoomMornitor(QString)),server,SIGNAL(zoomMornitor(QString)));
    connect(server,SIGNAL(zoomImage(QString,QImage)),pMornitorView,SLOT(onZoomImageCome(QString,QImage)));
    connect(pMornitorView,SIGNAL(zoomEnd(QString)),server,SIGNAL(zoomEnd(QString)));

    //host message
    connect(server,SIGNAL(hostMg(QString,QString,QString)),pMornitorView,SLOT(onHostMg(QString,QString,QString)));

    //sign
    connect(pToolBar,SIGNAL(beginSign()),server,SIGNAL(beginSign()));
    connect(pToolBar,SIGNAL(retrieveSign()),server,SIGNAL(retrieveSign()));
    connect(server,SIGNAL(signMg(QString,QString,QString)),pToolBar,SIGNAL(signMg(QString,QString,QString)));
    connect(pToolBar,SIGNAL(signMgToolTip(QString,QString,QString)),pMornitorView,SLOT(onSignMgToolTip(QString,QString,QString)));

    // Q&A
    connect(pToolBar,SIGNAL(question(QString)),server,SIGNAL(question(QString)));
    connect(pToolBar,SIGNAL(getAnswer()),server,SIGNAL(getAnswer()));
    connect(server,SIGNAL(answerMg(QString,QString,QString,QString)),pToolBar,SIGNAL(answerMg(QString,QString,QString,QString)));

    //cmd
    connect(pToolBar,SIGNAL(cmd(QString,bool,QString)),server,SIGNAL(cmd(QString,bool,QString)));
    connect(server,SIGNAL(cmdResult(int)),pToolBar,SIGNAL(cmdResult(int)));
    connect(pMornitorView,SIGNAL(beginCmd(QString,QString,QString)),pToolBar,SLOT(onSingleCmd(QString,QString,QString)));
    connect(server,SIGNAL(cmdCallBack(QString)),pToolBar,SIGNAL(cmdCallBack(QString)));

    //close all or one computer
    connect(pToolBar,SIGNAL(closeComputer(QString)),server,SIGNAL(closeComputer(QString)));
    connect(pMornitorView,SIGNAL(closeComputer(QString)),server,SIGNAL(closeComputer(QString)));

    connect(pMornitorView,SIGNAL(reGetPosition(QString)),server,SIGNAL(reGetPosition(QString)));

    connect(pToolBar,SIGNAL(reStartComputer(QString)),server,SIGNAL(reStartComputer(QString)));
    connect(pMornitorView,SIGNAL(restartComputer(QString)),server,SIGNAL(reStartComputer(QString)));

    connect(pToolBar,SIGNAL(endProtect1()),server,SIGNAL(endProtect()));

    connect(pMornitorView,SIGNAL(msg(QString,QString)),server,SIGNAL(msg(QString,QString)));

    connect(pToolBar,SIGNAL(skinChange()),pMornitorView,SLOT(onSkinChange()));

    //关闭zoomlabel
    connect(pToolBar,SIGNAL(logoutSignal()),pMornitorView,SLOT(onCloseZoomLabel()));


    connect(pToolBar,SIGNAL(hideTray()),this,SLOT(onHideTray()));
    connect(pToolBar,SIGNAL(showTray()),this,SLOT(onShowTray()));


    connect(pMornitorView,SIGNAL(checkClientAttribute(QString)),server,SIGNAL(checkClientAttribute(QString)));
    connect(server,SIGNAL(clientAttribute(QString,QString,QString,QString)),pMornitorView,SLOT(onClientAttribute(QString,QString,QString,QString)));

    //transport file
    connect(pToolBar,SIGNAL(fileName(QString)),server,SIGNAL(fileName(QString)));
    connect(server,SIGNAL(transportFileCallBack()),pToolBar,SIGNAL(transportFileCallBack()));

    //portrait
    connect(pToolBar,SIGNAL(importPortrait()),server,SIGNAL(importPortrait()));
    connect(server,SIGNAL(iniPortrait(QString,QImage)),pToolBar,SIGNAL(iniPortrait(QString,QImage)));
    connect(pToolBar,SIGNAL(beginComparePortrait(int)),server,SIGNAL(beginComparePortrait(int)));
    connect(server,SIGNAL(comparePortrait(QString,QImage)),pToolBar,SIGNAL(comparePortrait(QString,QImage)));

    connect(pToolBar,SIGNAL(changeLoginTName(QString)),this,SLOT(onChangeTname(QString)));

    //更新客户端的id和姓名,labelview接收
    connect(server,SIGNAL(signMg(QString,QString,QString)),pMornitorView,SIGNAL(signMg(QString,QString,QString)));

    connect(this,SIGNAL(onlogoutFromAdmin()),pToolBar,SLOT(LogOut()));


    LocalIpList.clear();
    isFirst = true;


    //发送服务器ip
    sendServerUdpSocket = new QUdpSocket;
    sendServerTimer =new QTimer(this);
    sendServerPort = 4040;
    sendServerTimer->start(2000);
    connect(sendServerTimer, SIGNAL(timeout()), this, SLOT(onSendServerTimerTimeOut()));

}

void MainWindow::onChangeTname(const QString &Tname)
{
    labTeacherName->setText("教师："+Tname);
}
/**
* @brief 发送服务器ip
* @date 2017/12/6
* @return void
*/
void MainWindow::onSendServerTimerTimeOut()
{
    //Global::LocalIP = getIP();   //测试

    if(!Global::shutdownAutoGetIp)  //自动获取ip
        Global::LocalIP = getIP();
    if(isFirst && LocalIpList.size() > 1)  //只会在启动执行一次
    {
        isFirst = false;

        QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

        QString saveIP = set->value("config/ServerIP","").toString().trimmed();
        QStringList tempL;
        tempL.clear();
        if(!saveIP.isEmpty() && saveIP.contains("."))  //检查IP是否为正确格式IP
            tempL = saveIP.split(".");
        if(saveIP.isEmpty() || tempL.size() != 4)
        {
            MyMessageBox box;
            box.iniMsgBox(MyMessageBox::info_no_beep,"检测到电脑有两个局域网IP，请选择本教室的局域网IP，否则软件不能正常运行。");
            box.setTitle("请选择正确的局域网IP");
            box.setCbxDialig(true,LocalIpList);
            box.resize(350,200);
            if(box.exec())
            {
                Global::LocalIP = box.Msg;
                Global::shutdownAutoGetIp = true;  //关闭自动获取IP
                set->setValue("config/ServerIP",box.Msg);  //保存选择的IP
            }
            else
            {
                MyMessageBox showbox;
                showbox.iniMsgBox(MyMessageBox::info_no_beep,"没有选择IP，如果不能正常使用，请重启软件，选择正确的局域网IP。");
                showbox.isShowDialog(true);
                showbox.exec();
            }
        }
        else
        {
            Global::LocalIP = saveIP;
            Global::shutdownAutoGetIp = true;  //关闭自动获取IP
        }
    }
    //qDebug() << Global::LocalIP;
    sendServerUdpSocket->writeDatagram(Global::LocalIP.toLatin1(), Global::LocalIP.length(), QHostAddress::Broadcast, sendServerPort);
}

/**
* @brief 获取本机局域网IP
* @date 2017/12/
* @author han
* @return 本机ip
*/
QString MainWindow::getIP()
{
    QString ipAddr;

    QString tempIp192 = "";
    QString tempIp172 = "";
    QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
    LocalIpList.clear();
    foreach(QHostAddress address, AddressList)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol &&
                address != QHostAddress::Null &&
                address != QHostAddress::LocalHost){

            if (address.toString().contains("127.0.")){
                continue;
            }
            if (address.toString().contains("192.")){  //局域网IP192
                tempIp192 = address.toString();
                LocalIpList.append(tempIp192);
                // qDebug() << tempIp192;
            }
            else if (address.toString().contains("172.")){  //局域网IP172
                tempIp172 = address.toString();
                LocalIpList.append(tempIp172);
                // qDebug() << tempIp172;
            }
            else
                tempIp192 = address.toString();
            if(tempIp172 != "")  //如果有172的ip，则返回172的ip
                ipAddr = tempIp172;
            else                 //否则返回192的ip
                ipAddr = tempIp192;
        }
    }

    if (ipAddr.trimmed().isEmpty())  //都没有就返回本机地址127.0.0.1
        ipAddr = QHostAddress(QHostAddress::LocalHost).toString();
    return ipAddr;
}

void MainWindow::onShowSituation()
{
    ui->statusBar->showMessage(QString("   在线主机数  %1").arg(QString::number(Global::hostNum)));
}

void MainWindow::trayClick(QSystemTrayIcon::ActivationReason reason)
{
    if(reason==QSystemTrayIcon::Trigger)
    {
        this->show();
        this->activateWindow();
    }
}


/**
* @brief 托盘菜单点击
* @date 2018/1/31 修改
* @author han
* @param
* @return void
*/
void MainWindow::onTrayMenuClick(QAction*p)
{
    if(p == showMainAction)
    {
        this->show();
        this->activateWindow();
    }
    else if(p == logoutAction)
    {
        showLoginWindow();
    }
    else if(exitiAction == p)
    {
        exit(0);
    }

}


/**
* @brief 显示登录窗口
* @date 2018/1/19
* @author han
* @param
* @return void
*/
void MainWindow::showLoginWindow()
{
    pToolBar->LogOut();
}

void MainWindow::onHideTray()
{
    tray->hide();
}

void MainWindow::onShowTray()
{
    tray->show();
}

void MainWindow ::closeEvent(QCloseEvent *ev)
{
    ev->ignore();
    this->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}
