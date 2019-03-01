#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QString>
#include "basewindow.h"
#include "chatroom.h"

/*
 * 登录界面--继承自baseWindow类
 * 1）控件图标
 * 2）自定义状态栏
 * 3）头像、背景
 * 4）窗口固定大小
 * 功能 ---获取本地ip
 */

namespace Ui {
class login;
}


class login : public BaseWindow
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

   //初始化标题栏
    void initTitleBar();
    //初始化控件
    void initControl();
    //获取ip地址
    QString getIP();


private slots:
    void on_createPushButton_clicked();

private:
    Ui::login *ui;
    BaseWindow *titleBar;
    User users;
    chatRoom *chat;
};

#endif // LOGIN_H
