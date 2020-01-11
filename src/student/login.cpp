#include "login.h"
#include "ui_login.h"
#include "stoast.h"
Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint |Qt::FramelessWindowHint);
    setFixedSize(400,250);
    connected = false;
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeout()));
    timeOutCount = 0;
    timer->start(1000);

    // 设置tab控件焦点切换顺序
    QWidget::setTabOrder(ui->leId, ui->leName);
    QWidget::setTabOrder(ui->leName, ui->pushButton);
    QWidget::setTabOrder(ui->pushButton, ui->leId);

    //设置控件不获取焦点
    ui->radioButton->setFocusPolicy(Qt::NoFocus);
    ui->radioButton_2->setFocusPolicy(Qt::NoFocus);

}


/**
* @brief 未连接前动态改变
* @date 2018/1/26
* @author han
* @param
* @return void
*/
void Login::onTimeout()
{
    timeOutCount ++ ;
    if(timeOutCount == 1)
        ui->labConn->setText("正在连接教师端.");
    else if(timeOutCount == 2)
        ui->labConn->setText("正在连接教师端..");
    else if(timeOutCount == 3)
        ui->labConn->setText("正在连接教师端...");
    else if(timeOutCount == 4)
        ui->labConn->setText("正在连接教师端....");
    else if(timeOutCount == 5)
        ui->labConn->setText("正在连接教师端.....");
    else if(timeOutCount == 6)
    {
        timeOutCount = 0;
        ui->labConn->setText("正在连接教师端......");
    }
}


/**
* @brief 接收连接到的信号
* @date 2018/1/24
* @author han
* @param
* @return void
*/
void Login::onConnected()
{
    SingleTonSToast::getInstance().setMessageVDuration("已连接", 2000);
    ui->radioButton->setEnabled(true);
    ui->radioButton_2->setEnabled(true);
    ui->labConn->setText("已连接");
    ui->labConn->setStyleSheet("QLabel{color:white}");
    connected = true;
    ui->radioButton->click();
    timer->stop();
}

Login::~Login()
{
    delete ui;
}


/**
* @brief 有教师上课
* @date 2018/1/24
* @author han
* @param
* @return void
*/
void Login::on_radioButton_clicked(bool checked)
{
    if(checked && connected)
    {
        ui->leName->setPlaceholderText("有教师上课无需输入");
        ui->leId->setPlaceholderText("有教师上课无需输入");
        ui->leId->setEnabled(false);
        ui->leName->setEnabled(false);
        ui->pushButton->setEnabled(true);
        ui->pushButton->setFocus();
    }
}

/**
* @brief 无教师上课
* @date 2018/1/24
* @author han
* @param
* @return void
*/
void Login::on_radioButton_2_clicked(bool checked)
{
    if(checked && connected)
    {
        ui->leId->setFocus();
        ui->leName->setPlaceholderText("");
        ui->leId->setPlaceholderText("");
        ui->leId->setEnabled(true);
        ui->leName->setEnabled(true);
        ui->pushButton->setEnabled(true);
    }
}

/**
* @brief 登录
* @date 2018/1/24
* @author han
* @param
* @return void
*/
void Login::on_pushButton_clicked()
{
    if(!connected)  //未连接
    {
        SingleTonSToast::getInstance().setMessageVDuration("未连接", 5000,1);
        return;
    }
    QString id = ui->leId->text().trimmed();
    QString name = ui->leName->text().trimmed();
    if(ui->radioButton_2->isChecked())  //无教师上课
    {
        if(id.isEmpty() || name.isEmpty())
        {
            SingleTonSToast::getInstance().setMessageVDuration("请输入以上内容", 5000,1);
            return;
        }

        emit loginSignal(id,name);
    }
    accept();
}

