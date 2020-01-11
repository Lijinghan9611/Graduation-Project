#ifndef SCREENBROCAST2_H
#define SCREENBROCAST2_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <QTcpSocket>

class ScreenBrocast2 : public QObject
{
    Q_OBJECT
public:
    explicit ScreenBrocast2(QObject *parent = 0);
    ~ScreenBrocast2();
    QUdpSocket *socket;
    QTcpSocket *tcpSocket;
    QTimer * timer;
    QHostAddress multicastAddress;
	int port;
    int quality;
    void start();

signals:

public slots:
    void onSetSocket();
    void onStartScreenBrocast();
    void onStopScreenBrocast();
    void onTime();
    void onSetQuality(int);
    void SetSocket(QTcpSocket*);
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


#endif // SCREENBROCAST2_H
