#include "MornitorView.h"
#include "Global.h"
#include "ui_ClientAtribute.h"
#include <QStyleOption>
#include <QPainter>
#include <QSettings>
#include <QDebug>
#include <QStringList>
#include <QHostInfo>
#include <MyMessageBox.h>

#include <QVBoxLayout>

MornitorView::MornitorView(QWidget*parent)  : QWidget(parent)
{
    //setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Expanding);
    mornitorState=true;
    cameraState=false;
    commonState = false;
    currentStyle = "else";
    UnknownClientCount = 0;
    addColumnCount = 0;
    isFirstAdd = true;
    UnknownClientPosition.clear();
    mainLayout = new QGridLayout(this);

    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    xLabelCount = set->value("ui/x").toInt();
    yLabelCount = set->value("ui/y").toInt();

    direction = set->value("ui/direction").toString();

    QString lack = set->value("ui/lack").toString();
    QStringList lackCharList = lack.split(" ");

    QString iniPoint = set->value("ui/iniPoint","A0").toString();
    if(iniPoint.size() != 2)
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::alert,"起始位置有误,请重新配置！");
        box.exec();
        return;
    }
    firstLeter = iniPoint.at(0).toLatin1();
    firstNum = iniPoint.at(1);

    foreach (QString str, lackCharList)
    {
        if(str.size()!=2)
            continue ;
        int x = str.at(0).toLatin1() - firstLeter;
        int y = QString(str.at(1)).toInt();
        int pos = x * xLabelCount + y;
        lackNumList << QString::number(pos);
    }

    delete set;

    //add labelview
    for(int i=0;i<yLabelCount;i++)
    {
        for(int j =0;j<xLabelCount;j++)
        {
            bool flag = false;
            //查找缺失
            foreach (QString str, lackNumList)
            {
                if(str.toInt() == i * xLabelCount + j+firstNum.toInt())
                {
                    flag = true;
                    break;
                }
            }

            addClientToLayout(i,j, flag);
        }
    }
    //zoom mornitor
    zoomLabel = new ZoomLabel;
    connect(zoomLabel,SIGNAL(zoomEnd()),this,SLOT(onZoomEnd()));
    sendMg = new SendMg ;
    sendMg->setWindowTitle("发送消息");
    connect(sendMg,SIGNAL(mg(QString)),this,SLOT(onSendMg2(QString)));
    clientAttribute = new ClientAtribute;
    connect(this,SIGNAL(clientCome(QString)),this,SLOT(onClientComeCheckExits(QString)));
}


/**
* @brief 添加客户端到layout显示
* @date 2018/1/23
* @author han
* @param
* @return void
*/
void MornitorView::addClientToLayout(int i, int j, bool isLack, QString unKnownPosition, QString ip)
{
    LabelView * labelview = new LabelView(this);
    QLabel * wordLabel = new QLabel(this);
    labelview->oldPositionX = j;

    QImage img(":/images/images/else/offline.ico");
    labelview->setPixmap(QPixmap::fromImage(img));
    labelview->setToolTip("该电脑本节课未使用.");

    char word =firstLeter+i;  //座位号字母加行数

    QString position= word+ QString::number(j+firstNum.toInt());

    //添加未知主机
    if(unKnownPosition != "")
    {
        if(currentStyle == "common")
        {
            labelview->setStyleSheet("LabelView{background-color:transparent;color:white;}");
            QImage img(":/images/images/else/online.ico");
            labelview->setPixmap(QPixmap::fromImage(img));
            labelview->setFixedSize(100,100);
        }
        else
        {
            if(Global::skinFileName == "redBlack.qss")
                labelview->setStyleSheet("LabelView{background-color:black;color:white;}");
            else
                labelview->setStyleSheet("LabelView{background-color:lightblue;color:white;}");
            labelview->setText(unKnownPosition);
            labelview->setFixedSize(170,120);
        }
        labelview->setToolTip("已连接.");

        position = unKnownPosition;  //更改位置
        UnknownClientPosition.append(position);
        labelview->ip = ip;
        labelview->onClientCome(ip);
    }
    wordLabel->setText(position);
    wordLabel->setAlignment(Qt::AlignCenter);
    wordLabel->setMinimumWidth(80);
    labelview->newPosition = position;
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(labelview);
    layout->addWidget(wordLabel);
    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    widget->setMinimumHeight(180);  //设置最小高度，布局自动增高
    labelview->setAlignment(Qt::AlignCenter);
    if(direction == "horizon")
    {
        mainLayout->addWidget(widget,i,j,1,1,Qt::AlignHCenter);
    }
    else
    {
        mainLayout->addWidget(widget,j,i,1,1,Qt::AlignHCenter);
    }
    m_map.insert(position,labelview);
    word_map.insert(position,wordLabel);

    if(isLack)  //将缺失的隐藏
    {
        labelview->hide();
        wordLabel->hide();
    }

    //隐藏和显示label
    connect(labelview,SIGNAL(hideLabel(QString,bool)),this,SLOT(onHideLabel(QString,bool)));
    //double click zoom mornitor
    connect(labelview,SIGNAL(mornitor(QString)),this,SLOT(onMornitor(QString)));
    // cmd for one
    connect(labelview,SIGNAL(beginCmd(QString,QString,QString)),this,SIGNAL(beginCmd(QString,QString,QString)));
    // close or restart pc
    connect(labelview,SIGNAL(closeComputer(QString)),this,SIGNAL(closeComputer(QString)));
    connect(labelview,SIGNAL(restartComputer(QString)),this,SIGNAL(restartComputer(QString)));
    //send message
    connect(labelview,SIGNAL(msg(QString)),this,SLOT(onSendMg(QString)));
    //client data(online time ,offline time)
    connect(this,SIGNAL(clientCome(QString)),labelview,SLOT(onClientCome(QString)));
    connect(this,SIGNAL(clientLeave(QString)),labelview,SLOT(onClientLeave(QString)));
    //check client attritube
    connect(labelview,SIGNAL(cheakClient(QString,QString,QString)),this,SLOT(onCheckClientMg(QString,QString,QString)));
    //更新客户端的id和姓名,labelview接收
    connect(this,SIGNAL(signMg(QString,QString,QString)),labelview,SLOT(onSignMg(QString,QString,QString)));
}


/**
* @brief 检查是否为主机号不明确的主机
* @date 2018/1/23
* @author han
* @param
* @return void
*/
void MornitorView::onClientComeCheckExits(const QString &ip)
{
    QString key = "client/"+ip;
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    QString position = set->value(key,"").toString().trimmed();
    QLabel * word_label = word_map.value(position,NULL);
    if(word_label == NULL && position != "")  //不存在
    {
        //计算位置
        int row = UnknownClientCount / xLabelCount;
        int i =  row+ yLabelCount;
        int j = UnknownClientCount % xLabelCount;
        UnknownClientCount++;  //命名不准确主机+1
        addClientToLayout(i,j,false,position,ip);
    }
}

/**
* @brief 隐藏label
* @date 2018/1/23
* @author han
* @param
* @return void
*/
void MornitorView::onHideLabel(const QString &position, const bool &flag)
{
    QLabel * word_label = word_map.value(position,NULL);
    if(word_label == NULL)
        return;
    if(flag)
    {
        word_label->hide();
    }
    else
    {
        word_label->show();
    }
}

/**
* @brief 关闭zoomLabel
* @date 2018/1/8
* @author han
* @return void
*/
void MornitorView::onCloseZoomLabel()
{
    zoomLabel->onClick();
}
void MornitorView::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(ev);
}


/**
* @brief 改变全部
* @date 2018/1/4
* @author han
* @return void
*/
void MornitorView::onSetAllLabel(const quint32 & order)  //change all
{
    switch(order)
    {
    case WM_Common:
        currentStyle = "common";
        for(int i = 0 ; i < yLabelCount ; i ++)
        {
            for(int j = 0 ; j < xLabelCount ; j ++)
            {
                char word =firstLeter+i;
                QString position= word+ QString::number(j+firstNum.toInt());
                if(m_map.value(position,NULL) == NULL)
                    continue;
                m_map.value(position)->setFixedSize(100,100);
                m_map.value(position)->clear();
                m_map.value(position)->setStyleSheet("LabelView{background-color:transparent;color:white;}");
                QImage img(":/images/images/else/offline.ico");
                m_map.value(position)->setPixmap(QPixmap::fromImage(img));
            }
        }
        //改变位置不明确主机的样式
        foreach(QString position1,UnknownClientPosition)
        {

            if(m_map.value(position1,NULL) == NULL)
                continue;
            m_map.value(position1)->setFixedSize(100,100);
            m_map.value(position1)->clear();
            m_map.value(position1)->setStyleSheet("LabelView{background-color:transparent;color:white;}");
            QImage img(":/images/images/else/offline.ico");
            m_map.value(position1)->setPixmap(QPixmap::fromImage(img));
        }
        break;
    case WM_Camera:
    case WM_Mornitor :
        currentStyle = "else";
        for(int i = 0 ; i < yLabelCount ; i ++)
        {
            for(int j = 0 ; j < xLabelCount ; j ++)
            {
                char word =firstLeter+i;
                QString position= word+ QString::number(j+firstNum.toInt());
                if(m_map.value(position,NULL) == NULL)
                    continue;
                m_map.value(position)->setFixedSize(170,120);
                m_map.value(position)->clear();
                if(Global::skinFileName == "redBlack.qss")
                    m_map.value(position)->setStyleSheet("LabelView{background-color:black;color:white;}");
                else
                    m_map.value(position)->setStyleSheet("LabelView{background-color:lightblue;color:white;}");
                m_map.value(position)->setText(m_map.value(position)->newPosition);
            }
        }
        //改变位置不明确主机的样式
        foreach(QString position1,UnknownClientPosition)
        {

            if(m_map.value(position1,NULL) == NULL)
                continue;
            m_map.value(position1)->setFixedSize(170,120);
            m_map.value(position1)->clear();

            if(Global::skinFileName == "redBlack.qss")
                m_map.value(position1)->setStyleSheet("LabelView{background-color:black;color:white;}");
            else
                m_map.value(position1)->setStyleSheet("LabelView{background-color:lightblue;color:white;}");
            m_map.value(position1)->setText(m_map.value(position1)->newPosition);
        }
        break;
    default :break;
    }
}

//change single
void MornitorView::onSetLabelStyle(const QString& str,quint32 order) // initialize labelview and button click
{
    QString key = "client/"+str;
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);


    //for first time client come config not exit position
    QString position;
    position = set->value(key,"buffer").toString();
    delete set;
    if(position == "buffer")
    {
        emit reGetPosition(str);
        return;
    }
    switch (order)
    {
    case WM_Common:
    {
        QImage img;
        img.load(":/images/images/else/online.ico");
        img.scaled(170,120,Qt::KeepAspectRatio);
        LabelView * m_label = m_map.value(position,NULL);
        if(m_label == NULL)
            return;
        m_label->setPixmap(QPixmap::fromImage(img));
        m_label->on = true;
        mornitorState = false;
        cameraState = false;
        commonState = true;
        break;
    }
    case WM_Mornitor:
    {
        LabelView * m_label = m_map.value(position,NULL);
        if(m_label == NULL)
            return;
        m_label->on = true;
        mornitorState = true;
        cameraState = false;
        commonState = false;
        break;
    }
    case WM_Camera:
    {
        LabelView * m_label = m_map.value(position,NULL);
        if(m_label == NULL)
            return;
        m_label->on = true;
        commonState=false;
        cameraState = true;
        mornitorState = false;
        break;
    }
    default:break;
    }

}


/**
* @brief 收到客户端发来的桌面图
* @date 2018/1/24
* @author han
* @param
* @return void
*/
void MornitorView::onImageChange(const QString& str, const QImage & img)
{
    if(commonState)
        return;
    QString key = "client/"+str;
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    QString position = set->value(key).toString();
    delete set;
    LabelView * m_label = m_map.value(position,NULL);
    if(m_label == NULL)
        return;
    m_label->setPixmap(QPixmap::fromImage(img));
    m_label->on = true;
}

//收到客户端的ip，mac，主机名信息
void MornitorView::onHostMg(const QString& ip, const QString& mac, const QString& name)
{
    QString key = "client/"+ip;
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    QString position = set->value(key).toString();
    delete set;

    LabelView * m_label = m_map.value(position,NULL);  //获取学生机的位置
    if(m_label == NULL)
        return;
    m_label->setToolTip("计算机名:  " + name + "\n\n" + "IP地址:  " + ip
                        + "\n\n" +"物理地址:  " + mac );
    m_label->ip = ip;
    m_label->mac = mac;
    m_label->name = name;
}


/**
* @brief 收到点名信息,设置显示学生的姓名信息
* @date 2018/1/4
* @author han
* @return void
*/
void MornitorView::onSignMgToolTip(const QString& ip, const QString& id, const QString& name)
{
    QString key = "client/"+ip;
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    QString position = set->value(key).toString();

    delete set;
    LabelView * m_label = m_map.value(position,NULL);
    if(m_label == NULL)
        return;
    QString tooltip = m_label->toolTip();
    m_label->setToolTip(tooltip+"\n\n"+QString("学号: %1\n\n").arg(id)
                        +QString("姓名: %1").arg(name));
    m_label->userId = id;
    m_label->userName = name;

    //显示姓名
    QLabel * word_label = word_map.value(position,NULL);
    if(word_label == NULL)
        return;
    QString info = position + " " + name;
    word_label->setText(info);
}

void MornitorView::onSendMg(const QString &str)
{
    sendMgIp = str;
    sendMg->move(cursor().pos().x()-40,cursor().pos().y()-50);
    sendMg->show();
    sendMg->activateWindow();
}

void MornitorView::onSendMg2(const QString &text)
{
    emit msg(sendMgIp,text);
}


/**
* @brief 放大显示学生机桌面
* @date 2018/1/8
* @author han
* @return void
*/
void MornitorView::onMornitor(const QString& ip)     //zoom
{
    emit zoomMornitor(ip);
    zoomIp = ip;
    zoomLabel->show();
}


/**
* @brief 收到学生机发来的桌面图
* @date 2018/1/8
* @author han
* @return void
*/
void MornitorView::onZoomImageCome(const QString& str,const QImage &img)
{
    if(str!=zoomIp)   //prevent when zoomlabel not end , choose another ip to zoom
    {
        emit zoomEnd(str);
        return;
    }
    zoomLabel->setPixmap(QPixmap::fromImage(img));
}

void MornitorView::onZoomEnd()
{
    emit zoomEnd(zoomIp);
}

void MornitorView::onLeave(const QString& str)
{
    QString key = "client/"+str;
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    QString position = set->value(key).toString();
    delete set;

    LabelView * m_label = m_map.value(position,NULL);
    if(m_label == NULL)
        return;
    if(commonState)
    {
        QImage img(":/images/images/else/offline.ico");
        m_label->setPixmap(QPixmap::fromImage(img));
    }
    else
    {
        m_label->clear();
        m_label->setText(position);
    }

    m_label->on=false;

    QString tooltip = m_label->toolTip();
    m_label->setToolTip(tooltip + "\n\n" + "已下线");

    //显示机位号
    QLabel * word_label = word_map.value(position,NULL);
    if(word_label == NULL)
        return;
    word_label->setText(position);
}

void MornitorView::onSkinChange()
{
    if(currentStyle =="common")
        return;
    if(Global::skinFileName == "redBlack.qss")
        foreach (LabelView* labelv, m_map)
        {
            labelv->setStyleSheet("LabelView{background-color:black;color:white;}");
        }
    else
        foreach (LabelView* labelv, m_map)
        {
            labelv->setStyleSheet("LabelView{background-color:lightblue;color:white;}");
        }
}

void MornitorView::onCheckClientMg(const QString &str,const QString& str2,const QString& str3)
{
    clientIp =str;
    clientMac = str2;
    clientName = str3;
    clientAttribute->show();
    clientAttribute->activateWindow();
    emit checkClientAttribute(str);
}


void MornitorView::onClientAttribute(const QString &sysType, const QString &sysBit, const QString &memory, const QString &cpu)
{
    clientAttribute->ui->label_8->setText(sysType);
    clientAttribute->ui->label_9->setText(cpu);
    clientAttribute->ui->label_10->setText(memory);
    clientAttribute->ui->label_11->setText(sysBit);
    clientAttribute->ui->label_12->setText(clientName);
    clientAttribute->ui->label_13->setText(clientIp);
    clientAttribute->ui->label_14->setText(clientMac);
}

MornitorView::~MornitorView()
{
    delete zoomLabel;
    delete sendMg;
    delete clientAttribute;
}


