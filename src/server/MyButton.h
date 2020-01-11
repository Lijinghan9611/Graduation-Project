#ifndef MYBUTTON_H
#define MYBUTTON_H
#include <QPushButton>
#include <QLabel>

class MyButton : public QPushButton
{
public:
    MyButton(QWidget *parent = 0);
    QLabel *imgLabel;
    QLabel *wordLabel;
};

#endif // MYBUTTON_H
