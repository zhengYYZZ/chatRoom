#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QDialog>
#include <QTime>
namespace Ui {
class TcpServer;
}

class QTcpServer;
class QTcpSocket;
class QFile;

class TcpServer : public QDialog
{
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = 0);
    ~TcpServer();

    //用于初始化
    void initServer();
    //拒绝接收文件
    void refused();

protected:
    //关闭事件
    void closeEvent(QCloseEvent *);

private:
    Ui::TcpServer *ui;
    qint16 tcpPort;
    QTcpServer *tcpServer;
    QString fileName;
    QString theFileName;
    QFile *localFile;
    qint64 payloadSize;
    qint64 totalBytes;
    qint64 bytesWritten;
    qint64 bytesToWrite;
    QByteArray outBlock;

    QTcpSocket *clientConnection;

    QTime time;

private slots:
    //发送数据
    void sendMessage();
    //更新进度条--槽
    void updateClientProgress(qint64 numBytes);
    //打开按钮--槽
    void on_serverOpenBtn_clicked();
    //发送按钮--槽
    void on_serverSendBtn_clicked();
    //关闭按钮--槽
    void on_serverCloseBtn_clicked();

signals:
    void sendFileName(QString fileName);
};

#endif // TCPSERVER_H
