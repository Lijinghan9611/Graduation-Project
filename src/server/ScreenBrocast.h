#ifndef SCREENBROCAST_H
#define SCREENBROCAST_H

#include <QThread>

class ScreenBrocast : public QThread
{
    Q_OBJECT
public:
    ScreenBrocast();
    void run();

signals:

public slots:
};

#endif // SCREENBROCAST_H
