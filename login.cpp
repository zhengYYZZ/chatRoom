#include "login.h"
#include "ui_login.h"
#include <QPixmap>
#include <QPushButton>
#include <QPainterPath>
#include <QPainter>
#include <QString>
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



    //暗注释
    ui->nameLineEdit->setPlaceholderText(QStringLiteral(" 昵称"));
    ui->ipLineEdit->setPlaceholderText(tr(" IP地址"));
}

