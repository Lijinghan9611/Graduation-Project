#include "Question.h"
#include "Global.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QMovie>


/**
* @brief 回答问题界面
* @date 2018/1/25
* @author han
* @param
* @return void
*/
Question::Question(QWidget *parent) : QLabel(parent)
{
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint |Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(400,250);

    QMovie * gif = new QMovie(":/image/images/2.gif",QByteArray(),this);
    setAlignment(Qt::AlignVCenter);
    this->setMovie(gif);
    gif->start();

    QWidget *firstWid = new QWidget(this);
    m_pQuestion = new QLabel(this);
    m_pAnswer = new QLineEdit(this);
    button = new QPushButton(this);
    m_pAnswer->setFixedSize(300,35);
    m_pAnswer->setFocusPolicy(Qt::StrongFocus);
    button->setFixedSize(60,23);
    button->setText("确定");

    QHBoxLayout * fLayout  = new QHBoxLayout(this);
    fLayout->addSpacing(50);
    fLayout->addWidget(m_pQuestion);
    fLayout->setContentsMargins(0,0,0,0);
    firstWid->setLayout(fLayout);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addStretch(0);
    layout->addWidget(firstWid);
    layout->addSpacing(10);
    layout->addWidget(m_pAnswer,0,Qt::AlignCenter);
    layout->addStretch(0);
    layout->addWidget(button,0,Qt::AlignRight);
    layout->setContentsMargins(0,0,5,5);
    setLayout(layout);

    connect(button,SIGNAL(clicked()),this,SLOT(onClick()));
}

void Question::onClick()
{
    emit AnswerMg(m_pAnswer->text());
    m_pAnswer->clear();
    m_pQuestion->clear();
    this->hide();
}





