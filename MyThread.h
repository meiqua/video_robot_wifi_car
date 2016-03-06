#ifndef MYTHREAD_H
#define MYTHREAD_H
#include<QThread>
class MyThread:public QThread
{
    Q_OBJECT
public:
    MyThread();
public slots:
    void threadSlot(void);
signals:
    void threadSignal(void);
};
#endif // MYTHREAD_H
