#ifndef CLIENTATRIBUTE_H
#define CLIENTATRIBUTE_H

#include <QWidget>

namespace Ui {
class ClientAtribute;
}

class ClientAtribute : public QWidget
{
    Q_OBJECT

public:
    explicit ClientAtribute(QWidget *parent = 0);
    ~ClientAtribute();
    void paintEvent(QPaintEvent*);
    void closeEvent(QCloseEvent*);
    Ui::ClientAtribute *ui;
};

#endif // CLIENTATRIBUTE_H
