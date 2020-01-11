#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H
#include <QThread>
#include <QTcpSocket>
#include "Global.h"
#include <QHostInfo>
#include <QTimer>

class ServerThread : public QThread
{
    Q_OBJECT
public:
    ServerThread(qintptr descritor);
    qintptr m_descriptor;
    QTcpSocket *socket;
    QTimer * timer;  //prevent cheackstyle before client position message come(see mornitor.cpp onSetLabelStyle)

private:
    quint32 dataSize;
    quint32 WM_User;
    QString ip;
    QString userId ,userName;

protected:
    void run();

public slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError);
    void onCheckStyle();

signals:
    void clientcome();    //connected
    void setsocket(QTcpSocket*);
    void checkStyle(QString);
    void leave(QString);            //leave
    void imageChange(QString,const QImage&); //send imgage
    void zoomImage(QString,const QImage&); //zoom
    void hostMg(QString,QString,QString);  //hostMessage tooltip
    void signMg(QString,QString,QString);
    void answerMg(QString,QString,QString,QString);
    void cmdResult(int);  //all students cmd
    void cmdCallBack(QString); //for one student
    void reGetPosition(const QString&);  //solve config does not exit the position for morment ,see mornitorview onSetLabelStyle()
    void isBroadcast();
    void clientAttribute(const QString&,const QString&,const QString&,const QString&);
    void transportFileCallBack();
    void iniPortrait(const QString&,const QImage &);//portrait
    void comparePortrait(const QString&,const QImage &);
    void receiveFile(const QString&,const QString &);
};

#endif // SERVERTHREAD_H







