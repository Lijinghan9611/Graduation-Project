#include "Sign.h"
#include "Global.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QWidget>
#include <QMovie>


/**
* @brief 点名签到界面
* @date 2018/1/25
* @author han
* @param
* @return void
*/
Sign::Sign(QWidget *parent) : QLabel(parent)
{
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint |Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    QMovie * gif = new QMovie(":/image/images/gif.gif",QByteArray(),this);
    setAlignment(Qt::AlignVCenter);
    this->setMovie(gif);
    gif->start();
    setFixedSize(400,250);
    id = new QLineEdit(this);
    name = new QLineEdit(this);
    button = new QPushButton(this);
    QLabel *label1 = new QLabel(this);
    QLabel *label2 = new QLabel(this);
    QLabel *label3 = new QLabel(this);

    QWidget * firstWid = new QWidget(this);
    QWidget * midWid = new QWidget(this);
    QWidget * lastWid = new QWidget(this);

    id->setFixedSize(200,35);
    id->setFocusPolicy(Qt::StrongFocus);
    name->setFixedSize(200,35);
    button->setFixedSize(60,23);
    button->setText("确定");
    label1->adjustSize();
    label1->setText("点名已开始");
    label2->adjustSize();
    label2->setText("学号:");
    label3->adjustSize();
    label3->setText("姓名:");

    QHBoxLayout * fLayout  = new QHBoxLayout(this);
    fLayout->addSpacing(77);
    fLayout->addWidget(label1);
    fLayout->setContentsMargins(0,0,0,0);
    firstWid->setLayout(fLayout);

    QHBoxLayout * sLayout  = new QHBoxLayout(this);
    sLayout->addStretch(0);
    sLayout->addWidget(label2);
    sLayout->addWidget(id);
    sLayout->addStretch(0);
    sLayout->setContentsMargins(0,0,0,0);
    midWid->setLayout(sLayout);

    QHBoxLayout * lLayout  = new QHBoxLayout(this);
    lLayout->addStretch(0);
    lLayout->addWidget(label3);
    lLayout->addWidget(name);
    lLayout->addStretch(0);
    lLayout->setContentsMargins(0,0,0,0);
    lastWid->setLayout(lLayout);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addStretch(0);
    layout->addWidget(firstWid);
    layout->addSpacing(5);
    layout->addWidget(midWid);
    layout->addWidget(lastWid);
    layout->addSpacing(45);
    layout->addWidget(button,0,Qt::AlignRight);
    layout->setContentsMargins(0,0,5,5);
    setLayout(layout);

    connect(button,SIGNAL(clicked()),this,SLOT(onClick()));
}

void Sign::onClick()
{
    emit signMg(id->text(),name->text());
    id->clear();
    name->clear();
    this->hide();
}


















