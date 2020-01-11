#include "Tool_Bar.h"
#include<QStyleOption>
#include<QPainter>
#include <QHBoxLayout>
#include <QDebug>
#include <QMenu>
#include <QApplication>

#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#include <qt_windows.h>
#endif

Tool_Bar::Tool_Bar(QWidget *parent )  : QWidget(parent)
{
    setFixedHeight(100);
    ispress = false;
    maxIsSet = false;
    normalIsSet = false;

    m_pIconLabel = new QLabel(this);
    m_pTitleLabel = new QLabel(this);
    m_pMinimizeButton = new QPushButton(this);
    m_pMaximizeButton = new QPushButton(this);
    m_pCloseButton = new QPushButton(this);
    m_pSetting = new QPushButton(this);

    m_pIconLabel->setFixedSize(20, 20);
    m_pIconLabel->setScaledContents(true);
    m_pMinimizeButton->setFixedSize(30, 28);
    m_pMaximizeButton->setFixedSize(30, 28);
    m_pCloseButton->setFixedSize(30, 28);
    m_pSetting->setFixedSize(30,28);
    m_pTitleLabel->setObjectName("whiteLabel");
    m_pTitleLabel->setFixedHeight(28);
    m_pTitleLabel->setText("电子教室教学管理软件");
    m_pIconLabel->setFixedSize(32,28);
    m_pIconLabel->setAlignment(Qt::AlignCenter);
    m_pIconLabel->setScaledContents(false);
    QImage omg(":/images/images/else/icon2.png");
    m_pIconLabel->setPixmap(QPixmap::fromImage(omg));
    m_pMinimizeButton->setObjectName("minimizeButton");
    m_pMaximizeButton->setObjectName("maximizeButton");
    m_pCloseButton->setObjectName("closeButton");
    m_pSetting->setObjectName("settingTitle");
    m_pMinimizeButton->setToolTip("最小化");
    m_pCloseButton->setToolTip("关闭");
    m_pSetting->setToolTip("更多");
    QMenu * menu = new QMenu;
    m_pSetting->setMenu(menu);
    set = new QAction("设置",menu);
    skin = new QAction("换肤",menu);
    logout = new QAction("注销",menu);
    QIcon icon5(":/images/images/button/logout.ico");
    logout->setIcon(icon5);
    endProtect = new QAction ("关闭进程保护",menu);
    menu->addAction(set);
    menu->addAction(endProtect);
    menu->addAction(skin);
    QMenu * menuSkin = new QMenu;
    redBlack = new QAction("红黑",menuSkin);
    blueWhite = new QAction("蓝白",menuSkin);
    menuSkin->addAction(redBlack);
    menuSkin->addAction(blueWhite);
    skin->setMenu(menuSkin);
    menu->addAction(logout);
    login = new LogIn;

    adminOption = new AdminOption;
    connect(m_pMinimizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pMaximizeButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pCloseButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(logout,SIGNAL(triggered()),this,SLOT(onMenuClick()));
    connect(set,SIGNAL(triggered()),this,SLOT(onMenuClick()));
    connect(endProtect,SIGNAL(triggered()),this,SLOT(onMenuClick()));
    connect(redBlack,SIGNAL(triggered()),this,SLOT(onMenuClick()));
    connect(blueWhite,SIGNAL(triggered()),this,SLOT(onMenuClick()));

    pClassButton = new MyButton(this);
    pCmdButton = new MyButton(this);
    //pCloseAllComputer = new MyButton(this);
    pScreenBrocastButton = new MyButton(this);
    //pRestartAllComputer = new MyButton(this);
    pClassManage = new CLassManage();
    pCmdWidget = new CmdWidget();
    pTransportFile = new MyButton(this);
    recieveFileButton = new MyButton(this);
    editTeacherInfoBtn = new MyButton(this);
    //文件发送模块
    pTranportFileWidget = new TranportFileWidget(1);
    recieveFileWidget = new TranportFileWidget(2);

    //修改教师信息窗口
    editTeacherDialog = new SignIn(this,"editTeacher");

    pLogout = new MyButton(this);
    QImage img;
    img.load(":/images/images/button/bro.ico");
    pScreenBrocastButton->imgLabel->setPixmap(QPixmap::fromImage(img));
    pScreenBrocastButton->wordLabel->setText("屏幕广播");
    img.load(":/images/images/button/class.ico");
    pClassButton->imgLabel->setPixmap(QPixmap::fromImage(img));
    pClassButton->wordLabel->setText("课堂管理");
    img.load(":/images/images/button/cmd.ico");
    pCmdButton->imgLabel->setPixmap(QPixmap::fromImage(img));
    pCmdButton->wordLabel->setText("远程命令");
    img.load(":/images/images/button/file.ico");
    pTransportFile->imgLabel->setPixmap(QPixmap::fromImage(img));
    pTransportFile->wordLabel->setText("传送文件");
    img.load(":/images/images/button/filein.png");
    recieveFileButton->imgLabel->setPixmap(QPixmap::fromImage(img));
    recieveFileButton->wordLabel->setText("接收文件");
    img.load(":/images/images/button/logout.ico");
    pLogout->imgLabel->setPixmap(QPixmap::fromImage(img));
    pLogout->wordLabel->setText("退出登录");
    img.load(":/images/images/button/teacherEdit.png");
    editTeacherInfoBtn->imgLabel->setPixmap(QPixmap::fromImage(img));
    editTeacherInfoBtn->wordLabel->setText("修改教师信息");

    QHBoxLayout *pLayout = new QHBoxLayout(this);
    pLayout->addWidget(m_pIconLabel,0,Qt::AlignTop);
    pLayout->addSpacing(5);
    pLayout->addWidget(m_pTitleLabel,0,Qt::AlignTop);
    pLayout->addSpacing(30);

    pLayout->addWidget(pScreenBrocastButton);
    pLayout->addWidget(pClassButton);
    pLayout->addWidget(pTransportFile);
    pLayout->addWidget(recieveFileButton);
    pLayout->addWidget(pCmdButton);
    pLayout->addWidget(editTeacherInfoBtn);
    pLayout->addWidget(pLogout);

    pLayout->addStretch(0);
    pLayout->addWidget(m_pSetting,0,Qt::AlignTop);
    pLayout->addWidget(m_pMinimizeButton,0,Qt::AlignTop);
    pLayout->addWidget(m_pMaximizeButton,0,Qt::AlignTop);
    pLayout->addWidget(m_pCloseButton,0,Qt::AlignTop);

    pLayout->setContentsMargins(0,0,0,0);
    pLayout->setSpacing(0);

    //收到文件
    connect(this,SIGNAL(receiveFile(QString,QString)),recieveFileWidget,SLOT(onReceiveFile(QString,QString)));
    //接收文件
    connect(recieveFileWidget,SIGNAL(startRecieveFileSignal()),this,SIGNAL(startRecieveFileSignal()));
    connect(recieveFileWidget,SIGNAL(stopRecieveFileSignal()),this,SIGNAL(stopRecieveFileSignal()));

    //Button
    connect(recieveFileButton,SIGNAL(clicked()),this,SLOT(onButtonClick()));
    connect(pClassButton,SIGNAL(clicked()),this,SLOT(onButtonClick()));
    connect(pCmdButton,SIGNAL(clicked()),this,SLOT(onButtonClick()));
    connect(pScreenBrocastButton,SIGNAL(clicked()),this,SLOT(onButtonClick()));
    connect(editTeacherInfoBtn,SIGNAL(clicked()),this,SLOT(onButtonClick()));
    connect(pTransportFile,SIGNAL(clicked()),this,SLOT(onButtonClick()));
    connect(pLogout,SIGNAL(clicked()),this,SLOT(onButtonClick()));
    //sign
    connect(pClassManage,SIGNAL(beginSign()),this,SIGNAL(beginSign()));
    connect(pClassManage,SIGNAL(retrieveSign()),this,SIGNAL(retrieveSign()));
    connect(this,SIGNAL(signMg(QString,QString,QString)),pClassManage,SLOT(onSignMg(QString,QString,QString)));
    connect(pClassManage,SIGNAL(signMgToolTip(QString,QString,QString)),this,SIGNAL(signMgToolTip(QString,QString,QString)));

    //Q&A
    connect(pClassManage,SIGNAL(question(QString)),this,SIGNAL(question(QString)));
    connect(pClassManage,SIGNAL(getAnswer()),this,SIGNAL(getAnswer()));
    connect(this,SIGNAL(answerMg(QString,QString,QString,QString)),pClassManage,SLOT(onAnswerMg(QString,QString,QString,QString)));

    //cmd
    connect(pCmdWidget,SIGNAL(cmd(QString,bool,QString)),this,SIGNAL(cmd(QString,bool,QString)));
    connect(this,SIGNAL(cmdResult(int)),pCmdWidget,SLOT(onCmdResult(int)));
    connect(this,SIGNAL(cmdCallBack(QString)),pCmdWidget,SLOT(onCmdCallBack(QString)));

    connect(this,SIGNAL(logoutSignal()),pClassManage,SLOT(onLogOut()));

    //worklog ini
    connect(pClassManage,SIGNAL(signEnd(QString,QString,QString,QString)),login,SIGNAL(signEnd(QString,QString,QString,QString)));
    connect(pClassManage,SIGNAL(editWorklog()),login,SLOT(onEditWorkLog()));


    //transport file
    connect(pTranportFileWidget,SIGNAL(fileName(QString)),this,SIGNAL(fileName(QString)));
    connect(this,SIGNAL(transportFileCallBack()),pTranportFileWidget,SLOT(onTransportFileCallBack()));

    //portrait
    connect(pClassManage,SIGNAL(importPortrait()),this,SIGNAL(importPortrait()));
    connect(this,SIGNAL(iniPortrait(QString,QImage)),pClassManage,SLOT(onIniPortrait(QString,QImage)));
    connect(pClassManage,SIGNAL(beginComparePortrait(int)),this,SIGNAL(beginComparePortrait(int)));
    connect(this,SIGNAL(comparePortrait(QString,QImage)),pClassManage,SLOT(onComparePortrait(QString,QImage)));
    connect(adminOption,SIGNAL(logoutFromAdmin()),this,SLOT(LogOut()));

}

void Tool_Bar::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    //Wrong to do this ! Death lock! watse lots of CPU
    QWidget * pw = this->window();
    if(pw->isTopLevel() && pw->isMaximized())
    {
        if(maxIsSet)
        {
            maxIsSet = false;
            return;
        }
        m_pMaximizeButton->setStyleSheet("QPushButton#maximizeButton{background-image:url(:/images/images/TiTleBar/btn_restore_normal.png);background-color:transparent;background-repeat:no-repeat;background-position:center;}"
                                         "QPushButton#maximizeButton:hover {background:url(:/images/images/TiTleBar/btn_restore_highlight.png),transparent;background-repeat:no-repeat;background-position:center;}"
                                         "QPushButton#maximizeButton:pressed{background:url(:/images/images/TiTleBar/btn_restore_down.png),transparent;background-repeat:no-repeat;background-position:center;}");
        m_pMaximizeButton->setToolTip("恢复");
        maxIsSet = true;
    }
    else
    {
        if(normalIsSet)
        {
            normalIsSet = false;
            return;
        }
        m_pMaximizeButton->setStyleSheet("QPushButton#maximizeButton{background-image:url(:/images/images/TiTleBar/btn_max_normal.png);background-color:transparent;background-repeat:no-repeat;background-position:center;}"
                                         "QPushButton#maximizeButton:hover {background:url(:/images/images/TiTleBar/btn_max_highlight.png),transparent;background-repeat:no-repeat;background-position:center;}"
                                         "QPushButton#maximizeButton:pressed{background:url(:/images/images/TiTleBar/btn_max_down.png),transparent;background-repeat:no-repeat;background-position:center;}");
        m_pMaximizeButton->setToolTip("最大化");
        normalIsSet = true;
    }

    QWidget::paintEvent(ev);
}

void Tool_Bar::mousePressEvent(QMouseEvent *event)
{
#ifdef Q_OS_WIN
    if (ReleaseCapture())
    {
        QWidget *pWindow = this->window();
        if (pWindow->isTopLevel())
        {
            SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
        }
    }
    event->ignore();
#else
    if(event->button() == Qt::LeftButton)
    {
        QWidget * p = this->window();
        if(p->isTopLevel())
        {
            m_DragPosition = event->globalPos() - p->pos();
            ispress = true;
            event->accept();
        }
    }
#endif
}

void Tool_Bar::mouseMoveEvent(QMouseEvent *event)
{
#ifdef Q_OS_WIN
    Q_UNUSED(event);
#else
    if (ispress && event->buttons() && Qt::LeftButton)
    {
        QWidget * p = this->window();
        if(p->isTopLevel())
        {
            if(p->isMaximized())
                p->showNormal();
            p->move(event->globalPos() - m_DragPosition);
            event->accept();
        }
    }
#endif
}

void Tool_Bar::mouseReleaseEvent(QMouseEvent *)
{
    ispress =false;
    update();
}

void Tool_Bar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
        emit m_pMaximizeButton->clicked();
}

void Tool_Bar::onButtonClick()
{
    Global g;
    QPushButton *pButton = qobject_cast<QPushButton *>(sender());
    if(pButton == pClassButton)
    {
        pClassManage->showMaximized();
        pClassManage->initLesson();  //更新显示课程
        pClassManage->activateWindow();
    }
    else if(pButton == pCmdButton)
    {
        pCmdWidget->onlyone = false;
        pCmdWidget->bar->m_pTitleLabel->setText("执行远程命令 (目标:全部上线主机)");
        if(pCmdWidget->isHidden())
        {
            pCmdWidget->pTxtEdit->clear();
            pCmdWidget->pLineEdit->setEnabled(true);
            pCmdWidget->show();
        }
        else
            pCmdWidget->activateWindow();

    }
    else if(pButton == pScreenBrocastButton)
    {
        if(!Global::isBroadcast)
        {
            Global::writeLog("work",QString("教师%1开始广播").arg(g.getTeacherID()));
            QImage img;
            img.load(":/images/images/button/end.ico");
            pScreenBrocastButton->imgLabel->setPixmap(QPixmap::fromImage(img));
            pScreenBrocastButton->wordLabel->setText("结束广播");
            emit brocastBegin();
            Global::isBroadcast = true;

        }
        else
        {
            Global::writeLog("work",QString("教师%1结束").arg(g.getTeacherID()));
            QImage img;
            img.load(":/images/images/button/bro.ico");
            pScreenBrocastButton->imgLabel->setPixmap(QPixmap::fromImage(img));
            pScreenBrocastButton->wordLabel->setText("屏幕广播");
            emit brocastEnd();
            Global::isBroadcast = false;
        }
    }
    else if(pButton == pTransportFile)  //传输文件
    {
        pTranportFileWidget->show();
        pTranportFileWidget->activateWindow();
    }
    else if(pButton == pLogout)
    {
        LogOut();
    }
    else if(pButton == recieveFileButton)
    {
        recieveFileWidget->show();
        recieveFileWidget->activateWindow();
    }
    else if(pButton == editTeacherInfoBtn)  //修改教师信息
    {
        editTeacherDialog->initEditTeacherDialog();
        editTeacherDialog->show();
        editTeacherDialog->activateWindow();
    }
}


/**
* @brief 接收来自labelview的启动cmd信号
* @date 2018/1/22
* @author han
* @param
* @return void
*/
void Tool_Bar::onSingleCmd(const QString& ip,const QString& userName,const QString& userID)
{
    pCmdWidget->initForOneClient(ip,userName,userID);
    if(pCmdWidget->isHidden())
    {
        pCmdWidget->pTxtEdit->clear();
        pCmdWidget->pLineEdit->setEnabled(true);
        pCmdWidget->show();
    }
    else
        pCmdWidget->activateWindow();
}


/**
* @brief 最小最大化
* @date 2017/12/18
* @author han
* @return void
*/
void Tool_Bar::onClicked()
{
    QPushButton *pButton = qobject_cast<QPushButton *>(sender());
    QWidget *pWindow = this->window();
    if (pWindow->isTopLevel())
    {
        if (pButton == m_pMinimizeButton)
        {
            pWindow->showMinimized();

        }
        else if (pButton == m_pMaximizeButton)
        {
            if(pWindow->isMaximized())

                pWindow->showNormal();
            else
                pWindow->showMaximized();
        }
        else if (pButton == m_pCloseButton)
        {
            pWindow->close();
        }
    }
}


/**
* @brief 主界面下拉菜单点击
* @date 2017/12/17
* @author han
* @return void
*/
void Tool_Bar::onMenuClick()
{
    QAction * action = qobject_cast<QAction*>(sender());
    QWidget *pWindow = this->window();
    if(action == logout)  //注销登录
    {
        LogOut();
    }
    else if(action == set)
    {
        if(pWindow->isTopLevel())
            pWindow->hide();
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::success,"配置成功,请重启启动!");

        UiConfigDialog dia;
        if(dia.exec() == QDialog::Accepted && dia.dlg.result()==QDialog::Accepted)
        {
            box.exec();
            exit(0);
        }
        else
            pWindow->show();
    }
    else if(action == endProtect)
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::information,"确认要取消客户端进程保护 ?");
        if(box.exec() == QDialog::Accepted)
        {
            emit endProtect1();  //to toolbar
            box.iniMsgBox(MyMessageBox::success,"已取消进程保护!");
            box.isShowDialog(true);
            box.exec();
        }
    }
    else if(action == blueWhite)
    {

        QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

        set->setValue("style/style","BlueWhite.qss");
        delete set;
        QFile qss(":/qssfile/files/BlueWhite.qss");
        qss.open(QFile::ReadOnly);
        qApp->setStyleSheet(qss.readAll());
        qss.close();
        Global::skinFileName = "blueWhite.qss";
        emit skinChange();
    }
    else if(action == redBlack)
    {
        QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

        set->setValue("style/style","RedBlack.qss");
        delete set;  //files/redBlack.qss
        QFile qss(":/qssfile/files/RedBlack.qss");  //
        qss.open(QFile::ReadOnly);
        qApp->setStyleSheet(qss.readAll());
        qss.close();
        Global::skinFileName = "redBlack.qss";
        emit skinChange();
    }

}


/**
* @brief 退出登录
* @date 2018/1/4
* @author han
* @return void
*/
void Tool_Bar::LogOut()
{
    QWidget *pWindow = this->window();
    Global::teacherNum = 0;
    Global::isAdmin = false;
    Global::LoginType = 0;
    Global::workLogId = -1;
    Global::AttendanceTime = 0;

    pClassManage->setSignTimerStop();//停止自动点名
    pClassManage->hide();
    pCmdWidget->hide();
    pTranportFileWidget->hide();
    emit logoutSignal();

    if(pWindow->isTopLevel())
    {
        if(Global::isBroadcast)  //停止广播
        {
            emit brocastEnd();
            Global::isBroadcast = false;
        }
        pWindow->hide();

        emit hideTray();
        login->resetAll();
        if(login->exec())
        {
            if(Global::isAdmin)
            {
                adminOption->setAdminPwd(login->adminPwd);
                login->adminPwd = "";
                adminOption->showMaximized();
            }
            else
            {
                emit setViewToScreen();
                emit changeLoginTName(login->teacherName);
                pClassManage->initLesson();  //更新显示课程
                pWindow->showMaximized();
                pWindow->activateWindow();
                emit showTray();
                pClassManage->setSignTimerStart();//开启自动点名
            }
        }
    }
}

Tool_Bar::~Tool_Bar()
{
    pClassManage->deleteLater();
    pCmdWidget->deleteLater();
    pTranportFileWidget->deleteLater();
    delete login;
    delete adminOption;
}
