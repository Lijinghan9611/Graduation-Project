// 仿Android的toast提示框
// 适宜将此类实现为多线程下的单例模式

#ifndef STOAST_H
#define STOAST_H

#ifdef Q_OS_WIN32  //Windows 下使用VC++编译器的utf-8字符集解析源码
    #if _MSC_VER  >=1600
        #pragma execution_character_set("utf-8")
    #endif
#endif

//// SToast 部分用到
#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>
#include <QPainter>
#include <QEvent>
#include <QDesktopWidget>
#include <QApplication>
#include <QPropertyAnimation>

//// SingleTonSToast 部分用到
#include <QMutex>
#include <QReadWriteLock>
#include <QAtomicPointer>
#include <QDebug>

class SToast : public QWidget
{
    Q_OBJECT
public:
    explicit SToast(QWidget *parent = 0);
    ~SToast();

    int duration()const;
    void SetDuration(int msec);  //持续显示时间，ms

signals:

public slots:
    void setMessageVDuration(QString msg, int msecDisplayTime=2500,int type = 0);  //消息显示多少毫秒消失

protected:
    void paintEvent(QPaintEvent *event);

private:
    QHBoxLayout *hLayout;
    QVBoxLayout *vLayout;
    QLabel *label;   //提示文字
    QLabel *labIcon; //图标
    int msDuration;  //单位ms
    QTimer durationTimer;
    QPropertyAnimation *animation;

    void fadeInAnimation();   //淡出动画
    void fadeOutAnimation();  //淡入动画

private slots:
    void timeOver();  //定时器超时响应函数
    void fadeInAnimationFinished();  //淡出动画结束响应槽
};

class SingleTonSToast
{
public:
    /*! \brief 用于获得SingleTonSToast实例，使用单例模式。
     *  \return SingleTonSToast实例的引用。
     */
    static SToast &getInstance(void)
    {
//#ifndef Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
//        if(!QAtomicPointer</*SingleTonSToast*/SToast>::isTestAndSetNative())  //运行时检测
//            qDebug() << "SingleTonSToast类 Error: TestAndSetNative not supported!";
//#endif
        //使用双重检测。
        /*! testAndSetOrders操作保证在原子操作前和后的的内存访问
         * 不会被重新排序。
         */
        if(instance.testAndSetOrdered(0, 0)){  //第一次检测
            QMutexLocker locker(&mutex);//加互斥锁。
            instance.testAndSetOrdered(0, new /*SingleTonSToast*/SToast);//第二次检测。
        }

        return *instance;
    }

private:
    SingleTonSToast();  //禁止构造函数。
    SingleTonSToast(const SingleTonSToast &);//禁止拷贝构造函数。
    SingleTonSToast & operator=(const SingleTonSToast &);//禁止赋值拷贝函数。

    QReadWriteLock internalMutex;   //函数使用的读写锁。
    static QMutex mutex;            //实例互斥锁。
    static QAtomicPointer</*SingleTonSToast*/SToast> instance;  /*!<使用原子指针,默认初始化为0。*/
};

#endif // STOAST_H
