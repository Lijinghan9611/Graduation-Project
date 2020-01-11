#ifndef BROCASTLABEL_H
#define BROCASTLABEL_H

#include <QLabel>
#include <QCloseEvent>
#include "Global.h"

/**
* @brief 显示广播界面
* @date 2017/12/7
*/
class BrocastLabel : public QLabel
{
    Q_OBJECT
public:
    explicit BrocastLabel(QWidget *parent = 0);
    void closeEvent(QCloseEvent*);

signals:

public slots:
    void onImgCome(const QImage&);
};

#endif // BROCASTLABEL_H
