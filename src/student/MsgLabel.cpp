#include "MsgLabel.h"
#include "Global.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>


/**
* @brief 收到文件后，在屏幕头部显示提示框
* @date 2017/12/7
*/
MsgLabel::MsgLabel(QWidget *parent) : QWidget(parent)
{
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint |Qt::FramelessWindowHint);
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect screenRect = desktopWidget->screenGeometry();
    swidth = screenRect.width();
    setGeometry((swidth/2)-this->width()/2,0,350,170);
    stop = new QTimer(this);
    connect(stop,SIGNAL(timeout()),this,SLOT(onStop()));
    tip = new QLabel(this);
    tip->setFixedWidth(300);
    tip->setAlignment(Qt::AlignCenter);
    tip->setText("(鼠标移动到此处,两秒后消失)");
    m_label= new QLabel(this);
    m_label->setObjectName("msglabel");
    m_label->setMinimumWidth(300);
    m_label->setMaximumWidth(600);
    m_label->setWordWrap(true);
    m_label->setAlignment(Qt::AlignCenter);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addStretch(0);
    layout->addWidget(m_label,0,Qt::AlignCenter);
    layout->addStretch(0);
    layout->addWidget(tip,0,Qt::AlignCenter);
    layout->addSpacing(10);
    layout->setContentsMargins(10,10,10,5);
    setLayout(layout);
    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
}

void MsgLabel::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(ev);
}

void MsgLabel::enterEvent(QEvent *)
{
    stop->start(2000);  //计时器开始计时
}

void MsgLabel::leaveEvent(QEvent *)
{
    stop->stop();  //计时器未到达时间，鼠标离开区域
}

void MsgLabel::resizeEvent(QResizeEvent *ev)
{
    this->move((swidth/2)-this->width()/2,0);
    return QWidget::resizeEvent(ev);
}


/**
* @brief 计时结束,隐藏通知
* @date 2017/12/7
*/
void MsgLabel::onStop()
{
    this->hide();
    stop->stop();
}
