#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "basewindow.h"

/*
 * 登录界面
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


private:
    Ui::login *ui;
    BaseWindow *titleBar;
};

#endif // LOGIN_H
