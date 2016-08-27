#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>

class QTcpSocket;

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

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QTcpSocket *m_pSocket;
    QTcpSocket *m_pRecvSocket;
    quint8 m_task;
    QString m_serverIP;

public slots:

private slots:
    void on_connected();
    void on_error(QAbstractSocket::SocketError error);
    void on_hostFound();
    void on_searchBtn_clicked();
    void on_readyRead();

    void on_recv_connected();
    void on_recv_error();
};

#endif // MAINWINDOW_H
