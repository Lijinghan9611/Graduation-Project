#include "ClientAtribute.h"
#include "ui_ClientAtribute.h"
#include "CoolTitleBar.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QStyleOption>


/**
* @brief 查询学生端机器信息
* @date 2018/1/
* @author han
* @param
* @return void
*/
ClientAtribute::ClientAtribute(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientAtribute)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags( Qt::FramelessWindowHint| windowFlags());
    setWindowTitle("信息查询");

    CoolTitleBar * bar = new CoolTitleBar(this);
    bar->m_pTitleLabel->setStyleSheet("QLabel{font-size:18px;color:white;font-family:幼圆;}");
    bar->m_pTitleLabel->setText("信息查询");
    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->addWidget(bar);
    pLayout->addStretch(0);
    pLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pLayout);

}


void ClientAtribute::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ClientAtribute::closeEvent(QCloseEvent *)
{
    ui->label_8->setText("正在查询中...");
    ui->label_9->setText("正在查询中...");
    ui->label_10->setText("正在查询中...");
    ui->label_11->setText("正在查询中...");
    ui->label_12->setText("正在查询中...");
    ui->label_13->setText("正在查询中...");
    ui->label_14->setText("正在查询中...");
}


ClientAtribute::~ClientAtribute()
{
    delete ui;
}
