#include "transmitthd.h"

#include <QTcpSocket>
#include <QHostAddress>
#include <QFile>

TransmitThd::TransmitThd(QTcpSocket *pSocket, QObject *parent) :
    QThread(parent),
    m_pSocket(pSocket),
    m_addrStr(pSocket->peerAddress().toString())
{
    connect(m_pSocket, &QTcpSocket::readyRead, this, &TransmitThd::on_readyRead);
    connect(m_pSocket, &QTcpSocket::disconnected, this, &TransmitThd::on_disconnected);
    connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError), this, SLOT(on_error(QAbstractSocket::SocketError)));
}

void TransmitThd::on_readyRead()
{
    QString fileName = m_pSocket->readAll();
    qDebug() << "Filename :" << fileName;

    QFile f("up/"+fileName);
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

    quint64 fileSize = f.size();
    if (-1 == m_pSocket->write(fileSize)) {
        qDebug() << m_addrStr << "Write error :" << m_pSocket->errorString();
        exit(-1);
    }

    while (!f.atEnd()) {
        auto bytes = f.read(1000);
        if (-1 == m_pSocket->write(bytes)) {
            qDebug() << m_addrStr << "Write error :" << m_pSocket->errorString();
            exit(-1);
        }
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
