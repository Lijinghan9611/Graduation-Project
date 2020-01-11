#include "CoolTitleBar.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

CoolTitleBar::CoolTitleBar(QWidget *parent) : QLabel(parent)
{
    ispress = false;
    setFixedHeight(75);

    m_pTitleLabel = new QLabel(this);
    m_pCloseButton = new QPushButton(this);
    m_pMinButton = new QPushButton(this);

    m_pTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pTitleLabel->setStyleSheet("QLabel{color:white}");
    m_pTitleLabel->setText("电子教室教学管理软件");
    m_pCloseButton->setFixedSize(27, 22);
    m_pMinButton->setFixedSize(27,22);
    m_pCloseButton->setObjectName("coolclose");
    m_pMinButton->setObjectName("coolmin");

    m_pCloseButton->setToolTip("关闭");
    m_pMinButton->setToolTip("最小化");

    //标题图标
    QLabel *labIcon = new QLabel(this);
    QImage img;
    img.load(":/images/images/else/icon1.png");
    labIcon->setMinimumSize(30,30);
    labIcon->setMaximumSize(30,30);
    labIcon->setPixmap(QPixmap::fromImage(img));


    QHBoxLayout *pLayout = new QHBoxLayout(this);
    pLayout->addSpacing(10);
    pLayout->addWidget(labIcon);
    pLayout->addSpacing(2);
    pLayout->addWidget(m_pTitleLabel);
    pLayout->addStretch(0);
    pLayout->addWidget(m_pMinButton,0,Qt::AlignTop);
    pLayout->addWidget(m_pCloseButton,0,Qt::AlignTop);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pLayout);

    connect(m_pCloseButton, SIGNAL(clicked(bool)), this, SLOT(onClick()));
    connect(m_pMinButton,SIGNAL(clicked(bool)),this,SLOT(onClick()));
}

void CoolTitleBar::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(ev);
}

void CoolTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (ispress && event->buttons() && Qt::LeftButton)
    {
        QWidget * p = this->window();
        if(p->isTopLevel())
        {
           p->move(event->globalPos() - m_DragPosition);
           event->accept();
        }
    }
}

void CoolTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    ispress =false;
}

void CoolTitleBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        QWidget * p = this->window();
        if(p->isTopLevel())
        {
            m_DragPosition = event->globalPos() - p->pos();
            ispress = true;
            event->accept();
        }
    }
}

void CoolTitleBar::onClick()
{
    QWidget *pWindow = this->window();
    QPushButton * pButton = qobject_cast<QPushButton*>(sender());
    if(pWindow->isTopLevel())
    {
        if(pButton==m_pCloseButton)
            pWindow->close();
        else
            pWindow->showMinimized();
    }
}
