#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>
#include "Global.h"


namespace Ui {
class Config;
}

class Config : public QDialog
{
    Q_OBJECT

public:
    explicit Config(QWidget *parent = 0);

    ~Config();

signals:

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Config *ui;
};

#endif // CONFIG_H
