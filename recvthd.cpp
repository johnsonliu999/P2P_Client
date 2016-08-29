#include "recvthd.h"

#include <QTcpSocket>

RecvThd::RecvThd(QTcpSocket *pSocket, QObject *parent) :
    QThread(parent),
    m_pSocket(pSocket)
{

}

void RecvThd::exit(int retcode)
{
    m_pSocket->moveToThread(parent()->thread());
    exit(retcode);
}
