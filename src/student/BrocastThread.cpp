#include "BrocastThread.h"
#include <QDebug>
#include <QImage>
#include "zlib.h"
#include <QLibrary>

typedef int(*uncom)(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);
uncom u;

BrocastThread::BrocastThread()
{
    index = 0;
    size = 0;
#ifdef Q_OS_WIN32
     QLibrary lib("bin/x86/zlibwapi.dll");
     if(lib.load())
         u = (uncom)lib.resolve("uncompress");
#endif
#ifdef Q_OS_WIN64
     QLibrary lib2("bin/x64/zlibwapi.dll");
     if(lib2.load())
         u = (uncom)lib2.resolve("uncompress");
#endif
}


/**
* @brief 执行线程
* @date 2017/12/7
*/
void BrocastThread::run()
{
    socket = new QUdpSocket;
    //socket->bind(QHostAddress::AnyIPv4,4565,QUdpSocket::ShareAddress); 组播
   // socket->joinMulticastGroup(QHostAddress("225.20.40.20"));  //加入组播
	socket->bind(4565, QUdpSocket::ShareAddress);  //改为UDP广播
    buffer = new(unsigned char[1024*1024*2]);
    memset(buffer,0,1024*1024*2);
    connect(socket,SIGNAL(readyRead()),this,SLOT(onReadyRead()),Qt::DirectConnection);
    exec();
}

/**
* @brief 接收
* @date 2017/12/7
*/
void BrocastThread::onReadyRead()
{
    while(socket->hasPendingDatagrams())
    {
        qDebug() << "收到文件";
        QByteArray data;
        data.resize(socket->pendingDatagramSize());
        socket->readDatagram(data.data(),data.size());
        unsigned char *dataBuffer = (unsigned char*)data.data();
        packHead * head = (packHead*)data.data();
        if(head->currentIndex==index)
        {
            index ++;
            size += head->PacketSize - head->headSize;
            memcpy(buffer+head->offset,dataBuffer+head->headSize,(head->PacketSize-head->headSize));
            if(size == head->allDataSize && head->count == head->currentIndex+1)
            {
                QImage img;
                img.loadFromData((uchar*)buffer,(int)size,"JPG");
                //qDebug() << "接收完毕";
                emit screenImg(img);  //发出收到广播图片的信号，broacastlabel中显示
                index = 0;
                size = 0;
                memset(buffer,0,1024*1024*2);
            }
        }
        else
        {
            index = 0;
            size = 0;
        }

    }
}

BrocastThread::~BrocastThread()
{
    delete buffer;
    delete socket;
}
