#ifndef RECVTHD_H
#define RECVTHD_H

#include <QThread>

class QTcpSocket;

class RecvThd : public QThread
{
    Q_OBJECT
public:
    RecvThd(QTcpSocket *pSocket, QObject *parent = 0);

private:
    QTcpSocket *m_pSocket;
    void exit(int retcode);

};

#endif // RECVTHD_H
