#ifndef COMPAREPORTRAIT_H
#define COMPAREPORTRAIT_H

#include <QWidget>
#include <QPaintEvent>

namespace Ui {
class ComparePortrait;
}

class ComparePortrait : public QWidget
{
    Q_OBJECT

public:
    explicit ComparePortrait(QWidget *parent = 0);
    ~ComparePortrait();
    void paintEvent(QPaintEvent*);

public slots:
    void onBegin(int);
    void onClientImgCome(const QString&,const QImage&);

private:
    Ui::ComparePortrait *ui;
};

#endif // COMPAREPORTRAIT_H
