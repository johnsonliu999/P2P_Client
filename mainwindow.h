#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>

#include <QFile>

class QTcpSocket;
class RecvThd;
class ServerThd;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum {
        PORT = 1234,
        LISTEN_PORT = 5678,
    };

    enum Type {
        LOG_IN,
        LOG_OUT,
        LOOK_UP
    };

    enum RecvStatus {
        SIZE,
        CONTENT
    };

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void recvContent();

private:
    Ui::MainWindow *ui;

    QTcpSocket *m_pSocket;
    QTcpSocket *m_pRecvSocket;

    ServerThd *m_pServerThd;
    quint8 m_task;
    QString m_serverIP;

    quint8 m_recvStatus;
    quint64 m_fileSize;
    quint64 m_recvBytes;

    QFile m_file;


public slots:

private slots:
    void on_connected();
    void on_error(QAbstractSocket::SocketError error);
    void on_hostFound();
    void on_readyRead();

    void on_searchBtn_clicked();

    void on_recvConnected();
    void on_recvError(QAbstractSocket::SocketError error);
    void on_recvReadyRead();
};

#endif // MAINWINDOW_H
