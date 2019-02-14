#include "charroom.h"
#include "ui_charroom.h"
#include "tcpserver.h"
#include "tcpclient.h"
#include <QFileDialog>
#include <QUdpSocket>
#include <QHostInfo>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QNetworkInterface>
#include <QProcess>
#include <QColorDialog>
#include <QTextCharFormat>

charRoom::charRoom(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::charRoom)
{
    ui->setupUi(this);
    udpSocket = new QUdpSocket(this);
    port = 45454;   //设置端口号
    udpSocket->bind(port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);    //绑定端口
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    sendMessage(NewParticipant);

    server = new TcpServer(this);
    connect(server,SIGNAL(sendFileName(QString)),
            this,SLOT(getFileName(QString)));
    connect(ui->messageTextEdit,SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this,SLOT(currentFormatChanged(QTextCharFormat)));
}

charRoom::~charRoom()
{
    delete ui;
}

//成员加入处理函数
void charRoom::newParticipant(QString userName, QString localHostName, QString ipAddress)
{
    //使用主机名判断用户是否已经加入
    bool isEmpty = ui->userTableWidget
            ->findItems(localHostName,Qt::MatchExactly).isEmpty();

    //如果用户未加入则向界面右侧的用户列表加入新用户的信息
    if(isEmpty) {
        QTableWidgetItem *user = new QTableWidgetItem(userName);
        QTableWidgetItem *host = new QTableWidgetItem(localHostName);
        QTableWidgetItem *ip = new QTableWidgetItem(ipAddress);

        ui->userTableWidget->insertRow(0);
        ui->userTableWidget->setItem(0,0,user);
        ui->userTableWidget->setItem(0,1,host);
        ui->userTableWidget->setItem(0,2,ip);

        ui->messageBrowser->setTextColor(Qt::gray);
        ui->messageBrowser->setCurrentFont(QFont("Times New Roman",10));
        ui->messageBrowser->append(tr("%1 在线").arg(userName));
        ui->userNumLabel->setText(tr("在线人数： %1")
                                  .arg(ui->userTableWidget->rowCount()));
        //发送新用户登录消息
        sendMessage(NewParticipant);
    }
}

//成员离开处理函数
void charRoom::participantLeft(QString username, QString localHostName, QString time)
{
    int rowNum = ui->userTableWidget->findItems(localHostName,
                                                Qt::MatchExactly).first()->row();
    ui->userTableWidget->removeRow(rowNum);
    ui->messageBrowser->setTextColor(Qt::gray);
    ui->messageBrowser->setCurrentFont(QFont("Times New Roman",10));
    ui->messageBrowser->append(tr("%1 于 %2 离开！").arg(username).arg(time));
    ui->userNumLabel->setText(tr("在线人数： %1")
                              .arg(ui->userTableWidget->rowCount()));
}

//广播消息，type用于接收端区分消息类型
void charRoom::sendMessage(MessageType type, QString serverAddress)
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);
    QString localHostName = QHostInfo::localHostName();   //获取用户名、主机名
    QString address = getIP();
    out << type << getUserName() << localHostName;

    QString clientAddress;
    int row;

    //判断发送消息的类型
    switch(type)
    {
    case Message:
        //发送的是消息
        if(ui->messageTextEdit->toPlainText() == "")
        {
            QMessageBox::warning(0,tr("警告"),
                                 tr("发送内容不能为空"),QMessageBox::Ok);
            return;
        }
        out << address << getMessage();
        ui->messageBrowser->verticalScrollBar()
                ->setValue(ui->messageBrowser->verticalScrollBar()->maximum());
        break;

    case NewParticipant:
        //广播新用户加入的消息
        out << address;
        break;

    case ParticipantLeft:
        //广播用户离开的消息
        break;

    case FileName:
        //发送传输的文件名
        row = ui->userTableWidget->currentRow();
        clientAddress= ui->userTableWidget->item(row,2)->text();
        out << address << clientAddress << fileName;
        break;

    case Refuse:
        //发送拒绝接受文件消息
        out << serverAddress;
        break;
    }
    //进行udp广播
    udpSocket->writeDatagram(data,data.length(),QHostAddress::Broadcast,port);
}

//判断是否接收、发送文件
void charRoom::hasPendingFile(QString userName, QString serverAddress,
                            QString clientAddress, QString fileName)
{

    QString ipAddress = getIP();
    if(ipAddress == clientAddress)
    {
        int btn = QMessageBox::information(this,tr("接收文件"),
                                           tr("来自%1(%2)的文件：%3,是否接收?")
                                           .arg(userName).arg(serverAddress)
                                           .arg(fileName),QMessageBox::Yes,QMessageBox::No);
        if(btn == QMessageBox::Yes) {
            QString name = QFileDialog::getSaveFileName(0,tr("保存文件"),fileName);
            if(!name.isEmpty())
            {
                TcpClient *client = new TcpClient(this);
                client->setFileName(name);
                client->setHostAddress(QHostAddress(serverAddress));
                client->show();
            }
        }
        else {
            sendMessage(Refuse,serverAddress);
        }
    }
}

//保存接受的文件
bool charRoom::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::WriteOnly|QFile::Text)) {
        QMessageBox::warning(this,tr("保存文件"),
                             tr("无法保存文件 %1:\n %2").arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out << ui->messageBrowser->toPlainText();
    return true;
}


//关闭事件：在关闭程序时发送用户离开的广播，让其他端点在其用户列表中删除用户
void charRoom::closeEvent(QCloseEvent *event)
{
    sendMessage(ParticipantLeft);
    QWidget::closeEvent(event);
}

//获取IP地址
QString charRoom::getIP()
{
    QList<QHostAddress>list = QNetworkInterface::allAddresses();
    foreach(QHostAddress address,list) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return 0;
}

//获取用户名
QString charRoom::getUserName()
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    foreach(QString string,envVariables)
    {
        int index = environment.indexOf(QRegExp(string));
        if(index != -1) {
            QStringList stringList = environment.at(index).split('=');
            if(stringList.size() == 2)
            {
                return stringList.at(1);
                break;
            }
        }
    }
    return "unKnow";
}

//获取用户输入的消息并设置
QString charRoom::getMessage()
{
    QString msg = ui->messageTextEdit->toHtml();
    ui->messageTextEdit->clear();   //将文本编辑器内容清空
    ui->messageTextEdit->setFocus();
    return msg;
}

//处理接收到的广播消息
void charRoom::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());   //调整为接收到数据的大小
        udpSocket->readDatagram(datagram.data(),datagram.size());
        QDataStream in(&datagram,QIODevice::ReadOnly);
        int messageType;
        in >> messageType;
        QString clientAddress,fileName;
        QString userName,localHostName,ipAddress,message;
        QString time = QDateTime::currentDateTime()
                .toString("yyyy-MM-dd hh:mm:ss");
        switch(messageType)
        {
        //显示发送的消息
        case Message:
            in >> userName >> localHostName >> ipAddress >> message;
            ui->messageBrowser->setTextColor(Qt::blue);
            ui->messageBrowser->setCurrentFont(QFont("Times New Roman",12));
            ui->messageBrowser->append("[" + userName + "]" + time);
            ui->messageBrowser->append(message);
            break;

        //有新成员加入
        case NewParticipant:
            in >> userName >> localHostName >> ipAddress;
            newParticipant(userName,localHostName,ipAddress);  //加入处理
            break;

        //有成员离开
        case ParticipantLeft:
            in >> userName >> localHostName;
            participantLeft(userName,localHostName,time);  //进行离开处理

        //要发送的文件名
        case FileName:
            in >> userName >> localHostName >> ipAddress;
            in >> clientAddress >> fileName;
            hasPendingFile(userName,ipAddress,clientAddress,fileName);
            break;

        //拒绝接受的文件
        case Refuse:
            in >> userName >> localHostName;
            QString serverAddress;
            in >> serverAddress;
            QString ipAddress = getIP();
            if(ipAddress == serverAddress)
            {
                server->refused();
            }
            break;
        }
    }
}

//发送按钮--槽
void charRoom::on_sendButton_clicked()
{
    sendMessage(Message);
}

void charRoom::getFileName(QString name)
{
    fileName = name;
    sendMessage(FileName);   //upd广播要发送的文件名
}

//传输文件按钮--槽
void charRoom::on_sendToolBtn_clicked()
{
    if(ui->userTableWidget->selectedItems().isEmpty())
    {
        QMessageBox::warning(0,tr("选择用户"),
                             tr("请先从用户列表选择要传送的用户!"),QMessageBox::Ok);
        return;
    }
    server->show();
    server->initServer();
}

//更改字体大小
void charRoom::on_SizeComboBox_currentIndexChanged(const QString &arg1)
{
    ui->messageTextEdit->setFontPointSize(arg1.toDouble());
    ui->messageTextEdit->setFocus();
}

//加粗
void charRoom::on_boldToolBtn_clicked(bool checked)
{
    if(checked)
    {
        ui->messageTextEdit->setFontWeight(QFont::Bold);
    }
    else
    {
        ui->messageTextEdit->setFontWeight(QFont::Normal);
    }
    ui->messageTextEdit->setFocus();
}


//斜体
void charRoom::on_italicToolBtn_clicked(bool checked)
{
    ui->messageTextEdit->setFontItalic(checked);
    ui->messageTextEdit->setFocus();
}

//下划线
void charRoom::on_underlineToolBtn_clicked(bool checked)
{
    ui->messageTextEdit->setFontUnderline(checked);
    ui->messageTextEdit->setFocus();
}

//字体颜色
void charRoom::on_colorToolBtn_clicked()
{
    color = QColorDialog::getColor(color,this);
    if(color.isValid()) {
        ui->messageTextEdit->setTextColor(color);
        ui->messageTextEdit->setFocus();
    }
}

//设置字体时可切换到相应状态
void charRoom::currentFormatChanged(const QTextCharFormat &format)
{
    ui->fontComboBox->setCurrentFont(format.font());
    if(format.fontPointSize() < 9) {
        ui->SizeComboBox->setCurrentIndex(3);
    }
    else {
        ui->SizeComboBox->setCurrentIndex(ui->SizeComboBox
                                          ->findText(QString::number(format.fontPointSize())));
    }
    ui->boldToolBtn->setChecked(format.font().bold());
    ui->italicToolBtn->setChecked(format.font().italic());
    ui->underlineToolBtn->setChecked(format.font().underline());
    color = format.foreground().color();
}

//保存聊天记录
void charRoom::on_saveToolBtn_clicked()
{
    if(ui->messageBrowser->document()->isEmpty()) {
        QMessageBox::warning(0,tr("警告"),
                             tr("聊天记录为空,无法保存!"),QMessageBox::Ok);
    }else {
        QString fileName = QFileDialog::getSaveFileName(this,tr("保存聊天记录"),
                                                        tr("聊天记录"),tr("文本(*.txt);;All File(*.*)"));
        if(!fileName.isEmpty())
            saveFile(fileName);
    }
}

//清空聊天记录
void charRoom::on_clearToolBtn_clicked()
{
    ui->messageBrowser->clear();
}

//退出按钮
void charRoom::on_exitButton_clicked()
{
    close();
}

void charRoom::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->messageTextEdit->setCurrentFont(f);
    ui->messageTextEdit->setFocus();
}
