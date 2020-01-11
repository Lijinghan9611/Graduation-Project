#include "ScreenBrocast2.h"
#include "Global.h"
#include <QDebug>
#include <QThread>
#include <QScreen>
#include <QPainter>
#include <QGuiApplication>
#include <QBuffer>
#include <QImageWriter>
#include <QCursor>
#include <QLibrary>
#include "zlib.h"
typedef int(*com)(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);
com c;


/**
* @brief 暂时弃用,改为threadsend中发送
* @date 2018/3/17
* @author han
* @param
* @return void
*/
ScreenBrocast2::ScreenBrocast2(QObject *parent) : QObject(parent)
{
    multicastAddress = "225.20.40.20";  //组播IP地址,改为UDP广播
	port = 4565;   //屏幕广播的端口
    quality = 60;  //默认广播质量中等 : 60
#ifdef Q_OS_WIN32  //加载压缩工具
	QLibrary lib("x86/zlibwapi.dll");
	if (lib.load())
		c = (com)lib.resolve("compress");
#endif
#ifdef Q_OS_WIN64
	QLibrary lib2("x64/zlibwapi.dll");
	if (lib2.load())
		c = (com)lib2.resolve("compress");
#endif
}
void ScreenBrocast2::SetSocket(QTcpSocket * s)
{
    tcpSocket = s;
}

void ScreenBrocast2::onSetSocket()
{
	socket = new QUdpSocket;
	timer = new QTimer;
	connect(timer, SIGNAL(timeout()), this, SLOT(onTime()));
}

void ScreenBrocast2::onSetQuality(int i)
{
	quality = i;
}


void ScreenBrocast2::onStartScreenBrocast()
{
    timer->start(100);  //大概35帧
}

void ScreenBrocast2::start()
{
    timer->start(100);  //大概35帧
}

void ScreenBrocast2::onStopScreenBrocast()
{
	timer->stop();
}


//发送广播图片
void ScreenBrocast2::onTime()
{
    qDebug() << "asdf";
    // Qt::PointingHandCursor  Qt::ArrowCursor
    //Qt::CursorShape QCursor::shape() const
    //Returns the cursor shape identifier.The return value is one of the Qt::CursorShape enum values(cast to an int). 返回光标形状标识符。 返回值是Qt :: CursorShape枚举值之一（强制转换为int）。
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
    stream << (quint32)buf.data().size() << (quint32)WM_Pic;
    data.append(buf.data());
    tcpSocket->write(data);
    screen = NULL;
    qDebug()<<"adfadgadf";
    /*
	int dataLength = buf.size();
	unsigned char*destBuffer = (unsigned char*)buf.data().data();

	//处理包头
	int packetCount = 0;
	int lastPacketSize = 0;
	int currentIndex = 0;

	packetCount = dataLength / UDP_MAXSIZE;
	lastPacketSize = dataLength % UDP_MAXSIZE;
	if (lastPacketSize != 0)
		packetCount++;

	packHead head;
	head.headSize = sizeof(head);
	head.allDataSize = dataLength;
	head.count = packetCount;

	unsigned char frameBuffer[1024 * 100];
	memset(frameBuffer, 0, 1024 * 100);
	while (currentIndex < packetCount)  //发送
	{
		if (currentIndex < (packetCount - 1))   //before last packet
		{
			head.PacketSize = sizeof(head)+UDP_MAXSIZE;
			head.currentIndex = currentIndex;
			head.offset = currentIndex * UDP_MAXSIZE;
			currentIndex++;
			memcpy(frameBuffer, &head, sizeof(head));
			memcpy(frameBuffer + sizeof(head), destBuffer + head.offset, UDP_MAXSIZE);
			//socket->writeDatagram((char*)frameBuffer, head.PacketSize, multicastAddress, port);
			socket->writeDatagram((char*)frameBuffer, head.PacketSize, QHostAddress::Broadcast, port);
		}
		else
		{
			head.PacketSize = sizeof(head)+(dataLength - currentIndex * UDP_MAXSIZE);
			head.currentIndex = currentIndex;
			head.offset = currentIndex * UDP_MAXSIZE;
			memcpy(frameBuffer, &head, sizeof(head));
			memcpy(frameBuffer + sizeof(head), destBuffer + head.offset, dataLength - currentIndex*UDP_MAXSIZE);
			//socket->writeDatagram((char*)frameBuffer, head.PacketSize, multicastAddress, port);  //4565
			socket->writeDatagram((char*)frameBuffer, head.PacketSize, QHostAddress::Broadcast, port);
			break;
		}
    }
    */
}


ScreenBrocast2::~ScreenBrocast2()
{
	delete socket;
	//    delete destBuffer;
	delete timer;
}
