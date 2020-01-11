#ifndef MSGLABEL_H
#define MSGLABEL_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QResizeEvent>

class MsgLabel : public QWidget
{
    Q_OBJECT
public:
    explicit MsgLabel(QWidget *parent = 0);
    void paintEvent(QPaintEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);
    void resizeEvent(QResizeEvent*);
    QLabel * m_label;
    int swidth;
private:
    QLabel * tip;
    QTimer *stop;
signals:

public slots:
    void onStop();
};

#endif // MSGLABEL_H
