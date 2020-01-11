#ifndef LABELVIEW_H
#define LABELVIEW_H
#include <QLabel>
#include <QWidget>
#include<QContextMenuEvent>
#include "Global.h"
#include "SendMg.h"

class LabelView : public QLabel
{
    Q_OBJECT

public:
    QString ip,mac,name;
    bool on;
    QString userId,userName;
    int oldPositionX;
    QString newPosition;
    QString onlineTime;

public:
    explicit LabelView(QWidget*parent = 0);
    ~LabelView();
    void contextMenuEvent(QContextMenuEvent*);
    void paintEvent(QPaintEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);


public slots:
    void onButtonClick();
    void onClientCome(QString);
    void onClientLeave(QString);

    void onSignMg(const QString&,const QString&,const QString&); //deal sign message

signals:
    void hideLabel(const QString&,const bool &);
    void mornitor(const QString&); //zoom
    void beginCmd(const QString&, const QString &, const QString&);
    void closeComputer(const QString&);
    void restartComputer(const QString&);
    void msg(const QString&);
    void cheakClient(const QString&,const QString&,const QString&);

private:
    QMenu * rightMenu ;
    QAction * cmd;
    QAction * closePc;
    QAction * RestartPc;
    QAction * sendMg;
    QAction * queryClientMg;
    QAction * setClientAction;

};

#endif // LABELVIEW_H
