#include "login.h"
#include "ui_login.h"
#include <QPixmap>
#include <QPushButton>
#include <QPainterPath>
#include <QPainter>
#include <QString>
#include <QNetworkInterface>
#include<QDebug>
#include<QMessageBox>

login::login(QWidget *parent) :
    BaseWindow(parent),
    ui(new Ui::login)
{
    titleBar = new BaseWindow;
    ui->setupUi(this);


    initTitleBar();
    initControl();

}

login::~login()
{
    delete ui;
}

#if 1
void login::initTitleBar()
{
    // 设置标题栏跑马灯效果，可以不设置;
    //m_titleBar->setTitleRoll();

    m_titleBar->move(0,0);
    m_titleBar->raise();
    m_titleBar->setBackgroundColor(0,0,0,true);
    m_titleBar->setButtonType(MIN_MAX_BUTTON);
    m_titleBar->setTitleWidth(this->width());

}
#endif

void login::initControl()
{
#if 0
    //设置上半角背景图片
    QPixmap backicon(tr("bargroud.jpg"));
    backicon = backicon.scaled(ui->backgroundLabel->width(),ui->backgroundLabel->height());
    ui->backgroundLabel->setPixmap(backicon);

    //设置头像
    int wid = ui->headPortraitLabel->width();
    int hei = ui->headPortraitLabel->height();
    QPixmap headicon("head.jpg");
    /*-----设置圆形头像-----
    QPixmap pixmap(wid,hei);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath path;
    path.addEllipse(0, 0, wid, hei);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, wid, hei, headicon);    */

    headicon = headicon.scaled(wid,hei);
    ui->headPortraitLabel->setPixmap(headicon);
#endif



    //暗注释
    ui->nameLineEdit->setPlaceholderText(QStringLiteral(" 昵称"));
    ui->ipLineEdit->setPlaceholderText(tr(" IP地址"));

    ui->ipLineEdit->setText(getIP());   //将ip显示到地址栏上

}

QString login::getIP()
{
    QList<QHostAddress>list = QNetworkInterface::allAddresses();
    foreach(QHostAddress address,list) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            return address.toString();
    }
    return 0;
}


void login::on_createPushButton_clicked()
{
    users.name = ui->nameLineEdit->text();
    if(ui->nameLineEdit->text() == "")
    {
        QMessageBox::warning(0,tr("警告"),
                             tr("发送内容不能为空"),QMessageBox::Ok);
        return;
    }
    users.hostaddress = getIP();
    users.address = getIP();
    //qDebug() << users.address << users.name << users.hostaddress;
    chat = new chatRoom(0,users);
    //chat->setUser(users);
    chat->show();

    this->hide();
}


