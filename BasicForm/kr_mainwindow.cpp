#include "kr_mainwindow.h"
#include "ui_kr_mainwindow.h"
#include "kwindows.h"
#include "kr_listwidget.h"
#include "ToolForm//kr_welcome.h"
#include "ToolForm/kr_class.h"
#include "ScreenShot/fullscreen.h"
#include "kmessagebox.h"
#include <QPainter>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QBitmap>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QListWidget>
#include <QDebug>
#include <QPropertyAnimation>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>
#include <QProcess>
#include <QFile>
#define DAILY_GROWTH_EXP 5000
#define DAILY_GROWTH_MONEY 500
#define MIN_GROWTH_EXP 100
#define MIN_GROWTH_MONEY 10
std::vector<bool> isFullScreenWindow;     //用于存储所有的窗口
int defaultSize = 0; //正常情况下全屏的数量
//声明回调函数
bool CALLBACK AutoHideEnumWindowsProc(HWND hwnd,LPARAM lParam);

kr_mainwindow::kr_mainwindow(QWidget *parent) :
    Kwindows(parent),
    ui(new Ui::kr_mainwindow),
autoHideTimer(new QTimer(this)),
fullScreenAutoHideTimer(new QTimer(this)),
isHandMovementHide(false),
popMenuHasShow(false)
{
    ui->setupUi(this);
    //加载设置
    loadSetting();
    initTitleBar();
    initSystemTrayIcon();
}

//回调函数
bool CALLBACK AutoHideEnumWindowsProc(HWND hwnd,LPARAM lParam)
{
    if (::IsWindow(hwnd) && ::IsWindowVisible(hwnd) && GetParent(hwnd)==NULL)//GetParent(hwnd)==NULL不捕捉控件句柄
    {
        RECT tempRect;
        QRect tempQRect;
        ::GetWindowRect(hwnd,&tempRect);

        tempQRect.setTopLeft(QPoint(tempRect.left,tempRect.top));
        tempQRect.setBottomRight(QPoint(tempRect.right,tempRect.bottom));

        QRect desktop = QApplication::desktop()->screenGeometry();
        if(tempQRect.width()==desktop.width()+1&&tempQRect.height()==desktop.height()+1){
            isFullScreenWindow.push_back(true);
        }

        ::EnumChildWindows(hwnd,(WNDENUMPROC)AutoHideEnumWindowsProc,0);
    }
    return true;
}

//鼠标焦点进入出现动画
void kr_mainwindow::enterEvent(QEvent *)
{
    QRect rc;
    QRect rect;
    rect.setRect(this->x(),this->y(),this->width()-10,this->height()-10);
    rc.setRect(rect.x(),rect.y(),rect.width(),rect.height());
    if(rect.top()<0)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(150);
        animation->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
        QRect rcEnd;
        rcEnd =QRect(rc.x(),-12,this->width(),this->height());
        animation->setEndValue(rcEnd);
        animation->start();
        //自修复
        defaultSize = 0;
        scanFullScreenWindow();
    }
    //假如一开始焦点便在软件内则停止首开自动隐藏时钟
    if(autoHideTimer->isActive())
    {
        autoHideTimer->stop();
    }
}
//鼠标焦点离开出现动画
void kr_mainwindow::leaveEvent(QEvent *)
{
    //如果打开了右键菜单，则不让隐藏
    if(popMenuHasShow)
    {
        return;
    }
    QRect rc;
    QRect rect;
    rect.setRect(this->x(),this->y(),this->width()-10,this->height()-10);
    rc.setRect(rect.x(),rect.y(),rect.width(),rect.height());
    if(rect.top()<0)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(200);
        animation->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
        QRect rcEnd;
        rcEnd =QRect(rc.x(),-rc.height()+2,this->width(),this->height());
        animation->setEndValue(rcEnd);
        animation->start();
    }
    popMenuHasShow=false;//自动纠正卡主无法自动收缩的问题
}

kr_mainwindow::~kr_mainwindow()
{
    delete ui;
}

void kr_mainwindow::paintEvent(QPaintEvent *event)
{
    //绘制窗口白色背景色;
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRoundedRect(QRect(10, 10, this->width()-20, this->height()-20),8,8);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(255, 255, 255)));
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundedRect(QRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2),8,8);
        if(i<3)
        {color.setAlpha(100 - i*50);}
        painter.setPen(color);
        painter.drawPath(path);
    }
    return QWidget::paintEvent(event);
}

void kr_mainwindow::initTitleBar()
{
    m_titleBar->move(1, 2);
    m_titleBar->setBackgroundColor(255,255, 255);
    //m_titleBar->setTitleIcon(":/Resources/titleicon.png");
    m_titleBar->setTitleContent(QString("KTM"));
    m_titleBar->setButtonType(MIN_BUTTON);
    m_titleBar->setTitleWidth(this->width());
    m_titleBar->setWindowBorderWidth(2);
    m_titleBar->hide();
}


void kr_mainwindow::initFrame()
{
    //窗口配置
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow);
    QGuiApplication::setQuitOnLastWindowClosed(false);
    //移动至右上角
    QRect deskRect = QApplication::desktop()->availableGeometry();
    this->move(deskRect.right()-this->width()+10,-12);//位置
    //向stackwidget添加控件
    kr_listwidget *k=new kr_listwidget(this);
    //传递账号信息
    connect(this,SIGNAL(signalSendAccount(QString*)),k,SLOT(getAccount(QString*)));
    //list窗口传最小化请求
    connect(k,SIGNAL(signalSendMinimizeRequest()),this,SLOT(HandMovementHide()));
    emit signalSendAccount(account);
    ui->stackedWidget->insertWidget(0,k);
    ui->stackedWidget->setCurrentIndex(0);
    //自动隐藏时钟初始化
    autoHideTimer->start(1200);
    autoHideTimer->setSingleShot(true);
    connect(autoHideTimer,SIGNAL(timeout()),this,SLOT(autoHideOnFirst()));
    //全屏自动隐藏时钟初始化
    scanFullScreenWindow();
    fullScreenAutoHideTimer->start(5000);
    connect(fullScreenAutoHideTimer,SIGNAL(timeout()),this,SLOT(scanFullScreenWindow()));
    //初始化停止leaveevent操作的槽连接
    connect(k,SIGNAL(signalStopAutoHideListWidget()),this,SLOT(stopAutoHideListWidget()));
    //解决只有单独close时后台进程未清理及只有qApp->quit时析构函数无法执行的问题
    connect(qApp,SIGNAL(aboutToQuit()),this,SLOT(close()));
}

void kr_mainwindow::dailySignUp()
{
    bool todayHasLogined = true;//判断今天有没有登陆
    QSqlQuery *sql_query=new QSqlQuery();
    QString query;
    query=QString("CALL judgeTodayHaveLoginOrNot(\"%1\")").arg(*account);
    sql_query->exec(query);
    while(sql_query->next())
    {
        if(sql_query->value(0).toInt()>0)
            todayHasLogined = false;
        sql_query->prepare("SELECT Exp,Money FROM k_comuser WHERE Name = :Name");
        sql_query->bindValue(":Name",*account);
        sql_query->exec();
        while(sql_query->next())
        {
            myExp=sql_query->value(0).toInt();
            myMoney=sql_query->value(1).toInt();
        }
        if(!todayHasLogined){
            myExp+=DAILY_GROWTH_EXP;
            myMoney+=DAILY_GROWTH_MONEY;
            query=QString("UPDATE k_comuser SET Exp = :Exp,Money = :Money WHERE Name = :Name");
            sql_query->prepare(query);
            sql_query->bindValue(":Exp",myExp);
            sql_query->bindValue(":Money",myMoney);
            sql_query->bindValue(":Name",*account);
            sql_query->exec();
            Kmessagebox::showMyMessageBox(NULL,"每日首登,增加5000经验,500金钱","当前经验: "+QString("%1").arg(myExp)+"\n当前余额: "+QString("%1").arg(myMoney),MESSAGE_INFORMATION,BUTTON_OK,false);
        }
    }
    //更新上次登录时间，原在Login界面
    sql_query->prepare("UPDATE k_comuser SET LoginTime = :LoginTime WHERE Name = :Name");
    QString datetime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    sql_query->bindValue(":LoginTime",datetime);
    sql_query->bindValue(":Name",*account);
    sql_query->exec();
    //启动挂机增加经验金钱的计时器
    receiveTimer = new QTimer(this);
    receiveTimer->setInterval(60000);
    connect(receiveTimer,SIGNAL(timeout()),this,SLOT(receiveExpAndMoney()));
    receiveTimer->start();
}

void kr_mainwindow::initSystemTrayIcon()
{
    account = new QString("");

    QIcon icon = QIcon("://KTM.ico");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip(tr("KTM 任务管理桌面工具"));
    trayIcon->show();

    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this,SLOT(trayiconActivated(QSystemTrayIcon::ActivationReason)));
    //创建监听行为
    classAction = new QAction(tr("MyGDUT (&M)"), this);
    connect(classAction, SIGNAL(triggered()), this, SLOT(callClassWidget()));
    //最小化按钮
    minimizeAction = new QAction(tr("最小化 (&I)"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(HandMovementHide()));
    //退出按钮
    quitAction = new QAction(tr("退出 (&Q)"), this);
    connect(quitAction,SIGNAL(triggered()),qApp,SLOT(quit()));
    //注销按钮
    loginOutAction = new QAction(tr("注销 (&R)"), this);
    connect(loginOutAction,SIGNAL(triggered()),this,SLOT(loginOut()));
    //截图按钮
    grabAction = new QAction(tr("截图 (&CTRL+ALT+A)"), this);
    connect(grabAction,SIGNAL(triggered()),this,SLOT(Screenshot()));
    //创建右键弹出菜单
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(grabAction);
    trayIconMenu->addAction(classAction);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(loginOutAction);
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);
}

void kr_mainwindow::showWelcome()
{
    kr_welcome *w = new kr_welcome();
    connect(this,SIGNAL(signalSendAccount(QString*)),w,SLOT(getAccount(QString*)));
    emit signalSendAccount(account);
    w->show();
}

void kr_mainwindow::trayiconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        if(*account!=""){
            this->showNormal();
            this->raise();
            isHandMovementHide = false;
        }
        break;
    default:
        break;
    }
}

void kr_mainwindow::loginOut()
{
    qApp->quit();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}

void kr_mainwindow::getAccount(QString *userAccount)
{
    account=userAccount;
    showWelcome();
    //等待账号传递完成再进行下一轮传递
    initFrame();
    dailySignUp();//每日签到判断
}


//与失焦自动隐藏动画函数代码段相同
void kr_mainwindow::autoHideOnFirst()
{
    QRect rc;
    QRect rect;
    rect.setRect(this->x(),this->y(),this->width()-10,this->height()-10);
    rc.setRect(rect.x(),rect.y(),rect.width(),rect.height());
    if(rect.top()<0)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
        animation->setDuration(300);
        animation->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
        QRect rcEnd;
        rcEnd =QRect(rc.x(),-rc.height()+2,this->width(),this->height());
        animation->setEndValue(rcEnd);
        animation->start();
    }
}

void kr_mainwindow::stopAutoHideListWidget()
{
    popMenuHasShow=!popMenuHasShow;
    //在完成删除后，结束阻塞自动隐藏，并调用隐藏
    if(!popMenuHasShow)
    {
        autoHideOnFirst();
    }
}

void kr_mainwindow::callClassWidget()
{
    kr_class *kc = new kr_class();
    kc->show();
}

void kr_mainwindow::receiveExpAndMoney()
{
    QSqlQuery *sql_query=new QSqlQuery();
    QString query;
    myExp+=MIN_GROWTH_EXP;
    myMoney+=MIN_GROWTH_MONEY;
    query=QString("UPDATE k_comuser SET Exp = :Exp,Money = :Money WHERE Name = :Name");
    sql_query->prepare(query);
    sql_query->bindValue(":Exp",myExp);
    sql_query->bindValue(":Money",myMoney);
    sql_query->bindValue(":Name",*account);
    sql_query->exec();
}

void kr_mainwindow::scanFullScreenWindow()
{
    isFullScreenWindow.clear();
    ::EnumWindows((WNDENUMPROC)AutoHideEnumWindowsProc,0);
    if(defaultSize == 0){
        defaultSize = isFullScreenWindow.size();
    }else{
        defaultSize = isFullScreenWindow.size() < defaultSize ? isFullScreenWindow.size():defaultSize;
    }
    if(isFullScreenWindow.size()>defaultSize)
    {
        if(!isHandMovementHide){//手动隐藏
            this->hide();
        }
    }else{
        if(this->isHidden()&&*account!=""){
            if(!isHandMovementHide){
                this->showNormal();
                this->raise();
            }
        }
    }
}

void kr_mainwindow::Screenshot()
{
    this->hide();
    FullScreen *fc = new FullScreen();
    fc->show();
    this->show();
}

void kr_mainwindow::HandMovementHide()
{
    this->hide();
    isHandMovementHide = true;
}

void kr_mainwindow::loadSetting()
{
    QString filepath=QCoreApplication::applicationDirPath();
    filepath+="/setting.ini";
    filepath.replace("/","\\");
    QSettings setting(filepath,QSettings::IniFormat);
    setting.beginGroup("config");
    setWindowOpacity(setting.value("Opactiy").toFloat()/100);
    setting.endGroup();
}
