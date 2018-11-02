#ifndef KR_LOGIN_H
#define KR_LOGIN_H

#include <QWidget>
#include "kwindows.h"
#include "kr_mainwindow.h"

namespace Ui {
class kr_login;
}

class kr_login : public Kwindows
{
    Q_OBJECT

public:
    explicit kr_login(QWidget *parent = 0);
    ~kr_login();
signals:
    void signalSendAccount(QString *userAccount);
private slots:
    void openRegisterPage();
    //注册操作
    void registerAccount();
    void registerGiveUp();
    //登录操作
    void LoginAccount();
    //存储设置
    void saveSetting();
    //设置自动运行
    void autoRun(int checkstate);
    //取消登录操作
    void cancelLogin();
    //弹出主窗口
    void openMainWindow();
    //自动登录必须要保存密码,所以必须同步
    void synchronizeCheckBox(int checkstate);
    void on_FcomboBox_currentIndexChanged(int index);
public:
    //先生成对象
    void createMainwindow();
    kr_mainwindow *km;
private:
    Ui::kr_login *ui;
    void initTitleBar();
    void initAllsignalslot();
    void initWidgets();
    void initLocation();
    void paintEvent(QPaintEvent *event);
    void initMysql();
    //判断软件版本是否最新
    void judgeSoftwareVersion();
    //读取设置
    void loadSetting();
    //登录延时计时器
    QTimer *open;
};

#endif // KR_LOGIN_H
