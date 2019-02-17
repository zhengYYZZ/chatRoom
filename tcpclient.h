#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QDialog>
#include <QHostAddress>
#include <QFile>
#include <QTime>

/*
 * 文件传输客户端
 */

class QTcpSocket;

namespace Ui {
class TcpClient;
}

class TcpClient : public QDialog
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = 0);
    ~TcpClient();

    //设置主机地址
    void setHostAddress(QHostAddress address);
    //设置要传输的文件名
    void setFileName(QString fileName);

protected:
    //关闭事件
    void closeEvent(QCloseEvent *);

private:
    Ui::TcpClient *ui;

    QTcpSocket *tcpClient;
    quint16 blockSize;
    QHostAddress hostAddress;
    qint16 tcpPort;

    qint64 TotalBytes;
    qint64 bytesReceive;
    qint64 bytesToReceive;
    qint64 fileNameSize;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;

    QTime time;

private slots:
    //与服务端连接
    void newConnect();
    //保存接收文件
    void readMessage();
    //显示错误
    void displayError(QAbstractSocket::SocketError);
    //取消按钮--槽
    void on_tcpClientBtn_clicked();
    //关闭按钮--槽
    void on_tcpClientCloseBtn_clicked();
};

#endif // TCPCLIENT_H
