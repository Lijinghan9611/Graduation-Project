#ifndef BROCASTTHREAD_H
#define BROCASTTHREAD_H
#include <QThread>
#include <QUdpSocket>

class BrocastThread :public QThread
{
    Q_OBJECT
public:
    BrocastThread();
    void run();
    ~BrocastThread();

private:
    QUdpSocket * socket;
    unsigned char * buffer;
  //  unsigned char * unCompressBuf;
    int index;
    unsigned long size;

public slots:
    void onReadyRead();

signals:
    void screenImg(const QImage&);
};

struct packHead
{
    unsigned int allDataSize;
    unsigned int headSize;
    unsigned int PacketSize;  //current head+data
    unsigned int count;
    unsigned int currentIndex;
    unsigned int offset;
};

#endif // BROCASTTHREAD_H
