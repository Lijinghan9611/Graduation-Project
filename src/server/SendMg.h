#ifndef SENDMG_H
#define SENDMG_H
#include <QWidget>
#include "CmdLineEdit.h"

class SendMg : public QWidget
{
    Q_OBJECT
public:
    explicit SendMg(QWidget *parent = 0);
    void paintEvent(QPaintEvent*);
    CmdLineEdit * pLineEdit;
public slots:
    void onReturnPressed();
signals:
    void mg(const QString&);
};

#endif // SENDMG_H
