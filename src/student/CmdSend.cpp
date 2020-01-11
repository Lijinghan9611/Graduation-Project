#include "CmdSend.h"
#include "Global.h"
#include <QThread>
#include <cstdlib>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QDebug>

CmdSend::CmdSend(QObject *parent) : QObject(parent)
{

}

void CmdSend::onCmd(bool onlyone ,const QByteArray& str)
{

    if(!onlyone)
    {
        int  result = system(str);
        emit cmdResult(result);
    }
    else
    {
        QString path = QCoreApplication::applicationDirPath();
        QString cmdTxtPath = path + "/cmd.txt";
        int result = system(str+">>"+cmdTxtPath.toUtf8());
        if(result != 0)
        {
            emit cmdCallBack(QString::number(result));
            return;
        }
        QFile f(path + "/cmd.txt");
        if(!f.open(QIODevice::ReadWrite | QIODevice::Text))
            emit cmdCallBack("error");

        QTextStream txtInput(&f);
        QString allStr;
        while(!txtInput.atEnd())
        {
            allStr += txtInput.readLine()+"|+|";
        }
        emit cmdCallBack(allStr);
        f.remove();
        f.close();
    }
}

