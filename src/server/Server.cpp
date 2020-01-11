#include "Server.h"

Q_DECLARE_METATYPE(QString)

Server::Server(QWidget*parent) :QTcpServer(parent)
{
    int id = qRegisterMetaType<QString>("QString");
    Q_UNUSED(id);

    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);


    //此端口为发送命令的TCP端口
    //quint16 port = quint16(set->value("net/port").toInt());
    quint16 port = SERVER_PORT;

    QString ip = set->value("net/ip").toString();
    if(ip=="Any")
    {
        if(this->listen(QHostAddress::Any,port))
            qDebug()<<"listenning";
        else
        {
            qDebug()<<"listen error,plese cheack your network configuration";
            return;
        }
    }
    else
    {
        if(this->listen(QHostAddress(ip),port))
            qDebug()<<"listenning";
        else
        {
            qDebug()<<"listen error,plese cheack your network configuration";
            return;
        }
    }
    delete set;

}


void Server::onCheckStyle(const QString& str)
{
    emit checkStyle(str);
}

void Server::onGetStyle(const QString& str,quint32 i)
{
    emit getStyle(str,i);
}

void Server::onStyleChange(quint32 style)
{
    emit styleChange(style);
}

void Server::onStyleChange(const QString& str, quint32 style)
{
    emit styleChange1(str,style);
}

void Server::onImageChange(const QString& str, const QImage & img)
{
    emit imageChange(str,img);
}

void Server::incomingConnection(qintptr handle)
{
    thread = new ServerThread(handle);
    send = new ThreadSend();
    send->moveToThread(thread);


    //开始结束接收文件信号
    connect(this,SIGNAL(startRecieveFileSignal()),send,SLOT(onStartRecFile()));
    connect(this,SIGNAL(stopRecieveFileSignal()),send,SLOT(onStopRecFile()));

    //收到文件
    connect(thread,SIGNAL(receiveFile(QString,QString)),this,SIGNAL(receiveFile(QString,QString)));


    //set the socket of send which is equal to thread
    connect(thread,SIGNAL(setsocket(QTcpSocket*)),send,SLOT(SetSocket(QTcpSocket*)));
    connect(thread,SIGNAL(setsocket(QTcpSocket*)),this,SIGNAL(setsocket(QTcpSocket*)));

    //initialize label view
    connect(thread,SIGNAL(checkStyle(QString)),this,SLOT(onCheckStyle(QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(getStyle(QString,quint32)),send,SLOT(onSendStyle(QString,quint32)));

    //change the label view
    connect(this,SIGNAL(styleChange(quint32)),send,SLOT(onStyleChange(quint32)),Qt::QueuedConnection);
    connect(send,SIGNAL(styleChange(QString,quint32)),this,SLOT(onStyleChange(QString,quint32)));

    //Image come(label view)
    connect(thread,SIGNAL(imageChange(QString,QImage)),this,SLOT(onImageChange(QString,QImage)),Qt::QueuedConnection);

    //zoom
    connect(this,SIGNAL(zoomMornitor(QString)),send,SLOT(onZoonMornitor(QString)));
    connect(thread,SIGNAL(zoomImage(QString,QImage)),this,SIGNAL(zoomImage(QString,QImage)),Qt::QueuedConnection);
    connect(this,SIGNAL(zoomEnd(QString)),send,SLOT(onZoomEnd(QString)));

    //hostMessage(tooltip) 接收者为mainwindow
    connect(thread,SIGNAL(hostMg(QString,QString,QString)),this,SIGNAL(hostMg(QString,QString,QString)),Qt::QueuedConnection);

    //sign
    connect(this,SIGNAL(beginSign()),send,SLOT(onSignBegin()));
    connect(this,SIGNAL(retrieveSign()),send,SLOT(onRetrieveSign()));
    connect(thread,SIGNAL(signMg(QString,QString,QString)),this,SIGNAL(signMg(QString,QString,QString)),Qt::QueuedConnection);

    //Q&A
    connect(this,SIGNAL(question(QString)),send,SLOT(onQuestion(QString)));
    connect(this,SIGNAL(getAnswer()),send,SLOT(onGetAnswer()));
    connect(thread,SIGNAL(answerMg(QString,QString,QString,QString)),this,SIGNAL(answerMg(QString,QString,QString,QString)),Qt::QueuedConnection);

    //cmd
    connect(this,SIGNAL(cmd(QString,bool,QString)),send,SLOT(onCmd(QString,bool,QString)));
    connect(thread,SIGNAL(cmdResult(int)),this,SIGNAL(cmdResult(int)),Qt::QueuedConnection);
    connect(thread,SIGNAL(cmdCallBack(QString)),this,SIGNAL(cmdCallBack(QString)),Qt::QueuedConnection);

    //thread out
    connect(thread,SIGNAL(finished()),thread,SLOT(deleteLater()));
    connect(thread,SIGNAL(finished()),this,SLOT(onClientLeave()),Qt::QueuedConnection);
    connect(thread,SIGNAL(leave(QString)),this,SLOT(onClientLeave(QString)),Qt::QueuedConnection);

    //screen brocast
    connect(this,SIGNAL(brocastBegin()),send,SLOT(onBrocastBegin()));
    connect(thread,SIGNAL(isBroadcast()),send,SLOT(onBrocastBegin()));
    connect(this,SIGNAL(brocastEnd()),send,SLOT(onBrocastEnd()));
    connect(this,SIGNAL(brocastQuality(int)),send,SLOT(onSetQuality(int)));

    connect(this,SIGNAL(closeComputer(QString)),send,SLOT(onCloseComputer(QString)));

    connect(this,SIGNAL(reGetPosition(QString)),send,SLOT(onReGetPosition(QString)));
    connect(thread,SIGNAL(reGetPosition(QString)),send,SLOT(onReGetPosition(QString)));

    connect(this,SIGNAL(reStartComputer(QString)),send,SLOT(onReStartComputer(QString)));
    connect (this,SIGNAL(endProtect()),send,SLOT(onEndProtect()));
    connect(this,SIGNAL(msg(QString,QString)),send,SLOT(onMsg(QString,QString)));

    connect(this,SIGNAL(checkClientAttribute(QString)),send,SLOT(onCheckClientAttribute(QString)));
    connect(thread,SIGNAL(clientAttribute(QString,QString,QString,QString)),this,SIGNAL(clientAttribute(QString,QString,QString,QString)),Qt::QueuedConnection);

    connect(this,SIGNAL(fileName(QString)),send,SLOT(onTransportFile(QString)));
    connect(thread,SIGNAL(transportFileCallBack()),this,SIGNAL(transportFileCallBack()),Qt::QueuedConnection);

    //portrait
    connect(this,SIGNAL(importPortrait()),send,SLOT(onImportPortrait()));
    connect(thread,SIGNAL(iniPortrait(QString,QImage)),this,SIGNAL(iniPortrait(QString,QImage)),Qt::QueuedConnection);
    connect(this,SIGNAL(beginComparePortrait(int)),send,SLOT(onBeginComparePortrait(int)));
    connect(thread,SIGNAL(comparePortrait(QString,QImage)),this,SIGNAL(comparePortrait(QString,QImage)),Qt::QueuedConnection);

    thread->start();
    m_thread.append(thread);
}

void Server::onClientLeave()//防止由于上面finish信号和deletelater槽造成内存未初始化(野指针)
{
    ServerThread *thread = qobject_cast<ServerThread *>(sender());
    int index = m_thread.indexOf(thread);
    m_thread.removeAt(index);
    qDebug()<<"thread client leave";

}
//或者用QPointer.isnull
void Server::onClientLeave(const QString& str)
{
    emit leave(str);
    qDebug()<< str + "leave";
}


Server::~Server()
{
    //删除线程
    for(int i =0;i<m_thread.size();i++)
    {
        if(m_thread.at(i)->isRunning())
        {
            m_thread.at(i)->exit(0);
            m_thread.at(i)->wait();
            m_thread.at(i)->deleteLater();
            qDebug()<<"delete thread";
        }

    }
}



