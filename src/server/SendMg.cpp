#include "SendMg.h"
#include "CoolTitleBar.h"
#include "MyMessageBox.h"
#include <QVBoxLayout>
#include <QDebug>
#include<QStyleOption>
#include<QPainter>

SendMg::SendMg(QWidget *parent) :
    QWidget(parent)
{
    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags( Qt::FramelessWindowHint| windowFlags());
    setMinimumSize(300,100);

    QPushButton* button = new QPushButton(this);
    button->setObjectName("surebutton");
    button->setFixedHeight(40);
    button->setText("发   送");
    QFont font;
    font.setFamily("Adobe 黑体 Std R");
    font.setPixelSize(16);
    button->setFont(font);

    pLineEdit = new CmdLineEdit(this);
    pLineEdit->setFixedHeight(35);
    pLineEdit->setStyleSheet("font-size:15px;font-family:幼圆;");
    pLineEdit->setPlaceholderText("消息");
    pLineEdit->setFocus();
    CoolTitleBar * bar = new CoolTitleBar(this);
    bar->m_pTitleLabel->setText("发送消息");
    bar->setFixedHeight(55);

    QVBoxLayout * pLayout  =  new QVBoxLayout(this);
    pLayout->addWidget(bar);
    pLayout->addWidget(pLineEdit);
    pLayout->addWidget(button);
    pLayout->setSpacing(1);
    pLayout->setContentsMargins(0,0,0,0);
    setLayout(pLayout);

    connect(pLineEdit,SIGNAL(returnPressed()),this,SLOT(onReturnPressed()));
    connect(button,SIGNAL(clicked(bool)),this,SLOT(onReturnPressed()));

}

void SendMg::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(ev);
}

void SendMg::onReturnPressed()
{
    emit mg(pLineEdit->text());
    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::success,"发送成功!");
    box.exec();
    pLineEdit->tip<<pLineEdit->text();
    pLineEdit->index  = pLineEdit->tip.size();
    pLineEdit->clear();
    pLineEdit->setFocus();
}




