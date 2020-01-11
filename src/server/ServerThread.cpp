 #include "ServerThread.h"
#include <QDataStream>
#include <QImageReader>
#include <QBuffer>
#include <QHostAddress>
#include <QSettings>
#include <QMutex>
#include <QTimer>
#include <QTextCodec>
#include <QFile>
#include <QTextStream>


/**
* @brief 一个客户端对应一个线程
* @date 2018/1/25
* @author han
* @param
* @return void
*/
ServerThread::ServerThread(qintptr descritor)
{
    m_descriptor = descritor;
    dataSize = 0;
    userId = "";
    userName = "";
    connect(this,SIGNAL(clientcome()),this,SLOT(onConnected()),Qt::DirectConnection);  
}


//运行服务器线程
void ServerThread::run()
{
    socket = new QTcpSocket();
    timer = new QTimer();
    if(!socket->setSocketDescriptor(m_descriptor))
    {
        qDebug()<<"socker error";
        return;
    }
    emit clientcome();

    connect(socket,SIGNAL(readyRead()),this,SLOT(onReadyRead()),Qt::DirectConnection);
    connect(socket,SIGNAL(disconnected()),this,SLOT(onDisconnected()),Qt::DirectConnection);
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(onError(QAbstractSocket::SocketError)),Qt::DirectConnection);
    connect(timer,SIGNAL(timeout()),this,SLOT(onCheckStyle()),Qt::DirectConnection);

    exec();
}


void ServerThread::onConnected()
{
    qDebug()<<"connected";
    ip = QHostAddress(socket->peerAddress().toIPv4Address()).toString();
    emit reGetPosition(ip);  // solve first time client connect
    emit setsocket(socket);
    if(Global::isBroadcast)
        emit isBroadcast();

    QMutex mutex;
    mutex.lock();
    Global::hostNum ++;
    mutex.unlock();

    timer->start(1000);
}

//线程等待接受信息
void ServerThread::onReadyRead()           
{                                          
    while (socket->bytesAvailable()>0)
    {

        if(dataSize==0)
        {
            QDataStream stream(socket);
            stream.setVersion(QDataStream::Qt_5_0);
            if(socket->bytesAvailable()<sizeof(quint32)*2)
            {
                return;
            }
            stream>>dataSize;
            stream>>WM_User;  //接收命令
        }
        if(socket->bytesAvailable()<dataSize)
        {
            return;
        }
        switch(WM_User)
        {
        case WM_Word:
        {
            QByteArray str;
            str = socket->read(dataSize);
            qDebug()<<str;
            break;
        }
        case WM_Pic:  //接收到相片，监听客户端屏幕？
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
            emit imageChange(ip,img);
            break;
        }
        case WM_Position:  //收到客户机发来的位置信息，目前保存在config文件
        {
            QString str;
            str = socket->read(dataSize);
            QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

            QString key = "client/" + ip;

            set->setValue(key,str);
            delete set;
            break;
        }
        case WM_Zoom: //收到相片，未知操作
        {
            QByteArray str;
            str = socket->read(dataSize);
            QBuffer buf(&str);
            buf.open(QIODevice::ReadOnly);
            QImageReader reader(&buf,"PNG");
            QImage img = reader.read();
            emit zoomImage(ip,img);
            break;
        }
        case WM_HostMg:  //收到客户端ip，mac，主机名等信息
        {
            quint32 nameLengh,macLengh;
            QDataStream stream(socket);
            stream.setVersion(QDataStream::Qt_5_0);
            stream >>nameLengh>>macLengh;

            QString hostname , mac;
            hostname = socket->read(nameLengh);
            mac = socket->read(macLengh);
            emit hostMg(ip,mac,hostname);  //MornitorView中处理
            break;
        }
        case WM_SignMg:  //学生端签到？？signMg
        {
            quint32 idLengh,nameLengh;
            QDataStream stream(socket);
            stream.setVersion(QDataStream::Qt_5_0);
            stream >>idLengh>>nameLengh;
            QString id , name;
            id = socket->read(idLengh);
            name = socket->read(nameLengh);
            userId = id;
            userName = name;
            emit signMg(ip,id,name);  //传到toolbar
            break;
        }
        case WM_AnswerMg: //收到问题答案
        {
            QString str;
            str = socket->read(dataSize);
            emit answerMg(ip,userId,userName,str);
            break;
        }
        case WM_CmdResult:  //收到执行cmd命令的结果
        {
            int result;
            QDataStream stream(socket);
            stream.setVersion(QDataStream::Qt_5_0);
            stream >>result;
            emit cmdResult(result);
            break;
        }
        case WM_CmdCallBack:    //未知操作
        {
            QString str;
            str = socket->read(dataSize);
            emit cmdCallBack(str);
            break;
        }
        case WM_ReSendPosition:  //重新发送位置信息
        {
            QString str;
            str = socket->read(dataSize);
            QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

            QString key = "client/" + ip;
            set->setValue(key,str);
            delete set;
            emit checkStyle(ip);
            break;
        }
        case WM_ClientAttribute://客户端添加
        {
            QString str;
            str = socket->read(dataSize);
            QStringList l =str.split("|=+");
            if(l.size()!=4)
                break;
            emit clientAttribute(l.at(0),l.at(1),l.at(2),l.at(3));
            break;
        }
        case WM_TransportFileCallBack:  //文件传送结果
        {
            emit transportFileCallBack();
            break;
        }
        case WM_IniPortrait:  //接收到学生头像
        {
            QByteArray str;
            str = socket->read(dataSize);
            QBuffer buf(&str);
            buf.open(QIODevice::ReadOnly);
            QImageReader reader(&buf,"JPG");
            QImage img = reader.read();
            emit iniPortrait(ip,img);
            break;
        }
        case WM_BeginComparePortrait:  //头像对比
        {
            QByteArray str;
            str = socket->read(dataSize);
            QBuffer buf(&str);
            buf.open(QIODevice::ReadOnly);
            QImageReader reader(&buf,"JPG");
            QImage img = reader.read();
            emit comparePortrait(ip,img);
            break;
        }
        case WM_TransportFile:  //收到学生端的文件
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
            if(f.exists())  //发送文件接收结果
            {
                QByteArray data;
                QDataStream stream(&data,QIODevice::WriteOnly);
                stream.setVersion(QDataStream::Qt_5_0);
                stream<<(quint32)0<<(quint32)WM_TransportFileCallBack;
                socket->write(data);

                emit receiveFile(ip,AbsoluteFileName);
            }
            break;
        }

        default: break;
        }
     dataSize=0;
    // sleep(1);
    }
}


void ServerThread::onCheckStyle()
{
    emit checkStyle(ip);
    timer->stop();
}

void ServerThread::onDisconnected()
{
    emit leave(ip);
    QMutex mutex;
    mutex.lock();
    Global::hostNum--;
    mutex.unlock();

    socket->deleteLater();
    timer->deleteLater();
    socket= NULL;
    exit(0);
}

void ServerThread::onError(QAbstractSocket::SocketError error)
{
    qDebug()<<error;
}






