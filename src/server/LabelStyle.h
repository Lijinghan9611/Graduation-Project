#ifndef LABELSTYLE_H
#define LABELSTYLE_H

#include <QWidget>
#include <QPaintEvent>
#include <QPushButton>
#include <QRadioButton>
#include "Global.h"
#include <QLabel>

class LabelStyle : public QWidget
{
    Q_OBJECT
public:
    explicit LabelStyle(QWidget *parent = 0);
    void paintEvent(QPaintEvent*);

public:
    quint32 currentStyle;

private:
    QPushButton *pCommonButton;
    QPushButton *pMornitorButton;
    QPushButton *pCameraButton;
    QRadioButton * radio1;
    QRadioButton * radio2;
    QRadioButton * radio3;
    QLabel *currentViewLab;

signals:
    void getStyle(const QString&,quint32);
    void styleChange(quint32);

    void brocastQuality(int);

public slots:
    void onViewToScreen();
    void onButtonClick();
    void onRadioChange();
    void onCheckStyle(const QString&);  //cheack/get :initialize change:from button

};

#endif // LABLESTYLE_H
