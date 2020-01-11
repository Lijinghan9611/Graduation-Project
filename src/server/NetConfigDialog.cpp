#include "NetConfigDialog.h"
#include "ui_NetConfigDialog.h"
#include "UiConfigDialog.h"
#include "MyMessageBox.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include "CoolTitleBar.h"
#include "ComboboxItem.h"

NetConfigDialog::NetConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetConfigDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_QuitOnClose);
    setWindowFlags( Qt::FramelessWindowHint| windowFlags());
    CoolTitleBar * bar = new CoolTitleBar(this);
    bar->m_pTitleLabel->setText("电子教室教学管理软件-数据库配置");
    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->addWidget(bar);
    pLayout->addStretch(0);
    pLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pLayout);

    ui->label_4->setText("MySql 参数设置:");
    ui->label_5->setText("数据库用户名:");
    ui->label_6->setText("密码:");
    ui->label_7->setText("数据库主机IP:");
    ui->label_8->setText("数据库名:");

    ui->lineEdit_5->setEchoMode(QLineEdit::Password);
    ui->pushButton->setFocus();
    ui->pushButton_2->clearFocus();
    ui->pushButton->setObjectName("surebutton");
    ui->pushButton_2->setObjectName("commonbutton");
    ui->pushButton_3->setObjectName("commonbutton");

    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    ui->lineEdit_3->setText(set->value("database/hostname").toString());
    ui->lineEdit_4->setText(set->value("database/username").toString());
    ui->lineEdit_5->setText(set->value("database/password").toString());
    ui->lineEdit_6->setText(set->value("database/dbname").toString());
    delete set;
    firstConfig = false;
}

void NetConfigDialog::on_pushButton_clicked()
{
    MyMessageBox box;
    QString hostname,username,passwd,dbname;
    hostname = ui->lineEdit_3->text();
    username = ui->lineEdit_4->text();
    passwd = ui->lineEdit_5->text();
    dbname = ui->lineEdit_6->text();
    if(hostname.trimmed().isEmpty()||username.trimmed().isEmpty()
            ||passwd.trimmed().isEmpty()||dbname.trimmed().isEmpty())
    {
        box.iniMsgBox(MyMessageBox::alert,"请配置好数据库连接信息.");
        box.exec();
        return;
    }

    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);


    set->setValue("database/hostname",hostname);
    set->setValue("database/username",username);
    set->setValue("database/password",passwd);
    set->setValue("database/dbname",dbname);

    set->setValue("config/config","1");
    delete set;
    Global::writeLog("work",QString("修改数据库配置"));
    accept();
}

void NetConfigDialog::on_pushButton_2_clicked()
{
    if(firstConfig)
    {
        firstConfig = false;
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::alert,"确定退出？必须进行初始化配置才能使用此软件.");
        if(box.exec())
            exit(0);
        else
        {
            firstConfig = true;
            return;
        }

    }

    reject();
}


/**
* @brief 返回上一级
* @date 2017/12/17
* @author han
* @return void
*/
void NetConfigDialog::on_pushButton_3_clicked()
{
    this->close();
    UiConfigDialog dia;
    if(firstConfig)
    {
        dia.firstConfig = true;
        dia.dlg.firstConfig = true;
    }
    dia.exec();

}



NetConfigDialog::~NetConfigDialog()
{
    delete ui;
}


