#include "CmdLineEdit.h"
#include <QDebug>

CmdLineEdit::CmdLineEdit(QWidget *parent):QLineEdit(parent)
{
    index = 0;
}

void CmdLineEdit::keyPressEvent(QKeyEvent *ev)
{

    if(ev->key()==Qt::Key_Up)
    {
        if(index<=tip.size() && index > 0)
        {
            if(this->text()==tip.at(index-1) && index-2>=0)
                index = index-2;
            else
                index --;
            setText(tip.at(index));
        }
    }
    if(ev->key()==Qt::Key_Down)
    {
        if(index==0 && index <tip.size()-1)
        {
            index ++;
            setText(tip.at(index));
        }
        if(index <tip.size())
        {
            setText(tip.at(index));
            index ++;
        }
    }

    return QLineEdit::keyPressEvent(ev);
}
