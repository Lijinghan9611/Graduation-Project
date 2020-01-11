#include "LabelView.h"
#include "MyMessageBox.h"
#include "Global.h"
#include <QDebug>
#include <QMenu>
#include <QDateTime>

//监控屏幕上主机的右键菜单
LabelView::LabelView(QWidget*parent):QLabel(parent)
{
    setAlignment(Qt::AlignCenter);
    setScaledContents(true);
    userId = "0";
    userName = "0";
    on=false;

    rightMenu = new QMenu();
    cmd= new QAction("启动远程命令窗口",this);
    closePc = new QAction("关闭此机",this);
    RestartPc = new QAction("重启此机",this);
    sendMg = new QAction("发送消息",this);
    queryClientMg = new QAction("查询此机信息",this);
    setClientAction = new QAction("将此电脑设置为缺失",this);

    // 添加图标
    QIcon icon1(":/images/images/button/close.ico");
    QIcon icon2(":/images/images/button/cmd.ico");
    QIcon icon3(":/images/images/button/restart.ico");
    QIcon icon4(":/images/images/button/free.ico");
    QIcon icon5(":/images/images/button/information.ico");
    QIcon icon6(":/images/images/button/sub.png");

    cmd->setIcon(icon2);
    closePc->setIcon(icon1);
    RestartPc->setIcon(icon3);
    sendMg->setIcon(icon4);
    queryClientMg->setIcon(icon5);
    setClientAction->setIcon(icon6);

    rightMenu->addAction(sendMg);
    rightMenu->addAction(cmd);
    rightMenu->addAction(closePc);
    rightMenu->addAction(RestartPc);
    rightMenu->addAction(queryClientMg);
    rightMenu->addAction(setClientAction);
    connect(cmd,SIGNAL(triggered()),this,SLOT(onButtonClick()));
    connect(closePc,SIGNAL(triggered()),this,SLOT(onButtonClick()));
    connect(RestartPc,SIGNAL(triggered()),this,SLOT(onButtonClick()));
    connect(sendMg,SIGNAL(triggered()),this,SLOT(onButtonClick()));
    connect(queryClientMg,SIGNAL(triggered()),this,SLOT(onButtonClick()));
    connect(setClientAction,SIGNAL(triggered()),this,SLOT(onButtonClick()));
}


//点击右键，弹出菜单
void LabelView::contextMenuEvent(QContextMenuEvent *ev)
{
    if(!on)
    {
        cmd->setEnabled(false);
        closePc->setEnabled(false);
        RestartPc->setEnabled(false);
        sendMg->setEnabled(false);
        queryClientMg->setEnabled(false);
        setClientAction->setEnabled(true);
    }
    else
    {
        cmd->setEnabled(true);
        closePc->setEnabled(true);
        RestartPc->setEnabled(true);
        sendMg->setEnabled(true);
        queryClientMg->setEnabled(true);
        setClientAction->setEnabled(false);
    }
    rightMenu->exec(cursor().pos());
    ev->accept();
}

void LabelView::paintEvent(QPaintEvent *ev)
{
    QLabel::paintEvent(ev);
}

//双击，
void LabelView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton && on == true && Global::style == WM_Mornitor)
        emit mornitor(ip);
}

void LabelView::enterEvent(QEvent *)
{
    if(Global::style == WM_Common)
    {
        setFixedSize(95,95);
    }
    else
    {
        if(on)
        {
            if(Global::skinFileName == "redBlack.qss")
                setStyleSheet("LabelView{background-color:black;border:4px solid qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #13375C, stop:0.5 #12CABD stop:1 darkblue);}");
            else
                setStyleSheet("LabelView{background-color:#0E8EE7;border:4px solid qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #13375C, stop:0.5 #12CABD stop:1 darkblue);}");
        }
        else
        {
            if(Global::skinFileName == "redBlack.qss")
                setStyleSheet("LabelView{background-color:black;border:3px solid qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #FF0000, stop:0.5 #FFABAB  stop:1 #FF0000);}");
            else
                setStyleSheet("LabelView{background-color:lightblue;border:3px solid qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 #FF0000, stop:0.5 #FFABAB  stop:1 #FF0000);}");
        }
    }
}

void LabelView::leaveEvent(QEvent *)
{
    if(Global::style == WM_Common)
    {
        setStyleSheet("LabelView{border:none;background-color:transparent;color:white;}");
        setFixedSize(100,100);
    }
    else
    {
        if(Global::skinFileName == "redBlack.qss")
            setStyleSheet("LabelView{border:none;background-color:black;color:white;}");
        else
            setStyleSheet("LabelView{border:none;background-color:lightblue;color:white;}");
    }
}

void LabelView::onButtonClick()
{
    QAction * action = qobject_cast<QAction*>(sender());
    MyMessageBox box;
    if(action==cmd)
        emit beginCmd(ip,userName,userId);
    else if(action == closePc)
    {
        box.iniMsgBox(MyMessageBox::alert,"确定关闭此机?");
        if(box.exec() == QDialog::Accepted)
        {
            emit closeComputer(ip);
            box.iniMsgBox(MyMessageBox::success,"执行成功!");
            box.exec();
        }
    }
    else if(action == RestartPc)
    {
        box.iniMsgBox(MyMessageBox::alert,"确定重启此机?");
        if(box.exec() == QDialog::Accepted)
        {
            emit restartComputer(ip);
            box.iniMsgBox(MyMessageBox::success,"执行成功!");
            box.exec();
        }
    }
    else if(action == sendMg)
        emit msg(ip);
    else if(action == queryClientMg)
        emit cheakClient(ip,mac,name);
    else if(action == setClientAction)
    {
        SingleTonSToast::getInstance().setMessageVDuration("设置成功", 5000);
        QSettings *set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

        QString oldLack = set->value("ui/lack","").toString();
        oldLack += " " + newPosition;
        set->setValue("ui/lack",oldLack);
        emit hideLabel(newPosition,true);

        delete set;
        this->hide();
    }
}



/**
* @brief    客户端登录
* @date 2018/1/24
* @author han
* @param
* @return void
*/
void LabelView::onClientCome(QString str)
{
    if(str == ip)
    {
        if(this->isHidden())
        {
            this->show();
            emit hideLabel(newPosition,false);
        }
        QDateTime currentTime = QDateTime::currentDateTime();
        onlineTime = currentTime.toString("yyyy-MM-dd hh:mm:ss");
    }
}


/**
* @brief 客户端离线,插入电脑使用记录
* @date 2018/1/10
* @author han
* @return void
*/
void LabelView::onClientLeave(QString str)
{
    if(str == ip)
    {
        QDateTime currentTime = QDateTime::currentDateTime();
        QString offlineTime = currentTime.toString("yyyy-MM-dd hh:mm:ss");
        Global g;
        g.connectdb();
        g.sql.exec(QString("insert into usestatus(position,online,userID,userName,offline) values('%1','%2',%3,'%4','%5')")
                   .arg(newPosition,onlineTime,userId,userName,offlineTime));
    }
}

/**
* @brief 更新id和姓名
* @date 2017/12/11
* @author han
* @return void
*/
void LabelView::onSignMg(const QString& ip1, const QString& id, const QString& name)
{
    if(ip1 == ip)
    {
        userId = id;
        userName = name;
    }
}

LabelView::~LabelView()
{
    delete rightMenu;
}













