#ifndef CMDLINEEDIT_H
#define CMDLINEEDIT_H
#include <QLineEdit>
#include <QKeyEvent>

class CmdLineEdit :public QLineEdit
{
public:
    CmdLineEdit(QWidget *parent = 0);
    QStringList tip;
    int index;

protected:
    void keyPressEvent(QKeyEvent*);
};

#endif // CMDLINEEDIT_H
