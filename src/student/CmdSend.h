#ifndef CMDSEND_H
#define CMDSEND_H

#include <QObject>
#include <QDebug>

class CmdSend : public QObject
{
    Q_OBJECT
public:
    explicit CmdSend(QObject *parent = 0);

signals:
    void cmdResult(int);
    void cmdCallBack(const QString&);

public slots:
    void onCmd(bool,const QByteArray&);
};

#endif // CMDSEND_H
