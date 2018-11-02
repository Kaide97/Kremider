#ifndef KR_MAINWINDOW_H
#define KR_MAINWINDOW_H

#include <QWidget>
#include "kwindows.h"
#include <QSystemTrayIcon>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QSqlDatabase>
namespace Ui {
class kr_mainwindow;
}

class kr_mainwindow : public Kwindows
{
    Q_OBJECT

public:
    explicit kr_mainwindow(QWidget *parent = 0);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    //自动隐藏功能暂关判定
    bool popMenuHasShow;
    //加载设置
    void loadSetting();
    ~kr_mainwindow();
signals:
    void signalSendAccount(QString *userAccount);
private slots:
    //托盘双击单击槽函数
    void trayiconActivated(QSystemTrayIcon::ActivationReason reason);//托盘创建槽函数
    //注销操作函数
    void loginOut();
    //得到用户信息 虚函数重载
    void getAccount(QString *userAccount);
    //打开软件第一次自动收缩
    void autoHideOnFirst();
    //接受来自listwidget信息，停止leaveEvent自动隐藏
    void stopAutoHideListWidget();
    //显示课表页面
    void callClassWidget();
    //定时增加经验金钱
    void receiveExpAndMoney();
    //自动判断有没有其他窗口全屏
    void scanFullScreenWindow();
public slots:
    //截图
    void Screenshot();
    //最小化
    void HandMovementHide();
private:
    Ui::kr_mainwindow *ui;
    void initTitleBar();
    //初始化程序框架
    void initFrame();
    //每日首登签到
    QTimer *receiveTimer;
    void dailySignUp();
    int myExp,myMoney;
    //画图事件
    void paintEvent(QPaintEvent *event);
    //初始化系统托盘
    void initSystemTrayIcon();
    //系统托盘
    QSystemTrayIcon *trayIcon;
    QAction *classAction;
    QAction *minimizeAction;
    QAction *loginOutAction;
    QAction *quitAction;
    QAction *grabAction;
    QMenu   *trayIconMenu;
    //显示欢迎界面
    void showWelcome();
    //自动隐藏时钟
    QTimer *autoHideTimer;
    //检测到有全屏窗口自动隐藏时钟
    QTimer *fullScreenAutoHideTimer;
    bool isHandMovementHide;
};

#endif // KR_MAINWINDOW_H
