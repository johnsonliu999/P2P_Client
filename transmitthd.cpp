#include "transmitthd.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>
#include <QDir>
#include <QDataStream>

TransmitThd::TransmitThd(QTcpSocket *pSocket, QObject *parent) :
    QThread(parent),
    m_pSocket(pSocket),
    m_addrStr(pSocket->peerAddress().toString())
{
    connect(m_pSocket, &QTcpSocket::readyRead, this, &TransmitThd::on_readyRead);
    connect(m_pSocket, &QTcpSocket::disconnected, this, &TransmitThd::on_disconnected);
    connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
}

void TransmitThd::on_readyRead()
{
    QString fileName;
    QDataStream in(m_pSocket);
    in >> fileName;
    qDebug() << "Filename :" << fileName;

    QFile f("up/" + fileName);
    if (!f.exists()) {
        qDebug() << m_addrStr << "file not exist";
        m_pSocket->disconnectFromHost();
        exit(-1);
    }

    if (!f.open(QIODevice::ReadOnly)) {
        qDebug() << m_addrStr << "Open error :" << f.errorString();
        m_pSocket->disconnectFromHost();
        exit(-1);
    }

    QDataStream out(m_pSocket);
    out << f.size();

    while (!f.atEnd()) {
        auto bytes = f.read(1000);
        out << bytes;
    }

    f.close();
    qDebug() << m_addrStr << "Finished transmit" << fileName;
}

void TransmitThd::on_disconnected()
{
    qDebug() << m_addrStr << " disconnected";
    quit();
}

void TransmitThd::on_error(QAbstractSocket::SocketError e)
{
    qDebug() << m_addrStr << "error :" << m_pSocket->errorString();
    exit(-1);
}

void TransmitThd::exit(int retcode)
{
    m_pSocket->moveToThread(this->parent()->thread());
    exit(retcode);
}
