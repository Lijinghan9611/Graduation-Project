#include "BrocastLabel.h"
#include <qdebug.h>

BrocastLabel::BrocastLabel(QWidget *parent) : QLabel(parent)
{
    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowState(Qt::WindowFullScreen);  //全屏显示
    //setFixedSize(800,400);  //测试
    setWindowTitle("屏幕广播");
    setAlignment(Qt::AlignCenter);
    setCursor(Qt::BlankCursor);
}

/**
* @brief 收到广播图片,显示
* @date 2017/12/7
*/
void BrocastLabel::onImgCome(const QImage &img)
{   
    this->setPixmap(QPixmap::fromImage(img));
}

/**
* @brief 不让关闭窗体
* @date 2017/12/7
*/
void BrocastLabel::closeEvent(QCloseEvent *ev)
{
    ev->ignore();
}
