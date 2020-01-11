#ifndef SIGN_H
#define SIGN_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

class Sign : public QLabel
{
    Q_OBJECT
public:
    explicit Sign(QWidget *parent = 0);
    QLineEdit * id ;
    QLineEdit * name;
    QPushButton * button;

signals:
    void signMg(const QString&,const QString&);

public slots:
    void onClick();

};

#endif // SIGN_H
