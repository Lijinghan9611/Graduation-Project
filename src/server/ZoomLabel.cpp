#include "ZoomLabel.h"
#include <QVBoxLayout>
#include <QDebug>


/**
* @brief 主界面双击学生机时,放大显示学生桌面
* @date 2018/1/8
* @author han
* @return void
*/
ZoomLabel::ZoomLabel()
{
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("监控");
    setWindowOpacity(1);
    setWindowState(Qt::WindowMaximized);
    setScaledContents(false);
    setAlignment(Qt::AlignCenter);
    QImage img;
    img.load(":/images/images/else/common.jpg");
    setPixmap(QPixmap::fromImage(img));

    QVBoxLayout * layout = new QVBoxLayout(this);

    closeButton = new QPushButton();
    layout->addWidget(closeButton,0,Qt::AlignCenter);
    closeButton->setFixedSize(80,80);
    closeButton->setObjectName("labelclose");

    closeButton->hide();
    connect(closeButton,SIGNAL(clicked(bool)),this,SLOT(onClick()));
}

void ZoomLabel::enterEvent(QEvent *)
{
    closeButton->show();
}

void ZoomLabel::leaveEvent(QEvent *)
{
    closeButton->hide();
}

void ZoomLabel::closeEvent(QCloseEvent *)
{
    emit zoomEnd();
    hide();
}

void ZoomLabel::mousePressEvent(QMouseEvent *event)
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

void ZoomLabel::mouseMoveEvent(QMouseEvent *event)
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

void ZoomLabel::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    ispress =false;
}



/**
* @brief 关闭窗体
* @date 2018/1/8
* @author han
* @return void
*/
void ZoomLabel::onClick()
{
    emit zoomEnd();
    hide();
}
