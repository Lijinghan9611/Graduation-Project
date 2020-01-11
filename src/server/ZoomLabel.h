#ifndef ZOOMLABEL_H
#define ZOOMLABEL_H
#include <QLabel>
#include<QPushButton>
#include <QMouseEvent>
#include "Global.h"


class ZoomLabel : public QLabel
{
    Q_OBJECT
public:
    ZoomLabel();
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void closeEvent(QCloseEvent*);
    void mouseMoveEvent(QMouseEvent*event);
    void mouseReleaseEvent(QMouseEvent*event);
    void mousePressEvent(QMouseEvent *event);

public slots:
    void onClick();

signals:
    void zoomEnd();

private:
    QPushButton *closeButton;
    QPoint m_DragPosition;
    bool ispress;
};

#endif // ZOOMLABEL_H
