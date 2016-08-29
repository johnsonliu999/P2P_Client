#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "recvthd.h"
#include "serverthd.h"

#include <QInputDialog>
#include <QTcpSocket>
#include <QErrorMessage>

#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pSocket(new QTcpSocket),
    m_pRecvSocket(new QTcpSocket),
    m_task(LOG_IN),
    m_recvStatus(SIZE),
    m_fileSize(0),
    m_recvBytes(0)
{
    ui->setupUi(this);

    bool ok;
    m_serverIP = QInputDialog::getText(nullptr, "Log in", "Server IP :", QLineEdit::Normal, "", &ok);

    if (!ok) exit(-1);

    m_pSocket->connectToHost(m_serverIP, PORT);
    connect(m_pSocket, &QTcpSocket::connected, this, &MainWindow::on_connected);
    connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
    connect(m_pSocket, &QTcpSocket::hostFound, this, &MainWindow::on_hostFound);
    connect(m_pSocket, &QTcpSocket::readyRead, this, &MainWindow::on_readyRead);

    connect(m_pRecvSocket, &QTcpSocket::connected, this, &MainWindow::on_recvConnected);
    connect(m_pRecvSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_recvError(QAbstractSocket::SocketError)));
    connect(m_pRecvSocket, &QTcpSocket::readyRead, this, &MainWindow::on_recvReadyRead);
}

MainWindow::~MainWindow()
{
    disconnect(m_pSocket, &QTcpSocket::connected, this, &MainWindow::on_connected);
    disconnect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_error(QAbstractSocket::SocketError)));
    disconnect(m_pSocket, &QTcpSocket::hostFound, this, &MainWindow::on_hostFound);
    m_pSocket->connectToHost(m_serverIP, PORT);
    if (!m_pSocket->waitForConnected())
        qDebug() << "Connect time out";
    QDataStream out(m_pSocket);
    out << quint8(LOG_OUT);
    m_pSocket->disconnectFromHost();
    if (!m_pSocket->waitForDisconnected())
        qDebug() << "Disconnect time out";

    m_pSocket->deleteLater();
    m_pRecvSocket->deleteLater();

    m_pServerThd->quit();
    m_pServerThd->wait();

    delete ui;
}

inline void MainWindow::recvContent()
{
    QDataStream in(m_pRecvSocket);
    QByteArray bytes;
    in >> bytes;
    m_recvBytes += bytes.size();
    qDebug() << "bytes :" << m_recvBytes;
    m_file.write(bytes);

    if (m_recvBytes == m_fileSize) {
        statusBar()->showMessage("Finished recv file", 2);
        m_file.close();
        m_recvStatus = SIZE;
    }
}

void MainWindow::on_connected()
{
    qDebug() << "Connect succeed";
    QDataStream out(m_pSocket);

    if (m_task == LOG_IN) {
        this->show();
        out << quint8(LOG_IN);

        QDir dir("./up");
        dir.setFilter(QDir::Files);
        auto files = dir.entryList();
        out << files;

        m_pSocket->disconnectFromHost();

        m_pServerThd = new ServerThd(this);
        m_pServerThd->start();

        m_task = LOOK_UP;

    } else if (m_task == LOOK_UP) {
        out << quint8(LOOK_UP);
        out << ui->nameEdit->text();
    }

}

void MainWindow::on_error(QAbstractSocket::SocketError error)
{
    this->hide();
    qDebug() << "Error :" << m_pSocket->errorString();

    bool ok;
    auto ip = QInputDialog::getText(nullptr, "Try Again", "Server IP :", QLineEdit::Normal, "", &ok);
    if (!ok) this->deleteLater();
    m_pSocket->abort();
    m_pSocket->connectToHost(ip, PORT);
}

void MainWindow::on_hostFound()
{
    qDebug() << "Host found";
}

void MainWindow::on_searchBtn_clicked()
{
    QString name = ui->nameEdit->text();
    if (name == "") return;

    ui->availableEdit->clear();
    ui->sizeEdit->clear();
    ui->nameEdit->setEnabled(false);
    m_fileSize = 0;

    m_pSocket->connectToHost(m_serverIP, PORT);
}

void MainWindow::on_recvConnected()
{
    qDebug() << "Recv connected";
    QDataStream out(m_pRecvSocket);
    out << ui->nameEdit->text();
}

void MainWindow::on_recvError(QAbstractSocket::SocketError error)
{
    qDebug() << "Recv Error :" << m_pRecvSocket->errorString();
}

void MainWindow::on_recvReadyRead()
{
    QDataStream in(m_pRecvSocket);
    if (m_recvStatus == SIZE) {    
        in >> m_fileSize;

        qDebug() << "file size :" << m_fileSize;
        ui->sizeEdit->setText(QString::number(m_fileSize));

        if (!m_fileSize) return;
        m_recvStatus = CONTENT;

        m_file.setFileName("down/" + ui->nameEdit->text());
        qDebug() << m_file.fileName();
        if (!m_file.open(QIODevice::WriteOnly)) {
            qDebug() << "Open file error" << m_file.errorString();
            return ;
        }

        if (m_pRecvSocket->bytesAvailable())
            recvContent();
    } else if (m_recvStatus == CONTENT) {
        recvContent();
    }

}

void MainWindow::on_readyRead()
{
    QDataStream in(m_pSocket);
    QString ip;
    in >> ip;
    m_pSocket->disconnectFromHost();

    if (ip.isNull()) {
        qDebug() << "No Available IP";
        ui->availableEdit->setText("No Available");
        ui->nameEdit->setEnabled(true);
        return;
    }

    ui->availableEdit->setText(ip);
    qDebug() << "Recv Ip : " << ip;
    m_pRecvSocket->connectToHost(ip, LISTEN_PORT);
}
