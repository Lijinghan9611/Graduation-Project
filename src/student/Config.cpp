#include "Config.h"
#include "ui_Config.h"
#include <QSettings>
#include <QHostInfo>

/**
* @brief 客户端配置服务器ip，端口，机位号（改为自动，此界面不显示）
* @date 2017/12/7
*/
Config::Config(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Config)
{
    ui->setupUi(this);

    Qt::WindowFlags flags =Qt::Dialog;  //隐藏？号
    flags |=Qt::WindowCloseButtonHint;

    setWindowFlags(flags);  //设置窗口标志

	QString hostname = QHostInfo::localHostName(); //主机名
    if (hostname.contains("-")) //规则命名 实验室-机位号
	{
		QStringList list = hostname.split("-");  //把规则的主机名切割好
		hostname = list.at(1);  //获取机位号
		ui->lineEdit_3->setText(hostname);
        ui->labmsg->setText("请检查机位号是否为显示器后的编号。");
	}
	else  //不规则命名
	{		
        ui->labmsg->setText("请输入显示器后方的编号。");
		ui->lineEdit_3->setPlaceholderText("A0");
	}
	
}

Config::~Config()
{
    delete ui;
}

/**
* @brief 确定配置信息
* @date 2017/12/8
*/
void Config::on_pushButton_clicked()
{
    QString position = ui->lineEdit_3->text().toUpper();  //获取机位号并转换为大写

    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    set->setValue("position/position",position);
    set->setValue("config/config","1");

    accept();

    delete set;
}

void Config::on_pushButton_2_clicked()
{
    exit(0);
}

