#ifndef TRANSMITTHD_H
#define TRANSMITTHD_H

#include <QThread>
#include <QAbstractSocket>

class QTcpSocket;

class TransmitThd : public QThread
{
    Q_OBJECT
public:
    TransmitThd(QTcpSocket* pSocket, QObject* parent = 0);

private:
    void exit(int retcode);

private:
    QTcpSocket* m_pSocket;
    QString m_addrStr;

private slots:
    void on_readyRead();
    void on_disconnected();
    void on_error(QAbstractSocket::SocketError e);

};

#endif // TRANSMITTHD_H
