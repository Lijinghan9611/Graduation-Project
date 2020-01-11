#ifndef CMDTHREAD_H
#define CMDTHREAD_H
#include <QThread>
#include <QDebug>

class CmdThread: public QThread
{
     Q_OBJECT
public:
    CmdThread();
    void run();
};

#endif // CMDTHREAD_H
