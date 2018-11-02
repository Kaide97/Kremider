#include "kr_login.h"
#include "ui_kr_login.h"
#include "kwindows.h"
#include "kmessagebox.h"
#include "kr_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSettings>
#include <QDir>
#include <QTimer>
#include <QRegExp>
#include <QValidator>
#include <QTime>
#include <QMovie>
#include <QProcess>
#include <QDesktopServices>
#define VERSION 655//软件版本
kr_login::kr_login(QWidget *parent) :
    Kwindows(parent),
    ui(new Ui::kr_login)
{
    ui->setupUi(this);
    initTitleBar();
    initMysql();
    initAllsignalslot();
    initWidgets();
    initLocation();
    //判断版本
    judgeSoftwareVersion();
    //存储设置在登录按钮的槽函数中
    loadSetting();
    //子窗口都需要,解决长时间使用内存不释放问题
    setAttribute(Qt::WA_DeleteOnClose, true);
}

kr_login::~kr_login()
{
    delete ui;
}

//初始化状态栏
void kr_login::initTitleBar()
{
    m_titleBar->hide();
}
//初始化信号槽
void kr_login::initAllsignalslot()
{
    //主页面切换注册按钮
    connect(ui->RegisterBtn,SIGNAL(clicked()),this,SLOT(openRegisterPage()));
    //注册按钮
    connect(ui->R_RegisterBtn,SIGNAL(clicked()),this,SLOT(registerAccount()));
    //注册页面返回按钮
    connect(ui->R_RegisterGVBtn,SIGNAL(clicked()),this,SLOT(registerGiveUp()));
    //登录按钮
    connect(ui->LoginBtn,SIGNAL(clicked()),this,SLOT(LoginAccount()));
    //开机自启选择
    connect(ui->AutonRunCheckBox,SIGNAL(stateChanged(int)),this,SLOT(autoRun(int)));
    //选择BOX同步
    connect(ui->AutnLoginCheckBox,SIGNAL(stateChanged(int)),this,SLOT(synchronizeCheckBox(int)));
    //关闭按钮
    connect(ui->CloseBtn,SIGNAL(clicked()),this,SLOT(close()));
}

void kr_login::initWidgets()
{
    //设置随机开始界面图
    srand(QTime(0,0,0).secsTo(QTime::currentTime()));
    ui->PitLabel->setStyleSheet(QString("border-image: url(:/Resources/Login/label/%1.png);").arg(rand()%6));
    //限制密码输入格式
    QRegExp regx("[0-9A-Za-z_,.-]+$");
    QRegExpValidator *validator = new QRegExpValidator(regx);
    ui->R_PasswordLineEdit->setValidator(validator);
    ui->RA_PasswordLineEdit->setValidator(validator);
    ui->PasswordLineEdit->setValidator(validator);
    ui->R_AccountLineEdit->setValidator(validator);
    //登录中动图
    QMovie *movie = new QMovie();
    movie->setFileName("://Resources/Login/label/Logining.gif");
    ui->LoginingLabel->setMovie(movie);
    movie->start();
    setAttribute(Qt::WA_TranslucentBackground);
}

void kr_login::initLocation()
{
    QSqlQuery *sql_query=new QSqlQuery();
    sql_query->exec("SELECT LocationName FROM k_location WHERE FatherID = 0");
    while (sql_query->next()) {
        ui->FcomboBox->addItem(sql_query->value(0).toString());
    }
}
//初始化数据库
void kr_login::initMysql()
{
    QSqlDatabase db=QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("Kmission");
    db.setPort(3306);
    if(!db.open())
    {
        int ch=Kmessagebox::showMyMessageBox(this,QString(tr("登录失败")),QString(tr("网络连接异常,是否重连")),MESSAGE_WARNNING,BUTTON_AUTO_OK_AND_CANEL,true);
        if(ch==0){initMysql();}
        else{
            this->close();//清理释放
            qApp->quit();//关闭程序
        }
    }else{
        qDebug()<<"Connect Success";
    }
}

void kr_login::judgeSoftwareVersion()
{
    int result;
    QSqlQuery *sql_query=new QSqlQuery();
    sql_query->exec("SELECT * FROM k_softwareInfo ORDER BY UpdateTime DESC LIMIT 1");
    while(sql_query->next())
    {
        if(sql_query->value(0).toInt()>VERSION){
            result = Kmessagebox::showMyMessageBox(this,"有新版本，是否更新","最新版本："+sql_query->value(0).toString()+"\t更新时间："+sql_query->value(1).toString().replace("T"," ")+"\n当前版本："+QString("%1").arg(VERSION),MESSAGE_INFORMATION,BUTTON_OK_AND_CANCEL,true);
            if(result==0){
                QProcess::startDetached(qApp->applicationDirPath()+"/KTMAutoUpdate.exe", QStringList());
                this->close();
                qApp->quit();
            }
        }
    }
}
//注册操作
void kr_login::registerAccount()
{
    if(ui->R_AccountLineEdit->text().size()<=20&&ui->R_PasswordLineEdit->text().size()>=6&&ui->R_PasswordLineEdit->text().size()<=20)
    {
        QSqlQuery *sql_query=new QSqlQuery();
        sql_query->prepare("SELECT Name FROM k_comuser WHERE Name = :Name");
        sql_query->bindValue(":Name",ui->R_AccountLineEdit->text());
        sql_query->exec();
        while(sql_query->next())
        {
            Kmessagebox::showMyMessageBox(this,"KTM","注册失败,账号已被注册",MESSAGE_WARNNING,BUTTON_OK,true);
            return;
        }
        if(ui->R_PasswordLineEdit->text()==ui->RA_PasswordLineEdit->text())
        {
            sql_query->prepare("INSERT INTO k_comuser (Name,Password,LocationID,Exp,Money) VALUES (:Name,:Password,:LocationID,0,0)");
            sql_query->bindValue(":Name",ui->R_AccountLineEdit->text().toUtf8());
            sql_query->bindValue(":Password",ui->R_PasswordLineEdit->text().toUtf8());
            sql_query->bindValue(":LocationID",ui->ScomboBox->currentData().toInt());
            sql_query->exec();
            Kmessagebox::showMyMessageBox(this,"KTM","注册成功,赶快加入使用吧!",MESSAGE_INFORMATION,BUTTON_OK,true);
            ui->stackedWidget->setCurrentIndex(0);
            ui->AccountLineEdit->setText(ui->R_AccountLineEdit->text());
            ui->PasswordLineEdit->setText(ui->R_PasswordLineEdit->text());
        }else{
            Kmessagebox::showMyMessageBox(this,"KTM","注册失败,两次密码输入不同喔!",MESSAGE_WARNNING,BUTTON_OK,true);
            ui->R_PasswordLineEdit->clear();
            ui->RA_PasswordLineEdit->clear();
            ui->R_PasswordLineEdit->setFocus();
        }
    }else{
        Kmessagebox::showMyMessageBox(this,"注册失败","账号或密码的格式或长度不满足条件",MESSAGE_WARNNING,BUTTON_OK,true);
    }
}

void kr_login::registerGiveUp()
{
    //放弃注册返回
    ui->stackedWidget->setCurrentIndex(0);
    //初始化页面
    ui->R_AccountLineEdit->clear();
    ui->R_AccountLineEdit->setFocus();
    ui->R_PasswordLineEdit->clear();
    ui->RA_PasswordLineEdit->clear();
}
//打开注册页面
void kr_login::openRegisterPage()
{
    ui->stackedWidget->setCurrentIndex(2);
}

//登录操作（自动登录同样跳转）
void kr_login::LoginAccount()
{
    if(!(ui->AccountLineEdit->text().isEmpty())&&!(ui->PasswordLineEdit->text().isEmpty()))
    {
        //登录时再保存设置
        saveSetting();
        QSqlQuery *sql_query=new QSqlQuery();
        sql_query->prepare("SELECT Name,Password FROM k_comuser WHERE Name = :Name");
        sql_query->bindValue(":Name",ui->AccountLineEdit->text().toUtf8());
        sql_query->exec();
        bool isEmpty=true;//判断是否有这个账号
        while(sql_query->next())
        {
            isEmpty=false;
            if(sql_query->value(0).toString()==ui->AccountLineEdit->text()&&sql_query->value(1).toString()==ui->PasswordLineEdit->text())
            {
                //模仿QQ登录,进入延时3秒的登录等待
                //自动登录延迟
                open=new QTimer();
                ui->stackedWidget->setCurrentIndex(1);
                //关闭按钮隐藏
                ui->CloseBtn->hide();
                open->start(2200);
//              //更新上次登录时间                //改到判断Mainwindow判断首登那里去
//              sql_query->prepare("UPDATE User SET LoginTime = :LoginTime WHERE Name = :Name");
//              QString datetime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
//              sql_query->bindValue(":LoginTime",datetime);
//              sql_query->bindValue(":Name",ui->AccountLineEdit->text().toUtf8());
//              sql_query->exec();
                //连接延时结束信号槽
                connect(open,SIGNAL(timeout()),this,SLOT(openMainWindow()));
                //连接取消登录信号槽
                connect(ui->CancelBtn,SIGNAL(clicked()),this,SLOT(cancelLogin()));
            }else{
                Kmessagebox::showMyMessageBox(this,"登录失败","账号或密码错误",MESSAGE_WARNNING,BUTTON_OK,true);
                ui->PasswordLineEdit->clear();
                ui->PasswordLineEdit->setFocus();
            }
        }if(isEmpty==true){
            Kmessagebox::showMyMessageBox(this,"登录失败","账号不存在",MESSAGE_WARNNING,BUTTON_OK,true);
        }
    }else{
        Kmessagebox::showMyMessageBox(this,"登录失败","账号或密码为空",MESSAGE_WARNNING,BUTTON_OK,this);
    }
}
//中断登录
void kr_login::cancelLogin()
{
    open->stop();
    ui->stackedWidget->setCurrentIndex(0);
    //关闭按钮出现
    ui->CloseBtn->show();
}
//登录成功打开主任务窗口
void kr_login::openMainWindow()
{
    if(km!=NULL){
        //传递账号信息
        connect(this,SIGNAL(signalSendAccount(QString*)),km,SLOT(getAccount(QString*)));
        emit signalSendAccount(new QString(ui->AccountLineEdit->text()));
        km->show();
        this->close();
    }
}
//同步checkBox
void kr_login::synchronizeCheckBox(int checkstate)
{
    if(checkstate==2)
    {
        ui->SaveCheckBox->setChecked(true);
    }
}
//保存设置
void kr_login::saveSetting()
{
    QString filepath=QCoreApplication::applicationDirPath();
    filepath+="/setting.ini";
    filepath.replace("/","\\");
    QSettings setting(filepath,QSettings::IniFormat);
    setting.beginGroup("config");
    //假如选择保存密码，则进入
    if(ui->SaveCheckBox->isChecked())
    {
        setting.setValue("SaveAccountPassword","true");
        setting.setValue("Account",ui->AccountLineEdit->text());
        setting.setValue("Password",ui->PasswordLineEdit->text());
        if(ui->AutnLoginCheckBox->isChecked())
        {
            setting.setValue("AutoLogin","true");
        }else{
            setting.setValue("AutoLogin","false");
        }
    }else{
        setting.setValue("SaveAccountPassword","false");
        setting.setValue("Account","");
        setting.setValue("Password","");
        setting.setValue("AutoLogin","false");
    }
    //开机自启设置
    if(ui->AutonRunCheckBox->isChecked())
    {
        setting.setValue("AutoRun","true");
    }else{
        setting.setValue("AutoRun","false");
    }
    if(setting.value("FootballClub").toString()=="")
    {
        setting.setValue("FootballClub","巴塞罗那");//球队选项
    }
    if(setting.value("ListItemColor").toString()=="")
    {
        setting.setValue("ListItemColor","Sapphire");//颜色选项
    }if(setting.value("BackgroundPicture").toString()=="")
    {
        setting.setValue("BackgroundPicture","3");//背景图片选项
    }if(setting.value("Font").toString()==""){
        setting.setValue("Font","方正正纤黑简体");//背景图片选项
    }

    setting.endGroup();
}
//读取设置
void kr_login::loadSetting()
{
    QString filepath=QCoreApplication::applicationDirPath();
    filepath+="/setting.ini";
    filepath.replace("/","\\");
    QSettings setting(filepath,QSettings::IniFormat);
    setting.beginGroup("config");
    if(setting.value("SaveAccountPassword").toString()=="true")
    {
        ui->AccountLineEdit->setText(setting.value("Account").toString());
        ui->PasswordLineEdit->setText(setting.value("Password").toString());
        ui->SaveCheckBox->setCheckState(Qt::CheckState::Checked);
        if(setting.value("AutoLogin").toString()=="true")
        {
            ui->AutnLoginCheckBox->setChecked(true);
            LoginAccount();
        }
    }
    if(setting.value("AutoRun").toString()=="true")
    {
        ui->AutonRunCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    //判断透明度
    if(setting.value("Opactiy").toString().isEmpty())
    {
        setting.setValue("Opactiy","98");
    }
    setting.endGroup();
}

//开机自动运行设置
void kr_login::autoRun(int checkstate)
{
    if(checkstate==2)
    {
        QSettings  reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
        QString strAppPath=QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        reg.setValue("DesktopTaskManager",strAppPath);
    }else if(checkstate==0){
        QSettings  reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);
        reg.remove("DesktopTaskManager");
    }
}

void kr_login::paintEvent(QPaintEvent *event)
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
        if(i<4)
        {color.setAlpha(120 - i*40);}
        painter.setPen(color);
        painter.drawPath(path);
    }

    return QWidget::paintEvent(event);
}

void kr_login::on_FcomboBox_currentIndexChanged(int index)
{
    ui->ScomboBox->clear();
    QSqlQuery *sql_query=new QSqlQuery();
    sql_query->prepare("SELECT LocationName,LocationID FROM k_location WHERE FatherID = :FatherID");
    sql_query->bindValue(":FatherID",index+1);
    sql_query->exec();
    while(sql_query->next())
    {
        ui->ScomboBox->addItem(sql_query->value(0).toString());
        ui->ScomboBox->setItemData(ui->ScomboBox->count()-1,sql_query->value(1));
    }
}

void kr_login::createMainwindow()
{
    km = new kr_mainwindow();
}
