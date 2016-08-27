#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QInputDialog>
#include <QTcpSocket>
#include <QErrorMessage>

#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pSocket(new QTcpSocket),
    m_pRecvSocket(new QTcpSocket),
    m_task(LOG_IN)
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

    connect(m_pRecvSocket, &QTcpSocket::connected, this, &MainWindow::on_recv_connected);
    connect(m_pRecvSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_recv_error(QAbstractSocket::SocketError)));
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

    delete ui;
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
    ui->nameEdit->setEnabled(false);

    m_pSocket->connectToHost(m_serverIP, PORT);
}

void MainWindow::on_recv_connected()
{
    qDebug() << "Recv connected";
}

void MainWindow::on_recv_error(QAbstractSocket::SocketError error)
{
    qDebug() << "Recv Error :" << m_pRecvSocket->errorString();
}

void MainWindow::on_readyRead()
{
    QString ip = m_pSocket->readAll();
    if (ip.isNull()) {
        qDebug() << "No Available IP";
        ui->availableEdit->setText("No Available");
        ui->nameEdit->setEnabled(true);
        return;
    }

    qDebug() << "Recv Ip : " << ip;
    m_pRecvSocket.connectToHost(ip, LISTEN_PORT);
}
