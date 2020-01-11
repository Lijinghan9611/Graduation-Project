#include "LogIn.h"
#include "ui_login.h"
#include "ComboboxItem.h"
#include "MyMessageBox.h"
#include "Global.h"
#include <QListWidget>
#include <QCompleter>
#include <QDebug>
#include <QHostInfo>

LogIn::LogIn(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogIn)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_QuitOnClose,false);

    //设置标识
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint |Qt::FramelessWindowHint);
    setWindowTitle("登录");

    workLog = new WorkLog;

    signin = new SignIn(0,"zhuce");
    connect(signin,SIGNAL(newItem(QString,QString)),this,SLOT(onNewItem(QString,QString)));

    Global g;
    g.connectdb();

    m_ListWidget = new QListWidget(this);
    m_ListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//or items have space
    ui->comboBox->setEditable(true);
    ui->comboBox->setMaxVisibleItems(5);
    ui->comboBox->setModel(m_ListWidget->model());
    ui->comboBox->setView(m_ListWidget);
    ui->comboBox->setObjectName("combobox_class");
    ui->comboBox->lineEdit()->setStyleSheet("margin:1px; border:0px;");  //make combobox better
    ui->comboBox->lineEdit()->setPlaceholderText("请输入教师编号");
    idLineEdit = ui->comboBox->lineEdit();
    g.sql.exec("select id ,name from teacher");
    for (int i = 0; i <g.sql.size(); ++i)
    {
        ComboboxItem* item = new ComboboxItem(this);
        item ->button->hide();
        g.sql.next();
        item->label->setText(g.sql.value(0).toString());
        item->label->setToolTip(g.sql.value(0).toString());
        item->name->setText(g.sql.value(1).toString());
        QListWidgetItem* widgetItem = new QListWidgetItem(m_ListWidget);
        m_ListWidget->setItemWidget(widgetItem, item);
        connect(item,SIGNAL(itemClick(QString,QString)),this,SLOT(comboClick(QString,QString)));
    }

    g.sql.clear();
    g.sql.exec("select usePurpose from purpose order by id");
    while(g.sql.next())
    {
        ui->cbxPurpose->addItem(g.sql.value(0).toString());
    }

    connect(ui->comboBox->lineEdit(),SIGNAL(returnPressed()),this,SLOT(comboEnter()));
    connect(ui->comboBox->lineEdit(),SIGNAL(editingFinished()),this,SLOT(onEditingFinish()));

    ui->lineEdit_2->setEchoMode(QLineEdit::Password);
    ui->pushButton_3->setObjectName("setting");
    ui->pushButton->setObjectName("surebutton");
    ui->pushButton_2->setObjectName("settingLogin");

    g.disconnectdb();
    ui->labFindPwd->installEventFilter(this);
    connect(this,SIGNAL(loginSignal()),workLog,SLOT(onLogin()));
    connect(this,SIGNAL(signEnd(QString,QString,QString,QString)),workLog,SLOT(onSignEnd(QString,QString,QString,QString)));
    connect(this,SIGNAL(signEnd(QString,QString,QString,QString)),this,SLOT(onSignEndBeignWorkLog()));
    labFind = true;

    ui->leOtherPurpose->setVisible(false);
    ui->leOtherPurpose->setPlaceholderText("请输入使用目的");

    // 设置tab控件焦点切换顺序
    QWidget::setTabOrder(ui->comboBox, ui->leName);
    QWidget::setTabOrder(ui->leName, ui->lineEdit_2);
    QWidget::setTabOrder(ui->lineEdit_2, ui->cbxPurpose);
    QWidget::setTabOrder(ui->cbxPurpose, ui->pushButton);
    QWidget::setTabOrder(ui->pushButton, ui->comboBox);


    ui->pushButton_3->setFocusPolicy(Qt::NoFocus);
    ui->leOtherPurpose->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_2->setFocusPolicy(Qt::NoFocus);

    ui->comboBox->setFocus();

    ui->pushButton->setShortcut(Qt::Key_Enter);//将字母区回车键与登录按钮绑定在一起

}


/**
* @brief 获取教师姓名
* @date 2017/12/29
* @author han
* @return void
*/
void LogIn::onEditingFinish()
{
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    if(ui->comboBox->currentText() == set->value("admin/id","admin").toString())
    {
        ui->leName->setText("admin");
        ui->labmsgName->clear();
        ui->labmsg->clear();
        ui->lineEdit_2->setFocus();
        return;
    }

    Global g;
    g.connectdb();

    g.sql.exec(QString("select name from teacher where id = '%1'").arg(ui->comboBox->lineEdit()->text().trimmed()));

    if(g.sql.next())
    {
        ui->leName->setText(g.sql.value(0).toString());
        ui->labmsg->clear();
        ui->labmsgName->clear();
        ui->lineEdit_2->setFocus();
    }
    else
    {
        ui->labmsg->setText("不存在");
        ui->leName->clear();
    }

}

void LogIn::setFindLabEnable(bool f)
{
    ui->labFindPwd->setEnabled(f);
    labFind = f;
    if(f)
    {
        ui->labFindPwd->setText("找回密码");
    }
    else
        ui->labFindPwd->clear();
}

/**
* @brief 给label添加点击事件
* @date 2017/12/13
* @author han
* @return void
*/
bool LogIn::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->labFindPwd) {
        if (event->type() == QEvent::MouseButtonPress )
        {
            if(!labFind)
                return false;
            SignIn *signin = new SignIn(0,"findPwd");
            signin->setWindowTitle("找回密码");
            signin->exec();
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return LogIn::eventFilter(obj, event);
    }
}

/**
* @brief 下拉显示记录的账号
* @date 2017/12/9
* @author han
* @return void
*/
void LogIn::comboClick(const QString& str,const QString& name)
{
    ui->comboBox->setCurrentText(str);
    ui->leName->setText(name);
    ui->comboBox->hidePopup();
}


/**
* @brief 输入后
* @date 2017/12/9
* @author han
* @return void
*/
void LogIn::comboEnter()
{
    int index = ui->comboBox->findText(ui->comboBox->lineEdit()->text());
    ui->comboBox->removeItem(index);
}



/**
* @brief 确定
* @date 2017/12/13
* @author han
* @return void
*/
void LogIn::on_pushButton_clicked()
{

    MyMessageBox box;
    Global g;
    if(ui->comboBox->currentText().trimmed().isEmpty() || ui->lineEdit_2->text().trimmed().isEmpty())
    {
        box.iniMsgBox(MyMessageBox::alert,"账号密码不能为空!");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    //加密处理
    QString defaultPwd = g.EncriptPasswordByMD5("123");
    QString adminPassword = set->value("admin/passwd",defaultPwd).toString();
    //密码加密
    QString strPwdMd5 = g.EncriptPasswordByMD5(ui->lineEdit_2->text().trimmed());
    if(ui->comboBox->currentText() == set->value("admin/id","admin").toString() && strPwdMd5 == adminPassword)
    {
        if(!set->contains("admin/id") || !set->contains("admin/passwd")) //不存在节点
        {
            set->setValue("admin/id","admin");
            set->setValue("admin/passwd",defaultPwd);
        }
        //管理员
        Global::writeLog("work",QString("管理员登录成功"));
        Global::isAdmin = true;
        adminPwd = ui->lineEdit_2->text().trimmed();
        accept();
        delete set;
        return;
    }

    g.connectdb();



    g.sql.exec(QString("select id,name from teacher where id = %1 and password = '%2'")
               .arg(ui->comboBox->currentText(),strPwdMd5));
    if(g.sql.next())
    {  //教师
        g.teacherNum = g.sql.value(0).toLongLong();  //全局保存教师登录id
        teacherName = g.sql.value(1).toString();

        Global::writeLog("work",QString("教师%1登录成功").arg(g.teacherNum));
        //设置默认的考勤时间,每个教师单独设置
        QString value = QString("AttendanceTime/%1").arg(g.teacherNum);
        if(!set->contains(value))  //判断是否存在
            set->setValue(value,"15");
        delete set;

        //修改教师最后一次登录时间
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss");
        g.sql.clear();
        g.sql.exec(QString("update teacher set lastLoginTime='%1' where id='%2';").arg(time,g.getTeacherID()));

        this->hide();
        workLog->purpose = ui->cbxPurpose->currentText().trimmed();
        if(ui->cbxPurpose->currentText().trimmed() == "课程实验"
                ||ui->cbxPurpose->currentText().trimmed() == "竞赛培训")
        {
            g.LoginType = Global::TYPE1;
            emit loginSignal();  //处理工作日志界面，初始化

            workLog->setReturnBtnEnable(false);  //隐藏返回按键
            workLog->initWorkLogForType3Login(true);
            workLog->exec(); //打开工作日志界面
        }
        else if(ui->cbxPurpose->currentText().trimmed() == "理论授课")
        {
            g.LoginType = Global::TYPE2;
            emit loginSignal();  //处理工作日志界面，初始化
            workLog->setReturnBtnEnable(false);  //隐藏返回按键
            workLog->initWorkLogForType3Login(true);
            workLog->exec(); //打开工作日志界面
        }
        else
        {
            g.LoginType = Global::TYPE3;
            emit loginSignal();  //处理工作日志界面，初始化
            workLog->setReturnBtnEnable(false);  //隐藏返回按键
            workLog->initWorkLogForType3Login(false);
            workLog->exec(); //打开工作日志界面
        }

        accept();
    }
    else
    {
        delete set;
        box.iniMsgBox(MyMessageBox::alert,"账号或密码不正确!");
        box.isShowDialog(true);
        box.exec();
        return;
    }
}


void LogIn::onNewItem(const QString& id , const QString& name)
{
    ComboboxItem* item = new ComboboxItem(this);
    item ->button->hide();
    item->label->setText(id);
    item->label->setToolTip(id);
    item->name->setText(name);
    QListWidgetItem* widgetItem = new QListWidgetItem(m_ListWidget);
    m_ListWidget->setItemWidget(widgetItem, item);
    connect(item,SIGNAL(itemClick(QString,QString)),this,SLOT(comboClick(QString,QString)));
}

/**
* @brief 打开配置窗口
* @date 2017/12/10
* @author han
* @return void
*/
void LogIn::on_pushButton_3_clicked()
{
    //this->hide();
    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::success,"配置成功,请重启启动!");
    Global::writeLog("work",QString("修改配置"));
    UiConfigDialog dia;
    if(dia.exec() == QDialog::Accepted && dia.dlg.result()==QDialog::Accepted)
    {
        if(box.exec() == QDialog::Accepted)
            exit(0);
    }
}


/**
* @brief 点名结束(弃用！教师登录立刻要求登记)
* @date 2017/12/12
* @author han
* @return void
*/
void LogIn::onSignEndBeignWorkLog()
{
    //    if(workLog->isHidden() &&set->value("admin/log",false).toBool() && set->value("admin/logtime",0).toInt() == 1 )
    //    {
    //        emit loginSignal();
    //        workLog->show();
    //    }
    //    delete set;
}


/**
* @brief 处理班级管理中的修改日志信号
* @date 2017/12/12
* @author han
* @return void
*/
void LogIn::onEditWorkLog()
{
    workLog->isAlter = true;
    workLog->setReturnBtnEnable(true);  //显示返回按键
    workLog->onLogin();
    workLog->show();
}

LogIn::~LogIn()
{
    delete signin;
    delete workLog;
    delete ui;
}


void LogIn::closeEvent(QCloseEvent *)
{

}

void LogIn::on_cbxPurpose_currentTextChanged(const QString &arg1)
{
    if(arg1.trimmed() == "其他")
    {
        ui->leOtherPurpose->setVisible(true);
    }
    else
    {
        ui->leOtherPurpose->setVisible(false);
    }
}


void LogIn::on_leName_editingFinished()
{

    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    if(ui->comboBox->currentText() == set->value("admin/id","admin").toString())
    {
        ui->leName->setText("admin");
        ui->labmsgName->clear();
        ui->labmsg->clear();
        ui->lineEdit_2->setFocus();
        return;
    }

    Global g;
    g.connectdb();

    g.sql.exec(QString("select id from teacher where name = '%1'").arg(ui->leName->text().trimmed()));

    if(g.sql.next())
    {
        ui->comboBox->lineEdit()->setText(g.sql.value(0).toString());
        ui->labmsgName->clear();
        ui->labmsg->clear();
        ui->lineEdit_2->setFocus();
    }
    else
    {
        ui->labmsgName->setText("不存在");
        ui->comboBox->lineEdit()->clear();
    }
}


/**
* @brief 重置界面
* @date 2018/1/8
* @author han
* @return void
*/
void LogIn::resetAll()
{
    ui->comboBox->lineEdit()->clear();
    ui->leName->clear();
    ui->lineEdit_2->clear();
    ui->comboBox->lineEdit()->setFocus();
}


/**
* @brief 设置/显示软件使用的局域网ip
* @date 2018/1/12
* @author han
* @param
* @return void
*/
void LogIn::on_pushButton_2_clicked()
{
    QStringList LocalIpList = getIpList();
    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::info_no_beep,"请选择本教室正确的局域网IP。<br>当前ip：" + Global::LocalIP);
    box.setTitle("设置局域网IP");
    box.setCbxDialig(true,LocalIpList,Global::LocalIP);
    box.exec();

    if(box.myDialogResult)
    {
        Global::LocalIP = box.Msg;
        QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
        set->setValue("config/ServerIP",box.Msg);  //保存选择的IP
        Global::shutdownAutoGetIp = true;  //关闭自动获取IP
    }
}

/**
* @brief 获取本机局域网IP列表
* @date 2017/12/
* @author han
* @return 本机ip
*/
QStringList LogIn::getIpList()
{
    QStringList LocalIpList;

    QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
    LocalIpList.clear();
    foreach(QHostAddress address, AddressList)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol &&
                address != QHostAddress::Null){
            LocalIpList.append(address.toString());
        }
    }

    if (LocalIpList.size() == 0)  //都没有就返回本机地址127.0.0.1
        LocalIpList.append("127.0.0.1");
    return LocalIpList;
}
