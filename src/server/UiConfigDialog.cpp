#include "UiConfigDialog.h"
#include "ui_UiConfigDialog.h"
#include <QVBoxLayout>
#include "CoolTitleBar.h"
#include "QSettings"
#include "MyMessageBox.h"
#include <qt_windows.h>

UiConfigDialog::UiConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UiConfigDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_QuitOnClose);
    setWindowFlags( Qt::FramelessWindowHint| windowFlags());
    CoolTitleBar * bar = new CoolTitleBar(this);
    bar->m_pTitleLabel->setText("电子教室教学管理软件-实验室配置");
    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->addWidget(bar);
    pLayout->addStretch(0);
    pLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pLayout);

    QImage horizon(":/images/images/ui/horizon.png");
    QImage vertical(":/images/images/ui/vertical.png");
    ui->label_5->setPixmap(QPixmap::fromImage(horizon));
    ui->label_6->setPixmap(QPixmap::fromImage(vertical));

    ui->lineEdit_3->setToolTip("输入缺失的原机位号,以空格分隔");
    ui->radioButton->click();
    ui->pushButton->setObjectName("surebutton");
    ui->pushButton_2->setObjectName("commonbutton");

    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    QString direction;
    direction = set->value("ui/direction","horizon").toString();
    if(direction =="horizon")
        ui->radioButton->click();
    else
        ui->radioButton_2->click();
    if(direction == "horizon")
    {
        ui->lineEdit->setText(set->value("ui/x").toString());
        ui->lineEdit_2->setText(set->value("ui/y").toString());
    }
    else
    {
        ui->lineEdit->setText(set->value("ui/y").toString());
        ui->lineEdit_2->setText(set->value("ui/x").toString());
    }
    //限制只能输入数字
    QRegExp regx("[0-9]+$");
    ui->lineEdit->setValidator(new QRegExpValidator(regx, ui->lineEdit));
    ui->lineEdit_2->setValidator(new QRegExpValidator(regx, ui->lineEdit_2));


    ui->lineEdit_3->setText(set->value("ui/lack").toString());
    ui->lineEdit_4->setText(set->value("ui/iniPoint","A0").toString());
    delete set;
    firstConfig = false;
}

void UiConfigDialog::on_pushButton_clicked()
{
    MyMessageBox box;
    QString iniPoint = ui->lineEdit_4->text().trimmed();
    if(iniPoint.size() != 2)
    {
        box.iniMsgBox(MyMessageBox::error,QString("起始位置错误,例如：A0"));
        box.isShowDialog(true);
        box.exec();
        return;
    }

    if(ui->lineEdit->text()=="")
    {       
        box.iniMsgBox(MyMessageBox::error,QString("请输入横向数量"));
        box.isShowDialog(true);
        box.exec();
        return;
    }
    if(ui->lineEdit_2->text()=="")
    {      
        box.iniMsgBox(MyMessageBox::error,QString("请输入纵向数量"));
        box.isShowDialog(true);
        box.exec();
        return;
    }
    QString x = ui->lineEdit->text();
    QString y = ui->lineEdit_2->text();
    QString direction;
    if(ui->radioButton->isChecked())
        direction = "horizon";
    else
        direction = "vertical";
    QString lack = ui->lineEdit_3->text().toUpper();

    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    if(direction == "horizon")
    {
        set->setValue("ui/x",x);
        set->setValue("ui/y",y);
    }
    else
    {
        set->setValue("ui/x",y);
        set->setValue("ui/y",x);
    }
    set->setValue("ui/direction",direction);
    set->setValue("ui/lack",lack);
    if(iniPoint == "")
        set->setValue("ui/iniPoint","A0");
    else
        set->setValue("ui/iniPoint",iniPoint);

    delete set;
    Global::writeLog("work",QString("修改实验室配置"));

    accept();
    dlg.exec();
}

void UiConfigDialog::on_pushButton_2_clicked()
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

    this->close();

}


UiConfigDialog::~UiConfigDialog()
{
    delete ui;
}
