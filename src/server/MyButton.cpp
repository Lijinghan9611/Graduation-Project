#include "MyButton.h"
#include "Global.h"
#include <QVBoxLayout>

MyButton::MyButton(QWidget *parent) : QPushButton(parent)
{
    setFixedSize(90,98);
    setObjectName("mybutton");

    imgLabel = new QLabel(this);
    wordLabel = new QLabel(this);
    imgLabel->setFixedSize(44,44);
    imgLabel->setScaledContents(true);
    wordLabel->setObjectName("mybuttontext");

    QVBoxLayout *pLayout =new QVBoxLayout();
    pLayout->addStretch(0);
    pLayout->addWidget(imgLabel,0,Qt::AlignCenter);
    pLayout->addSpacing(10);
    pLayout->addWidget(wordLabel,0,Qt::AlignCenter);
    pLayout->addSpacing(10);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pLayout);
}
