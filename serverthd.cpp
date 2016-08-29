#include "serverthd.h"
#include "transmitthd.h"

#include <QTcpServer>
#include <QTcpSocket>

ServerThd::ServerThd(QObject *parent) :
    QThread(parent),
    m_pServer(new QTcpServer(this))
{
    if (!m_pServer->listen(QHostAddress::Any, LISTEN_PORT)) {
        qDebug() << "listen failed";
        exit(-1);
    }

    qDebug() << "File server started succeed at port :" << LISTEN_PORT << thread();

    connect(m_pServer, &QTcpServer::newConnection, this, &ServerThd::on_newConnection);
    connect(m_pServer, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(on_acceptError(QAbstractSocket::SocketError)));
}

void ServerThd::on_acceptError(QAbstractSocket::SocketError e)
{
    qDebug() << "Accept error :" << m_pServer->errorString();
}


void ServerThd::on_newConnection()
{
    QTcpSocket* pSocket = m_pServer->nextPendingConnection();
    m_addrStr = pSocket->peerAddress().toString();
    qDebug() << "New Connection :" << m_addrStr;

    TransmitThd* pTransThd = new TransmitThd(pSocket, this);
    pSocket->setParent(nullptr);
    pSocket->moveToThread(pTransThd);
    pTransThd->start();
}
