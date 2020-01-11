#ifndef COOLTITLEBAR_H
#define COOLTITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include "Global.h"


class CoolTitleBar : public QLabel
{
    Q_OBJECT

public:
    QLabel *m_pTitleLabel;
    QPushButton *m_pCloseButton;
    QPushButton *m_pMinButton;

private:
    QPoint m_DragPosition;
    bool ispress;

public:
    explicit CoolTitleBar(QWidget *parent = 0);
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent*event);
    void mouseReleaseEvent(QMouseEvent*event);
    virtual void mousePressEvent(QMouseEvent *event);

public slots:
    void onClick();

};

#endif // COOLTITLEBAR_H
