#include"MyObject.h"
#include<QDebug>


MyObject::MyObject()
{
    tcpClient = new TcpClient();
}


void MyObject::objectSlot(void)
{
   while(1)
    qDebug()<<"new thread!";
}
