#include"TcpClient.h"
#include <QDebug>
TcpClient::TcpClient()
{
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));

}

QString TcpClient::readCMD()
{
    readData = tcpSocket->readAll();
    return readData;
}

void TcpClient::sendCMD(QString cmd)
{
    tcpSocket->write(cmd.toLatin1());
}

void TcpClient::newConnection(QString ipAdress,int port)
{
    tcpSocket->abort();
    tcpSocket->connectToHost(ipAdress,port);
}

void TcpClient::displayError(QAbstractSocket::SocketError)
{
    qDebug()<<tcpSocket->errorString();
}

