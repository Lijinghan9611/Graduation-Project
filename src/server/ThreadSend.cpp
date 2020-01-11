#include "ThreadSend.h"
#include "Global.h"
#include <QHostAddress>
#include <QThread>
#include <QDataStream>
#include <QByteArray>
#include <QBuffer>
#include <QImageWriter>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include "Global.h"
#include <QScreen>
#include <QPainter>
#include <QGuiApplication>
#include <QBuffer>
#include <QImageWriter>
#include <QCursor>
#include <QLibrary>
#include "zlib.h"
typedef int(*com)(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);
com c1;

/**
* @brief 发送各种命令
* @date 2018/1/25
* @author han
* @param
* @return void
*/
ThreadSend::ThreadSend(QObject *parent) : QObject(parent)
{

    quality = 60;  //默认广播质量中等 : 60
#ifdef Q_OS_WIN32  //加载压缩工具
    QLibrary lib("x86/zlibwapi.dll");
    if (lib.load())
        c1 = (com)lib.resolve("compress");
#endif
#ifdef Q_OS_WIN64
    QLibrary lib2("x64/zlibwapi.dll");
    if (lib2.load())
        c1 = (com)lib2.resolve("compress");
#endif
}

void ThreadSend::SetSocket(QTcpSocket * s )
{
    socket = s;
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(onTime()));

    ip = QHostAddress(socket->peerAddress().toIPv4Address()).toString();

    if(Global::isReceivingFile) //如果正在接收文件
        onStartRecFile();
}


/**
* @brief 设置广播质量
* @date 2018/3/17
* @author han
* @param
* @return void
*/
void ThreadSend::onSetQuality(int i)
{
    quality = i;
}


//发送广播图片
void ThreadSend::onTime()
{
    QString cusorPath = ":/images/images/custor/mouseNormal.png";

    QImage custorImg(cusorPath);  //加载鼠标图片
    QPoint custorPosition = QCursor::pos();  //获取鼠标位置
    QScreen * screen = QGuiApplication::primaryScreen();
    QImage img;

    img = screen->grabWindow(0).toImage();  //获取截屏
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_SourceAtop);//设置图片覆盖模式，重要
    p.drawPixmap(custorPosition, QPixmap::fromImage(custorImg));  //在截屏上画出鼠标
    QBuffer buf;
    QImageWriter writer(&buf, "JPG");
    writer.setQuality(quality);  //设置图像格式的质量。quality的取值范围依赖于图像格式。例如：JPEG格式支持从0（低视觉质量，高压缩率）到100（高视觉质量，低压缩率）范围。
    writer.write(img);  //把截屏写入缓存

    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream << (quint32)buf.data().size() << (quint32)WM_BrocastPic;
    data.append(buf.data());
    socket->write(data);
    screen = NULL;
}


void ThreadSend::onSendStyle(const QString&, quint32 order)
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)order;
    socket->write(data);
}

void ThreadSend::onStyleChange(quint32 style)
{    
    emit styleChange(ip,style);
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)style;
    socket->write(data);
}

void ThreadSend::onZoonMornitor(const QString& str)
{
    if(str != ip)
        return;
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_Zoom;
    socket->write(data);
}

void ThreadSend::onZoomEnd(const QString& str)
{
    if(str != ip)
        return;
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_ZoomEnd;
    socket->write(data);
}

void ThreadSend::onSignBegin()
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_BeginSign;
    socket->write(data);
}

void ThreadSend::onRetrieveSign()
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_RetrieveSign;
    socket->write(data);
}

void ThreadSend::onQuestion(const QString& str)
{
    QBuffer buf ;
    buf.open(QIODevice::WriteOnly);
    buf.write(str.toUtf8());
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_Question;
    data.append(buf.data());
    socket->write(data);
}

void ThreadSend::onGetAnswer()
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_GetAnswer;
    socket->write(data);
}

void ThreadSend::onCmd(const QString& target,bool onlyone,const QString& str)
{
    if(target != "Any" && target != ip)
        return;
    QBuffer buf ;
    buf.open(QIODevice::WriteOnly);
    buf.write(str.toUtf8());
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)(buf.data().size()+sizeof(bool))<<(quint32)WM_Cmd<<onlyone;
    data.append(buf.data());
    socket->write(data);
}


/**
* @brief 开始广播
* @date 2018/3/17
* @author han
* @param
* @return void
*/
void ThreadSend::onBrocastBegin()
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_BrocastBegin;
    socket->write(data);
    qDebug() << "asdf1";
    timer->start();
}

/**
* @brief 结束广播
* @date 2018/3/17
* @author han
* @param
* @return void
*/
void ThreadSend::onBrocastEnd()
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_BrocastEnd;
    socket->write(data);
    timer->stop();
}

void ThreadSend::onCloseComputer(const QString & str)
{
    if(str != "Any" && str != ip)
        return;
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_CloseComputer;
    socket->write(data);
}

void ThreadSend::onReGetPosition(const QString & str)
{
    if(str != ip)
        return;
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_RegetPosition;
    socket->write(data);
}

void ThreadSend::onReStartComputer(const QString & str)
{
    if(str != "Any" && str != ip)
        return;
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_ReStartComputer;
    socket->write(data);
}


/**
* @brief from toolbar
* @date 2017/12/18
* @author han
* @return void
*/
void ThreadSend::onEndProtect()
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_EndProtect;
    socket->write(data);
}

void ThreadSend::onMsg(const QString &str, const QString &str2)
{
    if(str != ip)
        return;
    QBuffer buf ;
    buf.open(QIODevice::WriteOnly);
    buf.write(str2.toUtf8());
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)buf.data().size()<<(quint32)WM_Word;
    data.append(buf.data());
    socket->write(data);
}

void ThreadSend::onCheckClientAttribute(const QString &str)
{
    if(str != ip)
        return;
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_CheckClientAttribute;
    socket->write(data);
}


/**
* @brief 传输文件,信号来自传送文件窗口
* @date 2017/12/19
* @author han
* @return void
*/
void ThreadSend::onTransportFile(const QString & str)
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
* @brief
* @date 2018/1/
* @author han
* @param
* @return void
*/
void ThreadSend::onImportPortrait()
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_IniPortrait;
    socket->write(data);
}


/**
* @brief 开始人像对比
* @date 2018/1/
* @author han
* @param
* @return void
*/
void ThreadSend::onBeginComparePortrait(int)
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_BeginComparePortrait;
    socket->write(data);
}


/**
* @brief 开始接收文件,接收作业
* @date 2018/1/
* @author han
* @param
* @return void
*/
void ThreadSend::onStartRecFile()
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_RecieveFileStart;
    socket->write(data);
}

/**
* @brief 结束接收文件
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void ThreadSend::onStopRecFile()
{
    QByteArray data;
    QDataStream stream(&data,QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_5_0);
    stream<<(quint32)0<<(quint32)WM_RecieveFileEnd;
    socket->write(data);
}










