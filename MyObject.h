#ifndef MYOBJECT_H
#define MYOBJECT_H
#include<QObject>
#include<TcpClient.h>
class MyObject: public QObject
{
    Q_OBJECT
public:
    MyObject();
    TcpClient *tcpClient;
public slots:
    void objectSlot(void);
signals:
    void objectSignal(void);
protected:
};
#endif // MYOBJECT_H
