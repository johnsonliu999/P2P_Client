#ifndef SERVERTHD_H
#define SERVERTHD_H

#include <QThread>
#include <QAbstractSocket>

class QTcpServer;

class ServerThd : public QThread
{
    Q_OBJECT

    enum {
        LISTEN_PORT = 5678,
    };
public:
    ServerThd(QObject* parent = 0);

private:
    QTcpServer* m_pServer;

    QString m_addrStr;

public slots:

private slots:
    void on_newConnection();
    void on_acceptError(QAbstractSocket::SocketError e);
};

#endif // SERVERTHD_H
