#include "MyMessageBox.h"
#include "ui_MyMessageBox.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "Global.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>

MyMessageBox::MyMessageBox(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MyMessageBox)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint | windowFlags()|Qt::WindowStaysOnTopHint );
    setAttribute(Qt::WA_QuitOnClose);

    bar = new CommonTitleBar(this);
    QWidget  * midwid = new QWidget(this);
    QWidget * endwid = new QWidget(this);
    pic  = new QLabel(midwid);
    text = new QLabel(midwid);
    button = new QPushButton(endwid);
    button2 = new QPushButton(endwid);

    bar->m_pTitleLabel->setText("提示");
    pic->setFixedSize(64,64);
    text->setAlignment(Qt::AlignVCenter);
    text->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    text->setWordWrap(true);
    endwid->setFixedHeight(30);
    endwid->setObjectName("endwid");
    button->setFixedSize(80,25);
    button->setText("确定");
    button2->setFixedSize(80,25);
    button2->setText("取消");
    leMsg = new QLineEdit;
    cbxValue = new QComboBox;
    isCbxDialog = false;
    cbxValue->setObjectName("combobox_class2");

    button->setShortcut(Qt::Key_Enter);//将字母区回车键与登录按钮绑定在一起


    QVBoxLayout * mLayout =  new QVBoxLayout(this);
    mLayout->addWidget(text);
    mLayout->addWidget(leMsg);
    mLayout->addWidget(cbxValue);
    QWidget * wi = new QWidget;
    wi->setLayout(mLayout);

    QHBoxLayout * hLayout =  new QHBoxLayout(this);
    hLayout->addWidget(pic);
    hLayout->addSpacing(20);
    hLayout->addWidget(wi);
    midwid->setLayout(hLayout);

    leMsg->hide();  //默认输入框隐藏
    cbxValue->hide();

    QHBoxLayout * hLayout2 =  new QHBoxLayout(this);
    hLayout2->addStretch(0);
    hLayout2->setSpacing(0);
    hLayout2->addWidget(button2);
    hLayout2->addSpacing(5);
    hLayout2->addWidget(button);
    hLayout2->setContentsMargins(5,1,5,1);
    endwid->setLayout(hLayout2);


    QVBoxLayout *pLayout = new QVBoxLayout();
    pLayout->addWidget(bar);
    pLayout->addSpacing(10);
    pLayout->addWidget(midwid);
    pLayout->addStretch(0);
    pLayout->addWidget(endwid);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pLayout);

    connect(button,SIGNAL(clicked(bool)),this,SLOT(onButtonClick()));
    connect(button2,SIGNAL(clicked(bool)),this,SLOT(onButtonClick()));
    hideTimer = new QTimer(this);
    connect(hideTimer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
    timeOutTime = 0;
    isEnterDialog = false;
    Msg = "";
}


/**
* @brief 有下拉框的窗口
* @date 2018/1/12
* @author han
* @param
* @return void
*/
void MyMessageBox::setCbxDialig(bool flag, QStringList item,QString currentText)
{
    if(flag)  //true显示下拉框
    {
        cbxValue->addItems(item);
        cbxValue->show();
        if(currentText != "")
            cbxValue->setCurrentText(currentText);
        isCbxDialog = true;
        cbxValue->setFocus();
    }
}
/**
* @brief 设置为输入窗口
* @date 2018/1/12
* @author han
* @param
* @return void
*/
void MyMessageBox::setEnterDialog(bool flag)
{
    if(flag)
    {
        leMsg->show();  //输入框显示
        isEnterDialog = true;
        leMsg->setFocus();
    }
}


/**
* @brief 设置表头
* @date 2018/1/12
* @author han
* @param
* @return void
*/
void MyMessageBox::setTitle(QString title)
{
    bar->m_pTitleLabel->setText(title);
}

/**
* @brief true：该提示框为显示窗口，隐藏确定按键
* @date 2017/12/13
* @author han
* @return void
*/
void MyMessageBox::isShowDialog(bool f)
{
    if(f)
    {
        button2->setVisible(false);
        button->setText("返回");
    }
    else
    {
        button2->setVisible(true);
        button->setText("确定");
    }
}

/**
* @brief 开启隐藏窗口timer
* @date 2017/12/13
* @author han
* @return void
*/
void MyMessageBox::startHideTimer(int time)
{
    totalTime = time;
    hideTimer->start(1000);
}

/**
* @brief 定时关闭
* @date 2017/12/13
* @author han
* @return void
*/
void MyMessageBox::onTimeOut()
{
    timeOutTime++;
    if(timeOutTime == totalTime)//到达关闭时间
    {
        hideTimer->stop();
        accept();
    }
    else
    {
        int totalSec = totalTime - timeOutTime;  //剩余秒数
        int sec = totalSec % 60;
        int mm = totalSec / 60;
        QString secStr = QString::number(sec,10);
        if(sec < 10)
            secStr = "0" + secStr;
        QString time = QString::number(mm,10)+ ":" + secStr;
        text->setText("即将进行点名,15分钟后将自动执行点名.<br>" + time);
    }
}


/**
* @brief 确定，取消
* @date 2018/2/22
* @author han
* @param
* @return void
*/
void MyMessageBox::onButtonClick()
{
    myDialogResult = false;
    QPushButton * pButton = qobject_cast<QPushButton*>(sender());
    if(pButton == button)
    {
        if(isEnterDialog)
            Msg = leMsg->text().trimmed();
        if(isCbxDialog)
            Msg = cbxValue->currentText();
        if(isCbxDialog)
        {
            myDialogResult = true;
            this->hide();
        }
        else
           accept();
    }
    else
    {
        if(isCbxDialog)
        {
            myDialogResult = false;
            this->hide();
        }
        else
            reject();
    }
}

/**
* @brief 初始化
* @date 2018/1/12 更新
* @author
* @param BoxStyle 类内自定义枚举类
* @param msg 消息内容
* @return void
*/
void MyMessageBox::iniMsgBox(const BoxStyle style, const QString msg)
{
    text->setText(msg);
    QImage img;

    switch(style)
    {
    case error:
        bar->m_pTitleLabel->setText("错误");
        img.load(":/images/images/MsgBox/error.ico");
        pic->setPixmap(QPixmap::fromImage(img));
        qApp->beep();
        break;
    case success:
        img.load(":/images/images/MsgBox/success.ico");
        pic->setPixmap(QPixmap::fromImage(img));
        break;
    case alert:
        bar->m_pTitleLabel->setText("警告");
        img.load(":/images/images/MsgBox/alert.ico");
        pic->setPixmap(QPixmap::fromImage(img));
        qApp->beep();
        break;
    case info_no_beep:
        bar->m_pTitleLabel->setText("提示");
        img.load(":/images/images/MsgBox/information.ico");
        pic->setPixmap(QPixmap::fromImage(img));
        break;
    case information:
        bar->m_pTitleLabel->setText("提示");
        img.load(":/images/images/MsgBox/information.ico");
        pic->setPixmap(QPixmap::fromImage(img));
        qApp->beep();
        break;
    default:
        bar->m_pTitleLabel->setText("提示");
        img.load(":/images/images/MsgBox/information.ico");
        pic->setPixmap(QPixmap::fromImage(img));
    }
}

void MyMessageBox::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(ev);
}


/**
* @brief 设置输入框类型
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void MyMessageBox::setEchoModePwd()
{
    leMsg->setEchoMode(QLineEdit::Password);
}
MyMessageBox::~MyMessageBox()
{
    delete ui;
}
