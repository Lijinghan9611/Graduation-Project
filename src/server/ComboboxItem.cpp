#include "ComboboxItem.h"
#include <QHBoxLayout>
#include <QDebug>

ComboboxItem::ComboboxItem(QWidget *parent) : QWidget(parent)
{
    label = new QLabel(this);
    head  = new QLabel(this);
    name  = new QLabel(this);
    button = new QPushButton(this);

    QImage img(":/images/images/combo/close.ico");
    button->setIcon(QPixmap::fromImage(img));
    button->setIconSize(img.size());
    button->setStyleSheet("background:transparent;");

    QImage img2(":/images/images/combo/head.ico");
    head->setPixmap(QPixmap::fromImage(img2));
    head->setFixedSize(img2.size());

    connect(button,SIGNAL(clicked(bool)),this,SLOT(removeItem()));

    QHBoxLayout * pLayout = new QHBoxLayout(this);
    pLayout->addWidget(head);
    pLayout->addWidget(label);
    pLayout->addSpacing(10);
    pLayout->addWidget(name);
    pLayout->addStretch(0);
    pLayout->addWidget(button);
    pLayout->setContentsMargins(5,5,0,5);
    setLayout(pLayout);

}



void ComboboxItem::mouseReleaseEvent(QMouseEvent * ev)
{
    if(ev->button()==Qt::LeftButton)
        emit itemClick(label->text(),name->text());

}

void ComboboxItem::removeItem()
{
    emit removeItem(label->text(),name->text());
}




