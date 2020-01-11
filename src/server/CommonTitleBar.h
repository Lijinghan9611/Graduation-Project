#ifndef COMMONTITLEBAR_H
#define COMMONTITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>

namespace Ui {
class CommonTitleBar;
}

class CommonTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit CommonTitleBar(QWidget *parent = 0);
    ~CommonTitleBar();
    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent*event);
    void mouseReleaseEvent(QMouseEvent*event);
    virtual void mousePressEvent(QMouseEvent *event);

public:
    QLabel *m_pIconLabel;
    QLabel *m_pTitleLabel;

private:
    Ui::CommonTitleBar *ui;
    QPushButton *m_pCloseButton;
    QPushButton *m_pMinButton;

public slots:

    void onClicked();

private:
    void updateMaximize();
    QPoint m_DragPosition;
    bool ispress;

};

#endif // COMMONTITLEBAR_H
