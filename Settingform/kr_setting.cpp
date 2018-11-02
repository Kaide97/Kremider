#include "kr_setting.h"
#include "ui_kr_setting.h"
#include "BasicForm/kwindows.h"
#include "BasicForm/kmessagebox.h"
#include <QKeyEvent>
#include <QPainter>
#include <QSettings>
#include <QProcess>
#include <QSqlQuery>
#include <ToolForm/kr_translate.h>
#include <Settingform/kr_about.h>
#include <BasicForm/kr_login.h>
#include <QFileDialog>
#define VERSION 655//软件版本
kr_setting::kr_setting(QWidget *parent) :
    Kwindows(parent),
    ui(new Ui::kr_setting)
{
    ui->setupUi(this);
    initAllSignalSlot();
    initTitleBar();
    initTeamNameComboBox();
    loadSetting();
    //子窗口都需要,解决长时间使用内存不释放问题
    setAttribute(Qt::WA_DeleteOnClose, true);
}

kr_setting::~kr_setting()
{
    delete ui;
}


void kr_setting::paintEvent(QPaintEvent *event)
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

void kr_setting::initAllSignalSlot()
{
    //同步滑块显示
    connect(ui->OpSlider,SIGNAL(valueChanged(int)),this,SLOT(seeSliderValue(int)));
    //控件函数的绑定
    connect(ui->SaveBtn,SIGNAL(clicked()),this,SLOT(saveSetting()));
    connect(ui->ExitBtn,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->ResetBtn,SIGNAL(clicked()),this,SLOT(resetSetting()));
    connect(ui->UpdateBtn,SIGNAL(clicked()),this,SLOT(callUpdate()));
    connect(ui->AboutBtn,SIGNAL(clicked()),this,SLOT(callAbout()));
    //解决只有单独close时后台进程未清理及只有qApp->quit时析构函数无法执行的问题
    connect(qApp,SIGNAL(aboutToQuit()),this,SLOT(close()));
}

//保存设置
void kr_setting::saveSetting()
{
    //改变了透明度才要重启
    if(lastOpactiy!=ui->OpSlider->value())
    {
        QString filepath=QCoreApplication::applicationDirPath();
        filepath+="/setting.ini";
        filepath.replace("/","\\");
        QSettings setting(filepath,QSettings::IniFormat);
        setting.beginGroup("config");
        setting.setValue("Opactiy",ui->OpSlider->value());
        setting.endGroup();
        int ch = Kmessagebox::showMyMessageBox(this,"提示","透明度改变生效需要重启应用,是否重启",MESSAGE_QUESTION,BUTTON_OK_AND_CANCEL,true);
        if(ch==0)
        {
            //程序重启操作
            qApp->quit();
            QProcess::startDetached(qApp->applicationFilePath(), QStringList());
        }else{
            //程序退出
            qApp->quit();
        }
    }else{
        QString filepath=QCoreApplication::applicationDirPath();
        filepath+="/setting.ini";
        filepath.replace("/","\\");
        QSettings setting(filepath,QSettings::IniFormat);
        setting.beginGroup("config");
        setting.setValue("FootBallClub",ui->TeamcomboBox->currentText());
        setting.setValue("ListItemColor",ui->ColorcomboBox->currentText());
        setting.setValue("BackgroundPicture",ui->BackgroundcomboBox->currentIndex());
        setting.setValue("Font",ui->fontComboBox->currentText());
        setting.endGroup();
        this->close();
    }
}
//读取设置
void kr_setting::loadSetting()
{
    QString filepath=QCoreApplication::applicationDirPath();
    filepath+="/setting.ini";
    filepath.replace("/","\\");
    QSettings setting(filepath,QSettings::IniFormat);
    setting.beginGroup("config");
    ui->OpSlider->setValue(setting.value("Opactiy").toInt());
    ui->TeamcomboBox->setCurrentText(setting.value("FootBallClub").toString());
    ui->ColorcomboBox->setCurrentText(setting.value("ListItemColor").toString());
    ui->BackgroundcomboBox->setCurrentText(ui->BackgroundcomboBox->itemText(setting.value("BackgroundPicture").toInt()));
    ui->fontComboBox->setCurrentText(setting.value("Font").toString());
    lastOpactiy=setting.value("Opactiy").toInt();
    setting.endGroup();
}

void kr_setting::seeSliderValue(int value)
{
    ui->OpStateLabel->setText(QString("不透明度:%1%").arg(value));
}

void kr_setting::resetSetting()
{
    ui->OpSlider->setValue(98);
}

void kr_setting::callUpdate()
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
        }else{
            Kmessagebox::showMyMessageBox(this,"更新查询","版本已经是最新,无需更新",MESSAGE_INFORMATION,BUTTON_OK,true);
        }
    }
}

void kr_setting::callAbout()
{
    kr_about *a=new kr_about();
    a->show();
}

void kr_setting::initTitleBar()
{
    m_titleBar->hide();
}

void kr_setting::initTeamNameComboBox()
{
    QSqlQuery *sql_query=new QSqlQuery();
    sql_query->prepare("SELECT TeamName FROM k_footballTeamInfo");
    sql_query->exec();
    while(sql_query->next())
    {
        ui->TeamcomboBox->addItem(sql_query->value(0).toString());
    }
}

void kr_setting::on_Back2LoginBtn_clicked()
{
    qApp->quit();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}
