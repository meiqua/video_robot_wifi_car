#ifndef TCPCLIENT_H
#define TCPCLIENT_H
#include<QtNetwork>
#include<QObject>

class TcpClient:public QObject
{
  Q_OBJECT
public:
    TcpClient();
    QTcpSocket *tcpSocket;
public slots:
    void newConnection(QString ipAdress,int port);
    void sendCMD(QString cmd);
    QString readCMD(void);
    void displayError(QAbstractSocket::SocketError);
signals:
    void connectionSuccess();
protected:

private:
    QString readData;
};
#endif // TCPCLIENT_H
