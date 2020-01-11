#include "CmdWidget.h"

#include <QTextCursor>
#include <QDebug>
#include <QTextBlock>
#include <QStyleOption>
#include <QPainter>
#include <QHBoxLayout>
#include <QSettings>

CmdWidget::CmdWidget(QWidget *parent) : QWidget(parent)
{
    onlyone = true;
    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags( Qt::FramelessWindowHint| windowFlags());
    setWindowTitle("执行CMD命令");
    setMinimumSize(800,500);

    bar = new CoolTitleBar(this);
    bar->m_pTitleLabel->setText("执行远程命令");
    pTxtEdit = new QTextEdit(this);
    pLineEdit = new CmdLineEdit(this);

    pLineEdit->setFixedHeight(30);
    pLineEdit->setStyleSheet("font-size:15px;font-family:幼圆;");
    pLineEdit->setPlaceholderText("命令");
    pLineEdit->setToolTip("多个命令请用&连接,输入clear清屏");
    pLineEdit->setFocus();
    pTxtEdit->setReadOnly(true);
    pTxtEdit->setStyleSheet("QTextEdit{font-size:16px;font-family:幼圆;border:1px solid grey;}");
    pTxtEdit->append("欢迎使用本系统,请输入命令,按Enter键开始执行");

    clearBtn = new QPushButton("清屏");
    execBtn =new QPushButton("执行");
    orderCbx =new QComboBox;
    execBtn->setObjectName("surebutton");
    clearBtn->setObjectName("commonbutton");

    clearBtn->setMinimumWidth(100);
    orderCbx->setMinimumWidth(100);
    execBtn->setMinimumWidth(100);

    orderCbx->addItem("");   //默认没有选中
    orderList.append("");  //添加一个空的
    orderCbx->setObjectName("combobox_class2");

    //读取配置文件
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    QString value = QString("CmdOrder");
    if(!set->contains(value + "/关机"))  //判断是否存在
    {
        //添加常用的关机、重启命令
        orderCbx->addItem("关机");
        orderCbx->addItem("重启");
#ifdef Q_OS_WIN
        orderList.append("shutdown -s -f -t 0");
        orderList.append("shutdown -r -f -t 0");
        set->setValue(value + "/关机","shutdown -s -f -t 0");
        set->setValue(value + "/重启","shutdown -r -f -t 0");
#else
        orderList.append("poweroff");
        orderList.append("reboot");
        set->setValue(value + "/关机","poweroff");
        set->setValue(value + "/重启","reboot");
#endif

    }
    else//读取设置的常用命令
    {
        set->beginGroup("CmdOrder");
        QStringList temp = set->childKeys();
        for(int i = 0;i < temp.size();i++)
        {
            QString key = QString(temp.at(i));
            QString ord = set->value(key).toString();

            if(ord == "")
                continue;
            orderCbx->addItem(temp.at(i));
            orderList.append(ord);
        }

    }

    delete set;


    QLabel *label = new QLabel("常用命令：");

    QHBoxLayout * menuLayout = new QHBoxLayout;
    menuLayout->addWidget(pLineEdit);
    menuLayout->addWidget(label);
    menuLayout->addWidget(orderCbx);
    menuLayout->addWidget(clearBtn);
    menuLayout->addWidget(execBtn);

    QWidget *w = new QWidget;
    w->setLayout(menuLayout);

    QVBoxLayout * pLayout  =  new QVBoxLayout(this);
    pLayout->addWidget(bar);
    pLayout->addWidget(w);
    pLayout->addWidget(pTxtEdit);
    pLayout->setContentsMargins(0,0,0,0);
    setLayout(pLayout);

    connect(pLineEdit,SIGNAL(returnPressed()),this,SLOT(onReturnPressed()));
    connect(clearBtn,SIGNAL(clicked(bool)),this,SLOT(on_clearBtnClick()));
    connect(execBtn,SIGNAL(clicked(bool)),this,SLOT(on_execBtnClick()));
}



/**
* @brief 初始化界面
* @date 2018/1/22
* @author han
* @param
* @return void
*/
void CmdWidget::initForOneClient(const QString& IP,const QString& userName,const QString& userID)
{
    this->pTxtEdit->clear();
    onlyone = true;
    ip = IP;
    QString title = QString("执行远程命令 (目标:IP：%1 用户名：%2 ID：%3)").arg(IP,userName,userID);
    bar->m_pTitleLabel->setText(title);
}

/**
* @brief 清屏
* @date 2018/1/8
* @author han
* @return void
*/
void CmdWidget::on_clearBtnClick()
{
    pTxtEdit->clear();
    pLineEdit->clear();
}


/**
* @brief 执行按键点击
* @date 2018/1/8
* @author han
* @return void
*/
void CmdWidget::on_execBtnClick()
{
    execOrder();
}


/**
* @brief 执行命令
* @date 2018/1/8
* @author han
* @return void
*/
void CmdWidget::execOrder()
{
    if(pLineEdit->text()=="" && orderCbx->currentText().trimmed() == "")  //无输入
    {
        pTxtEdit->append("请输入或选择远程命令");
        return;
    }
    if(pLineEdit->text()=="clear")  //clear清屏
    {
        pTxtEdit->clear();
        pLineEdit->clear();
        return;
    }
    if(Global::hostNum==0)   //没有客户端
    {
        pTxtEdit->append("无主机上线");
        return;
    }

    successCount = 0;
    index = 0;
    QString order;
    if(orderCbx->currentText().trimmed() == "")
        order = pLineEdit->text();
    else
    {
        int index = orderCbx->currentIndex();
        order = orderList.at(index);
    }

    pTxtEdit->append("正在执行:<strong> \""+order+"\" </strong>请稍等...");

    if(!onlyone)  //执行cmd命令
        emit cmd("Any",false,order);
    else
        emit cmd(ip,true,order);

    pLineEdit->tip << order;
    pLineEdit->index  = pLineEdit->tip.size();
    pLineEdit->clear();
    pLineEdit->setEnabled(false);
    execBtn->setEnabled(false);
    pLineEdit->setFocus();
}

void CmdWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//执行cmd命令
void CmdWidget::onReturnPressed()
{
    execOrder();
}

//cmd命令执行结果
void CmdWidget::onCmdResult(int result)
{
    if(!onlyone)
    {
        if(result ==0)
            successCount ++;
        index ++;
        if(index == Global::hostNum)
        {
            pTxtEdit->append(QString("<strong style=font-family:arial;color:green;font-size:15px;>执行完毕</strong><p>主机数%1 ,成功执行%2</p>").arg(Global::hostNum).arg(successCount));
            pLineEdit->setEnabled(true);
            execBtn->setEnabled(true);
            pLineEdit->setFocus();
        }
    }
}

//cmd命令执行结果
void CmdWidget::onCmdCallBack(const QString& allStr)
{
    pTxtEdit->append("<strong style=font-family:arial;color:green;font-size:15px;>执行完毕</strong>");

    QStringList list;
    list = allStr.split("|+|",QString::KeepEmptyParts);
    foreach (QString str, list)
    {
        pTxtEdit->append(QString("<p style=font-family:arial;font-size:14px;>%1</p>").arg(str));
    }
    pLineEdit->setEnabled(true);
    execBtn->setEnabled(true);
    pLineEdit->setFocus();
}

void CmdWidget::closeEvent(QCloseEvent *)
{
    pLineEdit->clear();
    pLineEdit->tip.clear();
    pLineEdit->index = 0;
    pLineEdit->setFocus();
}


