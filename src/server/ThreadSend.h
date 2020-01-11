#ifndef THREADSEND_H
#define THREADSEND_H

#include <QObject>
#include<QTcpSocket>
#include "Global.h"
#include <QTimer>

class ThreadSend : public QObject
{
    Q_OBJECT
public:
    explicit ThreadSend(QObject *parent = 0);
    QTcpSocket *socket;
    QString ip;
    void sendFile(QString filepath);
private:
    int quality;
    QTimer * timer;
signals:
    void styleChange(const QString&,quint32);

public slots:
    void onSetQuality(int i);
    void onTime();
    void SetSocket(QTcpSocket*);

    void onSendStyle(const QString&,quint32);  //style
    void onStyleChange(quint32);

    void onZoonMornitor(const QString&);  //zoom
    void onZoomEnd(const QString&);

    void onSignBegin();  //sign
    void onRetrieveSign();

    void onQuestion(const QString&); //A&Q
    void onGetAnswer();

    void onCmd(const QString&,bool,const QString&);  //cmd

    void onBrocastBegin();  //screen brocast
    void onBrocastEnd();

    void onCloseComputer(const QString&);
    void onReGetPosition(const QString&);
    void onReStartComputer(const QString&);
    void onEndProtect();
    void onMsg(const QString&,const QString&);
    void onCheckClientAttribute(const QString&);

    void onTransportFile(const QString&);

    void onImportPortrait();  //portrait
    void onBeginComparePortrait(int);
    void onStartRecFile();
    void onStopRecFile();
};

//struct packHead
//{
//    unsigned int allDataSize;
//    unsigned int headSize;
//    unsigned int PacketSize;  //current head+data
//    unsigned int count;
//    unsigned int currentIndex;
//    unsigned int offset;
//};

#endif // THREADSEND_H
