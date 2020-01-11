#include "CommonTitleBar.h"
#include "ui_CommonTitleBar.h"
#include <QStyleOption>
#include <QPainter>
#include <QDebug>
#include "Global.h"

CommonTitleBar::CommonTitleBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommonTitleBar)
{
    ui->setupUi(this);
    ispress = false;
    setFixedHeight(30);

    m_pIconLabel = new QLabel(this);
    m_pTitleLabel = new QLabel(this);
    m_pCloseButton = new QPushButton(this);
    m_pMinButton = new QPushButton(this);

    QImage img;
    img.load(":/images/images/else/icon1.png");
    m_pIconLabel->setPixmap(QPixmap::fromImage(img));

    m_pIconLabel->setFixedSize(20, 20);
    m_pIconLabel->setScaledContents(true);

    m_pTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pCloseButton->setFixedSize(27, 22);
    m_pMinButton->setFixedSize(27,22);

    m_pTitleLabel->setObjectName("title_label");
    m_pCloseButton->setObjectName("title_closeButton");
    m_pMinButton->setObjectName("title_minButton");

    m_pCloseButton->setToolTip("关闭");
    m_pMinButton->setToolTip("最小化");

    QHBoxLayout *pLayout = new QHBoxLayout(this);
    pLayout->addWidget(m_pIconLabel);
    pLayout->addSpacing(5);
    pLayout->addWidget(m_pTitleLabel);
    pLayout->addWidget(m_pMinButton);
    pLayout->addWidget(m_pCloseButton);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(pLayout);

    connect(m_pCloseButton, SIGNAL(clicked(bool)), this, SLOT(onClicked()));
    connect(m_pMinButton,SIGNAL(clicked(bool)),this,SLOT(onClicked()));
}



void CommonTitleBar::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(ev);
}

void CommonTitleBar::mousePressEvent(QMouseEvent *event)
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

void CommonTitleBar::mouseMoveEvent(QMouseEvent *event)
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

void CommonTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    ispress =false;
}



void CommonTitleBar::onClicked()
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


CommonTitleBar::~CommonTitleBar()
{
    delete ui;
}
