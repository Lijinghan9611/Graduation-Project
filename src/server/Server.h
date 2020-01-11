#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QSettings>
#include <QWidget>
#include "ServerThread.h"
#include "ThreadSend.h"
#include <QList>
#include <QThread>
#include "Global.h"


class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QWidget*parent = 0);
    QList<QThread*>m_thread;
    ~Server();
    ServerThread * thread;
    ThreadSend *send;
public slots:
    void onClientLeave();
    void onClientLeave(const QString&);
    void onCheckStyle(const QString&);
    void onGetStyle(const QString&,quint32);
    void onStyleChange(quint32);
    void onStyleChange(const QString&,quint32);
    void onImageChange(const QString&,const QImage&);

signals:
    void brocastQuality(int);
    void setsocket(QTcpSocket*);
    void receiveFile(const QString&,const QString &);  //收到学生端文件
    void startRecieveFileSignal();  //开始接收文件
    void stopRecieveFileSignal();  //结束接收文件

    void checkStyle(const QString&);
    void getStyle(const QString&,quint32);
    void leave(const QString&);
    void styleChange(quint32);
    void styleChange1(const QString&,quint32);
    void imageChange(const QString&,const QImage&);

    void zoomMornitor(const QString&);
    void zoomImage(const QString&,const QImage&);
    void zoomEnd(const QString&);

    void hostMg(const QString&,const QString&,const QString&);  //hostMessage tooltip

    void beginSign();  // sign
    void retrieveSign();
    void signMg(const QString&,const QString&,const QString&);

    void question(const QString&);  //A&Q
    void getAnswer();
    void answerMg(const QString&,const QString&,const QString&,const QString&);

    void cmd(const QString&,bool,const QString&);  //cmd
    void cmdResult(int);
    void cmdCallBack(const QString&);

    void brocastBegin();  //screen brocast
    void brocastEnd();

    void closeComputer(const QString&);
    void reGetPosition(const QString&);
    void reStartComputer(const QString&);
    void endProtect();
    void msg(const QString&,const QString&);
    void checkClientAttribute(const QString&);
    void clientAttribute(const QString&,const QString&,const QString&,const QString&);

    void fileName(const QString &);//transport file
    void transportFileCallBack();

    void importPortrait();  //portrait
    void iniPortrait(const QString&,const QImage &);
    void beginComparePortrait(int);
    void comparePortrait(const QString&,const QImage &);

protected:
    void incomingConnection(qintptr handle);
};


#endif // SERVER_H
