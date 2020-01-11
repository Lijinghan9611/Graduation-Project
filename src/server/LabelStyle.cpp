#include "LabelStyle.h"
#include<QStyleOption>
#include<QPainter>
#include <QHBoxLayout>
#include <QDebug>
#include <QLabel>

LabelStyle::LabelStyle(QWidget *parent) : QWidget(parent)
{
    setFixedHeight(23);
    currentStyle=WM_Common;

    radio1 = new QRadioButton("低");
    radio2 = new QRadioButton("中");
    radio3 = new QRadioButton("高");
    radio1->setToolTip("图片质量低,广播速度高");
    radio2->setToolTip("图片质量中,广播速度中等");
    radio3->setToolTip("图片质量高,广播速度低,请在网速给力的情况下选择");
    radio2->setChecked(true);
    pCommonButton = new QPushButton(this);
    pMornitorButton = new QPushButton(this);
    pCameraButton = new QPushButton(this);

    pCommonButton->setFixedSize(23,23);
    pMornitorButton->setFixedSize(23,23);
    pCameraButton->setFixedSize(23,23);

    pCommonButton->setObjectName("commonButton");
    pMornitorButton->setObjectName("mornitorButton");
    pCameraButton->setObjectName("cameraButton");

    pCommonButton->setToolTip("普通");
    pMornitorButton->setToolTip("屏幕监控");
    pCameraButton->setToolTip("摄像头");

    currentViewLab = new QLabel("普通");
    currentViewLab->setAlignment(Qt::AlignCenter);
    currentViewLab->setMinimumWidth(80);

    pCommonButton->setEnabled(false);

    QLabel *lab1 = new QLabel("广播质量:");

    QLabel *lab5 = new QLabel("显示类型:");
    QHBoxLayout *pLayout = new QHBoxLayout(this);
    pLayout->addSpacing(9);
    pLayout->addWidget(lab1);
    pLayout->addWidget(radio1);
    pLayout->addSpacing(9);
    pLayout->addWidget(radio2);
    pLayout->addSpacing(9);
    pLayout->addWidget(radio3);
    pLayout->addSpacing(9);
    pLayout->addStretch(0);
    pLayout->addWidget(lab5);
    pLayout->addSpacing(9);
    pLayout->addWidget(pCommonButton);
    pLayout->addWidget(pMornitorButton);
    pLayout->addWidget(pCameraButton);
    pLayout->addWidget(currentViewLab);
    pLayout->addSpacing(10);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, 0, 0, 0);

    connect(pCommonButton,SIGNAL(clicked(bool)),this,SLOT(onButtonClick()));
    connect(pMornitorButton,SIGNAL(clicked(bool)),this,SLOT(onButtonClick()));
    connect(pCameraButton,SIGNAL(clicked(bool)),this,SLOT(onButtonClick()));
    connect(radio1,SIGNAL(clicked(bool)),SLOT(onRadioChange()));
    connect(radio2,SIGNAL(clicked(bool)),SLOT(onRadioChange()));
    connect(radio3,SIGNAL(clicked(bool)),SLOT(onRadioChange()));
    setStyleSheet("QPushButton#commonButton{border-image:url(:/images/images/button/common1.ico)}"
                  "QPushButton#mornitorButton{border-image:url(:/images/images/button/screen2.ico)}"
                  "QPushButton#mornitorButton:hover{border-image:url(:/images/images/button/screen1.ico)}"
                  "QPushButton#cameraButton{border-image:url(:/images/images/button/camera2.ico)}"
                  "QPushButton#cameraButton:hover{border-image:url(:/images/images/button/camera1.ico)}");
}

void LabelStyle::onViewToScreen()
{
    pMornitorButton->click();
}

void LabelStyle::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(ev);

}

void LabelStyle::onButtonClick()
{
    QPushButton * pButton  = qobject_cast<QPushButton*>(sender());
    pButton->setEnabled(false);
    if(pButton == pCommonButton)
    {
        currentViewLab->setText("普通");
        emit styleChange(WM_Common);
        currentStyle=WM_Common;
        Global::style = WM_Common;
        pCameraButton->setEnabled(true);
        pMornitorButton->setEnabled(true);
        setStyleSheet("QPushButton#commonButton{border-image:url(:/images/images/button/common1.ico)}"
                      "QPushButton#mornitorButton{border-image:url(:/images/images/button/screen2.ico)}"
                      "QPushButton#mornitorButton:hover{border-image:url(:/images/images/button/screen1.ico)}"
                      "QPushButton#cameraButton{border-image:url(:/images/images/button/camera2.ico)}"
                      "QPushButton#cameraButton:hover{border-image:url(:/images/images/button/camera1.ico)}");

    }
    else
    {
        if(pButton ==pMornitorButton)
        {
            currentViewLab->setText("屏幕监控");
            emit styleChange(WM_Mornitor);
            currentStyle=WM_Mornitor;
            Global::style = WM_Mornitor;
            pCameraButton->setEnabled(true);
            pCommonButton->setEnabled(true);
            setStyleSheet("QPushButton#mornitorButton{border-image:url(:/images/images/button/screen1.ico)}"
                          "QPushButton#commonButton{border-image:url(:/images/images/button/common2.ico)}"
                          "QPushButton#commonButton:hover{border-image:url(:/images/images/button/common1.ico)}"
                          "QPushButton#cameraButton{border-image:url(:/images/images/button/camera2.ico)}"
                          "QPushButton#cameraButton:hover{border-image:url(:/images/images/button/camera1.ico)}");
        }
        else
        {
            currentViewLab->setText("摄像头");
            emit styleChange(WM_Camera);
            currentStyle=WM_Camera;
            Global::style = WM_Camera;
            pCommonButton->setEnabled(true);
            pMornitorButton->setEnabled(true);
            setStyleSheet("QPushButton#cameraButton{border-image:url(:/images/images/button/camera1.ico)}"
                          "QPushButton#commonButton{border-image:url(:/images/images/button/common2.ico)}"
                          "QPushButton#commonButton:hover{border-image:url(:/images/images/button/common1.ico)}"
                          "QPushButton#mornitorButton{border-image:url(:/images/images/button/screen2.ico)}"
                          "QPushButton#mornitorButton:hover{border-image:url(:/images/images/button/screen1.ico)}");
        }
    }
}


/**
* @brief 改变广播质量
* @date 2017/12/18
* @author han
* @return void
*/
void LabelStyle::onRadioChange()
{
    QRadioButton * button = qobject_cast<QRadioButton*>(sender());
    if(button == radio1)
        emit brocastQuality(30);
    else
    {
        if(button == radio2)
            emit brocastQuality(60);
        else
            emit brocastQuality(90);
    }
}

void LabelStyle::onCheckStyle(const QString& str)
{
    emit getStyle(str,currentStyle);
}
