#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "Global.h"
#include "CmdThread.h"
#include "CmdSend.h"
#include "BrocastThread.h"
#include<QSettings>
#include<QDebug>
#include <QScreen>
#include <QBuffer>
#include <QImageWriter>
#include<QDesktopWidget>
#include <QTextCodec>
#include <QLibrary>
#include <QDir>

#ifdef Q_OS_WIN
typedef bool (*startProtect)();
typedef bool (*stopProtect)();
typedef void (*startHide)();
typedef void (*stopHide)();
typedef bool (*startKeyBoard)(HINSTANCE);
typedef bool (*stopKeyBoard)();
#endif

static const int kMaxInfoBuffer = 256;
#define  GBYTES  1073741824
#define  MBYTES  1048576
#define  KBYTES  1024
#define  DKBYTES 1024.0


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dataSize = 0;
    endProtectFlag = false;
    //加载进程保护的库
    ui->label->append("正在加载进程保护");
#ifdef Q_OS_WIN32
    QLibrary lib("bin/pp.dll");
    if(lib.load())
    {
        ui->label->append("加载进程保护成功");
        startProtect beginProtect = (startProtect)lib.resolve("beginProtect");
        if(beginProtect != NULL)
            beginProtect();
    }
    else
        ui->label->append("加载进程保护失败");
#endif
#ifdef Q_OS_WIN64
    QLibrary lib2("bin/hp.dll");
    if(lib2.load())
    {
        startHide beginHide = (startHide)lib2.resolve("startHideProcess");
        if(beginHide != NULL)
            beginHide();
    }
#endif

    //托盘图标
    tray = new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/image/images/icon.ico"));
    tray->setToolTip("电子教室教学管理软件");
    trayMenu=new QMenu(this);
    uploadFile = new QAction("上传文件",this);
    uploadFile->setEnabled(false);
    getFileAction = new QAction("查看接收文件",this);
    trayMenu->addAction(uploadFile);
    trayMenu->addAction(getFileAction);
    getFileAction->setIcon(QIcon(":/image/images/getFile.png"));
    uploadFile->setIcon(QIcon(":/image/images/uploadfile.ico"));
    connect(trayMenu,SIGNAL(triggered(QAction*)),this,SLOT(onTrayMenuClick(QAction*)));
    tray->setContextMenu(trayMenu);
    tray->show();

    //文件收发窗口
    fileSendDialog = new FileInfoDialog;
    fileGetDialog = new FileInfoDialog;
    fileSendDialog->initDialog(0);
    fileGetDialog->initDialog(1);

    //收到文件
    connect(this,SIGNAL(receiveFile(QString)),fileGetDialog,SLOT(onReceiveFile(QString)));


    //登录界面
    login = new Login();
    connect(this,SIGNAL(connectToServer()),login,SLOT(onConnected()));
    connect(login,SIGNAL(loginSignal(QString,QString)),this,SLOT(onLogin(QString,QString)));

    //发送文件
    connect(fileSendDialog,SIGNAL(sendFileSignal(QString)),this,SLOT(onTransportFile(QString)));
    connect(this,SIGNAL(transportFileCallBack()),fileSendDialog,SLOT(onTransportFileCallBack()));

    //sign
    sign = new Sign;
    connect(sign,SIGNAL(signMg(QString,QString)),this,SLOT(sendSignMg(QString,QString)));
    //question
    question = new Question;
    connect(question,SIGNAL(AnswerMg(QString)),this,SLOT(sendAnswer(QString)));

    //brocast udp begin
    brocastLabel = new BrocastLabel;
    BrocastThread * brocastThread = new BrocastThread;
    connect(brocastThread,SIGNAL(finished()),brocastThread,SLOT(deleteLater()));
    connect(brocastThread,SIGNAL(screenImg(QImage)),brocastLabel,SLOT(onImgCome(QImage)),Qt::QueuedConnection);
    brocastThread->start();

    //cmd
    CmdThread * cmdThread  = new CmdThread();
    CmdSend * send = new CmdSend();
    send->moveToThread(cmdThread);
    connect(cmdThread,SIGNAL(finished()),cmdThread,SLOT(deleteLater()));
    connect(this,SIGNAL(cmd(bool,QByteArray)),send,SLOT(onCmd(bool,QByteArray)));
    connect(send,SIGNAL(cmdResult(int)),this,SLOT(sendCmdResult(int)));
    connect(send,SIGNAL(cmdCallBack(QString)),this,SLOT(sendCmdCallBack(QString)));
    cmdThread->start();

    //timer
    connectTimer = new QTimer(this);// 连接计时器，连接服务端
    screenTimer = new QTimer(this);  //屏幕发送计时器，用来计算
    cameraTimer = new QTimer(this);
    zoomTimer = new QTimer(this);
    HostMgTimer = new QTimer(this);
    protectTimer = new QTimer(this);
    connectTimer->start(5000);  //开启连接服务器的timer，待修改
    connect(connectTimer,SIGNAL(timeout()),this,SLOT(onConnectToHost()));  //链接槽
    connect(screenTimer,SIGNAL(timeout()),this,SLOT(onCatchScreen()));
    connect(cameraTimer,SIGNAL(timeout()),this,SLOT(onCamera()));
    connect(zoomTimer,SIGNAL(timeout()),this,SLOT(onZoom()));
    connect(HostMgTimer,SIGNAL(timeout()),this,SLOT(sendHostMg()));
    connect(protectTimer, SIGNAL(timeout()), this, SLOT(onProtect()));

    //查找服务器ip
    findServerUdpSocket = new QUdpSocket;
    findServerPort = 4040;
    findServerUdpSocket->bind(findServerPort, QUdpSocket::ShareAddress);
    connect(findServerUdpSocket, SIGNAL(readyRead()), this, SLOT(onRecieveServerIP()));

    //position 获取服务端的ip和端口
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    port = SERVER_PORT; //端口,此端口为接收命令的TCP端口
    position = set->value("position/position").toString();  //位置
    delete set;

    //tcp socket
    socket = new QTcpSocket(this);
    connect(socket,SIGNAL(connected()),this,SLOT(onConnected()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(onDisConnect()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onError(QAbstractSocket::SocketError)));

    //camera
    camera = new QCamera(this);
    camera2 = new QCamera(this);
    camera3 = new QCamera(this);
    cameraCapture = new QCameraImageCapture(camera);
    cameraCapture2 = new QCameraImageCapture(camera2);
    cameraCapture3 = new QCameraImageCapture(camera3);
    connect(cameraCapture,SIGNAL(imageCaptured(int,QImage)),this,SLOT(onCameraCapture(int,QImage)));
    connect(cameraCapture2,SIGNAL(imageCaptured(int,QImage)),this,SLOT(onCameraCapture2(int,QImage)));
    connect(cameraCapture3,SIGNAL(imageCaptured(int,QImage)),this,SLOT(onCameraCapture3(int,QImage)));
    connect(camera,SIGNAL(error(QCamera::Error)),this,SLOT(onCameraError(QCamera::Error)));
    //message
    msgLabel = new MsgLabel();

    isLostConnect = false;

    studentId = "";
    studentName = "";

}


/**
* @brief 登录,发送学号姓名
* @date 2018/1/24
* @author han
* @param
* @return void
*/
void MainWindow::onLogin(const QString & id, const QString &name)
{
    studentId = id;
    studentName = name;

    quint32 idLengh,nameLengh;
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);

    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    buf.write(id.toUtf8());
    idLengh = buf.size();
    buf.write(name.toUtf8());
    nameLengh = buf.size() - idLengh;

    stream<<(quint32)(buf.size()+sizeof(quint32)*2)<<(quint32)WM_SignMg<<idLengh<<nameLengh;
    data.append(buf.data());
    socket->write(data);
}

void MainWindow::showLogin()
{
    login->exec();
    sendPosition();
    sendHostMg();
}

void MainWindow::onTrayMenuClick(QAction*p)
{
    if(p == uploadFile)
    {
        fileSendDialog->show();
        fileSendDialog->activateWindow();
    }
    else if(p == getFileAction)
    {
        fileGetDialog->show();
        fileGetDialog->activateWindow();
    }
}

/**  
* @brief 接收服务端IP,用来建立TCP连接，接收教师机发过来的命令.随时改变，ip防止教师机重启
* @date 2017/12/8
* @return void
*/
void MainWindow::onRecieveServerIP()
{
    while (findServerUdpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(findServerUdpSocket->pendingDatagramSize());
        findServerUdpSocket->readDatagram(datagram.data(), datagram.size());
        if (datagram.contains("192.") || datagram.contains("172.")){  //局域网IP192, 172
            ip = datagram;  //把ip保存起来
        }
    }
}

/**
* @brief 定时连接到服务器
* @date 2017/12/8
* @return void
*/
void MainWindow::onConnectToHost()
{
    socket->connectToHost(ip,port);
    qDebug()<<"Connected time out serverIP:"+ ip;
}

/**
* @brief 连接上服务器后把定时器关闭
* @date 2017/12/8
* @return void
*/
void MainWindow::onConnected()
{
    connectTimer->stop();

    qDebug()<<"connected";
    if(isLostConnect)
    {
        isLostConnect = false;
        sendPosition();
        sendHostMg();
        if(studentId.isEmpty() || studentName.isEmpty())
        {
            onLogin(studentId, studentName);
        }
    }
    emit connectToServer();
}

void MainWindow::onReadyRead()
{

    while (socket->bytesAvailable()>0)
    {
        if(dataSize==0)
        {
            QDataStream stream(socket);
            stream.setVersion(QDataStream::Qt_5_0);
            if(socket->bytesAvailable()<(sizeof(quint32))*2)
                return;
            stream>>dataSize>>WM_User;
        }

        if(socket->bytesAvailable()<dataSize)
            return;
        switch(WM_User)
        {
        case WM_Common:
        {
            qDebug()<<"client common";
            cameraTimer->stop();
            screenTimer->stop();
            camera->stop();
            break;
        }
        case WM_Mornitor:  //收到显示客户端屏幕的命令
        {
            qDebug()<<"client mornitor";
            screenTimer->start(2000);
            cameraTimer->stop();
            camera->stop();
            break;
        }
        case WM_Camera:  //收到显示客户端摄像头的命令
        {
            camera->start();
            cameraTimer->start(1000);
            screenTimer->stop();
            qDebug()<<"client camara";
            break;
        }
        case WM_Zoom:
        {
            zoomTimer->start(300);
            qDebug()<<"client zoom";
            break;
        }
        case WM_ZoomEnd:
        {
            qDebug()<<"zoomEnd";
            zoomTimer->stop();
            break;
        }
        case WM_BeginSign: //点名开始
        {
            qDebug()<<"begin sign";
            sign->id->setFocus();
            sign->show();
            break;
        }
        case WM_RetrieveSign:
        {
            if(sign->isHidden())
                break;
            sendSignMg();
            break;
        }
        case WM_Question:  //收到提问
        {
            QString str;
            str = socket->read(dataSize);
            question->m_pQuestion->setText(str);
            question->m_pAnswer->setFocus();
            question->show();
            break;
        }
        case WM_GetAnswer:
        {
            if(question->isHidden())
                break;
            sendAnswer();
            break;
        }
        case WM_Cmd:
        {
            bool onlyone ;
            QByteArray str;
            QDataStream stream(socket);
            stream.setVersion(QDataStream::Qt_5_0);
            stream>>onlyone;
            str = socket->read(dataSize-sizeof(bool));
            emit cmd(onlyone,str);
            break;
        }
        case WM_BrocastPic:  //收到广播桌面图片
        {
            if(dataSize == 0)
            {
                //camera not exit
                break;
            }
            QByteArray str;
            str = socket->read(dataSize);
            QBuffer buf(&str);
            buf.open(QIODevice::ReadOnly);
            QImageReader reader(&buf,"JPG");
            QImage img = reader.read();
            QPixmap Pixmap = QPixmap::fromImage(img);
            Pixmap.scaled(brocastLabel->size(), Qt::KeepAspectRatio);
            brocastLabel->setPixmap(Pixmap);
            brocastLabel->setScaledContents(true);
            break;
        }
        case WM_BrocastBegin:  //开始广播
        {
#ifdef Q_OS_WIN
            protectTimer->start(500);
#endif
#ifdef Q_OS_WIN64
            QLibrary keyboard("bin/kb.dll");
            if(keyboard.load())
            {
                startKeyBoard beginKb = (startKeyBoard)keyboard.resolve("StartCatch");
                if(beginKb != NULL)
                {
                    beginKb(GetModuleHandleA(NULL));
                }
            }
#endif
#ifdef Q_OS_WIN32
            QLibrary keyboard2("bin/kb_32.dll");
            if(keyboard2.load())
            {
                startKeyBoard beginKb = (startKeyBoard)keyboard2.resolve("StartCatch");
                if(beginKb != NULL)
                {
                    beginKb(GetModuleHandleA(NULL));
                }
            }
#endif
            brocastLabel->show();   //显示label
            break;
        }
        case WM_BrocastEnd:  //结束广播
        {
            brocastLabel->hide();  //隐藏广播label
#ifdef Q_OS_WIN
            protectTimer->stop();
#endif
#ifdef Q_OS_WIN32
            QLibrary keyboard("bin/kb_32.dll");
            if(keyboard.load())
            {
                stopKeyBoard endKb = (stopKeyBoard)keyboard.resolve("StopCatch");
                if(endKb != NULL)
                {
                    endKb();
                }
            }
            keyboard.unload();
#endif
#ifdef Q_OS_WIN64
            QLibrary keyboard2("bin/kb.dll");
            if(keyboard2.load())
            {
                stopKeyBoard endKb = (stopKeyBoard)keyboard2.resolve("StopCatch");
                if(endKb != NULL)
                {
                    endKb();
                }
            }
            keyboard2.unload();
#endif
            break;
        }
        case WM_CloseComputer:  //收到关闭电脑的命令
        {
#ifdef Q_OS_WIN
            system("shutdown -s -f -t 0");
#else
            system("poweroff");
#endif
            break;
        }
        case WM_RegetPosition:  //收到重新发送位置的命令
        {
            reSendPosition();
            break;
        }
        case WM_ReStartComputer:   //收到重启命令
        {
#ifdef Q_OS_WIN
            system("shutdown -r -f -t 0");
#else
            system("reboot");
#endif
            break;
        }
        case WM_EndProtect:  //关闭进程保护
        {
            endProtectFlag = true;
#ifdef Q_OS_WIN32
            QLibrary lib("bin/pp.dll");
            if(lib.load())
            {
                stopProtect end = (stopProtect)lib.resolve("endProtect");
                if(end != NULL)
                {
                    end();
                }
                lib.unload();
            }
#endif
#ifdef Q_OS_WIN64
            QLibrary lib2("bin/hp.dll");
            if(lib2.load())
            {
                stopHide end = (stopHide)lib2.resolve("stopHideProcess");
                if(end != NULL)
                    end();
                lib2.unload();
            }
#endif
            break;
        }
        case WM_Word:  //收到教师端的发过来的信息
        {
            QString str;
            str = socket->read(dataSize);
            msgLabel->m_label->setText(str);
            msgLabel->show();
            break;
        }
        case WM_CheckClientAttribute:  //收到教师端查看客户端信息的命令
        {
            getAttribute();
            break;
        }
        case WM_TransportFile:  //收到文件
        {
            int nameSize ;
            QByteArray fileName,fileData;
            QString AbsoluteFileName;
            QDataStream stream(socket);
            stream.setVersion(QDataStream::Qt_5_0);
            stream>>nameSize;
            fileName = socket->read(nameSize);
            fileData = socket->read(dataSize - nameSize - sizeof(quint32));
#ifdef Q_OS_WIN
            QString path;
            if(QDir("D:/").exists())  //如果D盘存在
                path = "D:/RecvFile/";
            else
                path = "C:/RecvFile/";
            QDir *dir = new QDir(path);
            if(!dir->exists())  //目录不存在，创建
                dir->mkpath(path);
            AbsoluteFileName =  path + fileName;
#else
            QString path = "home/RecvFile/";
            QDir *dir = new QDir(path);
            if(!dir->exists())
                dir->mkpath(path);
            AbsoluteFileName =  path + fileName;
#endif
            QFile f(AbsoluteFileName);
            f.open(QFile::WriteOnly);
            f.write(fileData,fileData.size());
            f.close();
            if(f.exists())
            {
                emit receiveFile(AbsoluteFileName);
                msgLabel->m_label->setText("已接受教师文件，存储位置" +AbsoluteFileName);
                msgLabel->show();
                sendTransportFileCallBack();
            }
            break;
        }
        case WM_IniPortrait:
        {
            camera2->start();
            cameraCapture2->capture("2.jpg");
            break;
        }
        case WM_BeginComparePortrait:
        {
            camera3->start();
            cameraCapture3->capture("3.jpg");
            break;
        }
        case WM_TransportFileCallBack:  //文件传送结果
        {
            emit transportFileCallBack();
            break;
        }
        case WM_RecieveFileStart:  //教师端开始接收
        {

            fileSendDialog->show();
            uploadFile->setEnabled(true);
            break;
        }
        case WM_RecieveFileEnd:     //教师端结束接收文件
        {
            uploadFile->setEnabled(false);
            fileSendDialog->hide();
            break;
        }
        default:
            break;
        }
        dataSize=0;
    }  //end while
}

/**
* @brief 发送本机位置
* @date 2017/12/8
* @return void
*/
void MainWindow::sendPosition()
{
    QBuffer buf ;
    buf.open(QIODevice::WriteOnly);
    buf.write(position.toUtf8()); //读取本机主机名
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_Position;
    data.append(buf.data());
    socket->write(data);
}

/**
* @brief 重新发送本机位置
* @date 2017/12/8
* @return void
*/
void MainWindow::reSendPosition()
{
    QBuffer buf ;
    buf.open(QIODevice::WriteOnly);
    buf.write(position.toUtf8());
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_ReSendPosition;
    data.append(buf.data());
    socket->write(data);
}


/**
* @brief send local message:localname and mac
* @date 2017/12/6
* @param han
* @return void
*/
void MainWindow::sendHostMg()
{
    QString hostname = QHostInfo::localHostName(); //主机名
    QString mac;

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();  //获取本地mac

    foreach(QNetworkInterface in,interfaces)  //
    {
        if(in.flags().testFlag(QNetworkInterface::IsUp) && //mac地址获取
                in.hardwareAddress() != "" &&
                in.hardwareAddress() != "00:50:56:C0:00:08" &&
                in.hardwareAddress() != "00:50:56:C0:00:01" &&  //virtual mac
                in.hardwareAddress() != "00:00:00:00:00:00")
        {
            mac = in.hardwareAddress();
            break;
        }
    }

    quint32 nameLengh,macLength;
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);

    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    buf.write(hostname.toUtf8());
    nameLengh = buf.size();
    buf.write(mac.toUtf8());
    macLength = buf.size() - nameLengh;

    stream<<(quint32)(buf.size()+sizeof(quint32)*2)<<(quint32)WM_HostMg<<nameLengh<<macLength;
    data.append(buf.data());

    socket->write(data);
    HostMgTimer->stop();
}


/**
* @brief 获取截屏
* @date 2017/12/6
* @return void
*/
void MainWindow::onCatchScreen()
{
    QScreen * screen =QGuiApplication::primaryScreen();  //获取屏幕
    QImage sourceImg ,smallImg;
    sourceImg = screen->grabWindow(0).toImage();  //获取当前截屏
    smallImg = sourceImg.scaled(170, 120, Qt::IgnoreAspectRatio);  //截屏压缩

    //把截屏写入buf中
    QBuffer imageBuf ;
    QImageWriter writer(&imageBuf, "JPG");
    writer.write(smallImg);

    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream << (quint32)imageBuf.data().size() << (quint32)WM_Pic;
    data.append(imageBuf.data());

    socket->write(data);

    screen = NULL;
}

void MainWindow::onCamera()  //timer
{
    cameraCapture->capture("1.jpg");
}

/**
* @brief 捕捉摄像头并发送到服务端
* @date 2017/12/8
* @return void
*/
void MainWindow::onCameraCapture(int, QImage img)
{
    QImage smallImg = img.scaled(170,120,Qt::IgnoreAspectRatio);
    QBuffer buf ;
    QImageWriter writer(&buf,"JPG");
    writer.write(smallImg);
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_Pic;
    data.append(buf.data());
    socket->write(data);
}

/**
* @brief 捕捉摄像头并发送客户端
* @date 2017/12/8
* @return void
*/
void MainWindow::onCameraCapture2(int, QImage img)
{
    camera2->stop();
    QImage smallImg = img.scaled(170,120,Qt::IgnoreAspectRatio);
    QBuffer buf ;
    QImageWriter writer(&buf,"JPG");
    writer.write(smallImg);
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_IniPortrait;
    data.append(buf.data());
    socket->write(data);
}

/**
* @brief 捕捉摄像头并发送客户端
* @date 2017/12/8
* @return void
*/
void MainWindow::onCameraCapture3(int, QImage img)
{
    camera3->stop();
    QImage smallImg = img.scaled(170,120,Qt::IgnoreAspectRatio);
    QBuffer buf ;
    QImageWriter writer(&buf,"JPG");
    writer.write(smallImg);
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_BeginComparePortrait;
    data.append(buf.data());
    socket->write(data);
}

/**
* @brief 发送客户端截屏到服务端
* @date 2017/12/8
* @return void
*/
void MainWindow::onZoom()
{
    QScreen * screen =QGuiApplication::primaryScreen();
    QImage img ,smallImg;
    img = screen->grabWindow(0).toImage();

    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    int width = screenRect.width();
    int hight = screenRect.height();
    smallImg = img.scaled(width/1.5,hight/1.5,Qt::KeepAspectRatio);

    QBuffer buf ;
    QImageWriter writer(&buf,"PNG");
    writer.write(smallImg);
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_Zoom;
    data.append(buf.data());

    //qDebug()<<"zoom"<<data.size();
    socket->write(data);
    screen = NULL;
}

/**
* @brief 发送点名结果
* @date 2017/12/8
* @return void
*/
void MainWindow::sendSignMg(QString submitId,QString submitName)
{
    QString id ,name;
    QLabel * label = qobject_cast<QLabel*>(sender());  //转换信号类型，用来判断谁发送的
    if(label == sign)  //点名界面发来的
    {
        id = submitId;
        name = submitName;
    }
    else
    {
        id = sign->id->text();
        name = sign->name->text();
        sign->id->clear();
        sign->name->clear();
        sign->hide();
    }
    studentId = id;
    studentName = name;

    quint32 idLengh,nameLengh;
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);

    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    buf.write(id.toUtf8());
    idLengh = buf.size();
    buf.write(name.toUtf8());
    nameLengh = buf.size() - idLengh;

    stream<<(quint32)(buf.size()+sizeof(quint32)*2)<<(quint32)WM_SignMg<<idLengh<<nameLengh;
    data.append(buf.data());
    socket->write(data);
}

/**
* @brief 发送答案
* @date 2017/12/8
* @return void
*/
void MainWindow::sendAnswer(QString submit)
{
    QString str;
    QLabel * label = qobject_cast<QLabel*>(sender());
    if(label == question)
        str = submit;
    else
    {
        str =question->m_pAnswer->text();
        question->m_pAnswer->clear();
        question->m_pQuestion->clear();
        question->hide();
    }
    QBuffer buf ;
    buf.open(QIODevice::WriteOnly);
    buf.write(str.toUtf8());
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_AnswerMg;
    data.append(buf.data());
    socket->write(data);
}

/**
* @brief 回复执行cmd命令结果
* @date 2017/12/8
* @return void
*/
void MainWindow::sendCmdResult(int result)
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)sizeof(int)<<(quint32)WM_CmdResult<<result;
    socket->write(data);
}

/**
* @brief 收到CMD命令,回复确认
* @date 2017/12/8
* @return void
*/
void MainWindow::sendCmdCallBack(const QString& str)
{
    QBuffer buf ;
    buf.open(QIODevice::WriteOnly);
    buf.write(str.toUtf8());
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_CmdCallBack;
    data.append(buf.data());
    socket->write(data);
}

/**
* @brief unknown
* @date 2017/12/8
* @return void
*/
void MainWindow::onProtect()
{
#ifdef Q_OS_WIN
    HWND wnd= FindWindowA("#32770",NULL);
    if(IsWindowVisible(wnd))
        SendMessageA(wnd,WM_CLOSE,0,0);
#endif
}

/**
* @brief 获取本机信息,主机名，mac等
* @date 2017/12/8
* @return void
*/
void MainWindow::getAttribute()
{
    QString sysType,sysBit,memory,cpu;
#ifdef Q_OS_WIN
    SYSTEM_INFO system_info;
    memset(&system_info,0,sizeof(SYSTEM_INFO));
    GetSystemInfo(&system_info);
    OSVERSIONINFOEX os;
    os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
    if(GetVersionEx((OSVERSIONINFO *)&os))
    {
        switch(os.dwMajorVersion){
        case 4:
            //1996年7月发布
            switch(os.dwMinorVersion){
            case 0:
                if(os.dwPlatformId==VER_PLATFORM_WIN32_NT)
                    sysType="Microsoft Windows NT 4.0 ";
                else if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS)
                    sysType="Microsoft Windows 95 ";
                break;
            case 10:
                sysType="Microsoft Windows 98 ";
                break;
            case 90:
                sysType="Microsoft Windows Me ";
                break;
            }
            break;
        case 5:
            switch(os.dwMinorVersion){
            //1999年12月发布
            case 0:
                sysType="Microsoft Windows 2000 ";
                if(os.wSuiteMask==VER_SUITE_ENTERPRISE)
                    sysType.append("Advanced Server ");
                break;
                //2001年8月发布
            case 1:
                sysType="Microsoft Windows XP ";
                if(os.wSuiteMask==VER_SUITE_EMBEDDEDNT)
                    sysType.append("Embedded ");
                else if(os.wSuiteMask==VER_SUITE_PERSONAL)
                    sysType.append("Home Edition ");
                else
                    sysType.append("Professional ");
                break;
            case 2:
                if(os.wProductType==VER_NT_WORKSTATION &&
                        system_info.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
                    sysType="Microsoft Windows XP Professional x64 Edition ";
                if(GetSystemMetrics(SM_SERVERR2)==0 && os.wSuiteMask==VER_SUITE_BLADE)
                    sysType ="Microsoft Windows Server 2003 Web Edition ";
                else if(GetSystemMetrics(SM_SERVERR2)==0 && os.wSuiteMask==VER_SUITE_COMPUTE_SERVER)
                    sysType=("Microsoft Windows Server 2003 Compute Cluster Edition ");
                else if(GetSystemMetrics(SM_SERVERR2)==0 &&  os.wSuiteMask==VER_SUITE_STORAGE_SERVER)
                    sysType=("Microsoft Windows Server 2003 Storage Server ");
                else if(GetSystemMetrics(SM_SERVERR2)==0 &&   os.wSuiteMask==VER_SUITE_DATACENTER)
                    sysType=("Microsoft Windows Server 2003 Datacenter Edition ");
                else if(GetSystemMetrics(SM_SERVERR2)==0 &&   os.wSuiteMask==VER_SUITE_ENTERPRISE)
                    sysType=("Microsoft Windows Server 2003 Enterprise Edition ");
                else if(GetSystemMetrics(SM_SERVERR2)!=0 &&   os.wSuiteMask==VER_SUITE_STORAGE_SERVER)
                    sysType=("Microsoft Windows Server 2003 R2 Storage Server ");
                break;
            }
            break;
        case 6:
            switch(os.dwMinorVersion){
            case 0:
                if(os.wProductType==VER_NT_WORKSTATION)
                {
                    sysType="Microsoft Windows Vista ";
                    if (os.wSuiteMask==VER_SUITE_PERSONAL)
                        sysType.append("Home ");
                }
                else if(os.wProductType!=VER_NT_WORKSTATION)
                {
                    sysType="Microsoft Windows Server 2008 ";
                    if ( os.wSuiteMask==VER_SUITE_DATACENTER)
                        sysType.append("Datacenter Server ");
                    else if (os.wSuiteMask==VER_SUITE_ENTERPRISE)
                        sysType.append("Enterprise ");
                }
                break;
            case 1:
                if(os.wProductType==VER_NT_WORKSTATION)
                    sysType="Microsoft Windows 7 ";
                else
                    sysType="Microsoft Windows Server 2008 R2 ";
                break;
            }
            break;
        default:
            sysType="? ";
        }
    }

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
            si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
        sysBit.append("64位操作系统");
    else sysBit.append("32位操作系统");

    MEMORYSTATUSEX statusex;
    statusex.dwLength = sizeof(statusex);
    if (GlobalMemoryStatusEx(&statusex))
    {
        unsigned long long total = 0, remain_total = 0, avl = 0, remain_avl = 0;
        double decimal_total = 0, decimal_avl = 0;
        remain_total = statusex.ullTotalPhys % GBYTES;
        total = statusex.ullTotalPhys / GBYTES;
        avl = statusex.ullAvailPhys / GBYTES;
        remain_avl = statusex.ullAvailPhys % GBYTES;
        if (remain_total > 0)
            decimal_total = (remain_total / MBYTES) / DKBYTES;
        if (remain_avl > 0)
            decimal_avl = (remain_avl / MBYTES) / DKBYTES;

        decimal_total += (double) total;
        decimal_avl += (double) avl;
        char  buffer[kMaxInfoBuffer];
        sprintf_s(buffer,kMaxInfoBuffer, "%.2f GB (可用%.2f GB)", decimal_total, decimal_avl);
        memory.append(buffer);
    }

    std::string processor_name;
    LPCWSTR  str_path=L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0";
    HKEY key;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,str_path,0,KEY_ALL_ACCESS,&key) == ERROR_SUCCESS)
    {
        char processor_value[256];
        DWORD type = REG_SZ;
        DWORD value_size = sizeof(processor_value);
        if (RegQueryValueEx(key,L"ProcessorNameString",NULL,&type, (LPBYTE)&processor_value, &value_size) == ERROR_SUCCESS)
        {
            processor_name.append(processor_value,value_size);

        }
        RegCloseKey(key);
        int b = 0;
        for(int i = 0; i< processor_name.size() ;i++)
        {
            if(processor_name.at(i) != '\0')
            {
                cpu.append(processor_name.at(i));
                b ++;
            }
        }
    }
#else
    sysType = "Linux";
    sysBit = "64位操作系统";
    memory = "";
    cpu = "";
#endif

    QString str = sysType +"|=+"+ sysBit +"|=+"+ memory +"|=+"+ cpu;
    QBuffer buf ;
    buf.open(QIODevice::WriteOnly);
    buf.write(str.toUtf8());
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_ClientAttribute;
    data.append(buf.data());
    socket->write(data);
}

/**
* @brief 收到文件，回复确认
* @date 2017/12/8
* @return void
*/
void MainWindow::sendTransportFileCallBack()
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_TransportFileCallBack;
    socket->write(data);
}

/**
* @brief Tcp错误
* @date 2017/12/8
* @return void
*/
void MainWindow::onError(QAbstractSocket::SocketError error)
{
    qDebug()<<error;
}

/**
* @brief 摄像头捕捉错误
* @date 2017/12/8
* @return void
*/
void MainWindow::onCameraError(QCamera::Error)
{
    cameraTimer->stop();
    //add some tip
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_Pic;
    socket->write(data);
}

/**
* @brief 传输文件,信号来自传送文件窗口
* @date 2017/12/19
* @author han
* @return void
*/
void MainWindow::onTransportFile(const QString & str)
{
    QFile f(str);
    f.open(QFile::ReadOnly);
    QFileInfo fileInfo(f);
    QByteArray PurefileName = fileInfo.fileName().toUtf8();
    quint32 nameSize = PurefileName.size();

    QBuffer buf ;
    buf.open(QIODevice::WriteOnly);
    buf.write(f.readAll());

    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)(buf.data().size()+nameSize + sizeof(quint32))<<(quint32)WM_TransportFile<<nameSize;
    data.append(PurefileName);
    data.append(buf.data());
    socket->write(data);
}


/**
* @brief TCP连接关闭，教师端程序关闭，或者关机等情况
* @date 2017/12/8
* @return void
*/
void MainWindow::onDisConnect()
{
    SingleTonSToast::getInstance().setMessageVDuration("连接断开", 2000,1);
    qDebug()<<"disconnect from server,try to connet againt";
    socket->abort();
    connectTimer->start(5000);
    screenTimer->stop();
    cameraTimer->stop();
    camera->stop();
    isLostConnect = true;
}

/**
* @brief 关闭事件
* @date 2017/12/8
* @return void
*/
void MainWindow::closeEvent(QCloseEvent *)
{
    /*  if(!endProtectFlag)
        ev->ignore();
    else
        return QMainWindow::closeEvent(ev);*/
}

/**
* @brief 析构函数
* @date 2017/12/8
* @return void
*/
MainWindow::~MainWindow()
{
    delete sign;
    delete question;
    delete brocastLabel;
    delete msgLabel;
    delete ui;

#ifdef Q_OS_WIN32
    QLibrary lib("bin/pp.dll");
    if(lib.load())
    {
        stopProtect end = (stopProtect)lib.resolve("endProtect");
        if(end != NULL)
        {
            end();
        }
        lib.unload();
    }
#endif
#ifdef Q_OS_WIN64
    QLibrary lib2("bin/hp.dll");
    if(lib2.load())
    {
        stopHide end = (stopHide)lib2.resolve("stopHideProcess");
        if(end != NULL)
            end();
        lib2.unload();
    }
#endif
}





