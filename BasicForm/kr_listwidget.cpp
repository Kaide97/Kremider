#include "kr_listwidget.h"
#include "ui_kr_listwidget.h"
#include "kr_mainwindow.h"
#include "TaskForm//kr_addtask.h"
#include "ToolForm//kr_app.h"
#include "kmessagebox.h"
#include "settingform//kr_setting.h"
#include "TaskForm//kr_trashstation.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QPropertyAnimation>
#include <QDateTime>
#include <QTime>
#include <QSound>
#include <QMenu>
#include <QProcess>
#include <QSettings>
#include <QClipboard>
#include <QFileDialog>
#include <QScrollBar>
#define CHANGE_BACKGROUND_COST 5000
kr_listwidget::kr_listwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::kr_listwidget),
vl(new QVBoxLayout(this)),closeTimerText(new QStringList())
{
    ui->setupUi(this);
    this->setParent(parent);
    initAllSignalSlot();
    initWidgets();
}

kr_listwidget::~kr_listwidget()
{
    delete system_time;delete alarm_time;
    delete ui;
}
//调用新增界面
void kr_listwidget::callAddWidget()
{
    kr_addTask *kr=new kr_addTask();//新建事务详情窗口
    kr->isEdit=false;//使窗口显示模式为新增状态
    connect(this,SIGNAL(signalSendAccount(QString*)),kr,SLOT(getAccount(QString*)));//用于传递账号以用于编辑后同步到云
    connect(kr,SIGNAL(destroyed(QObject*)),this,SLOT(refreshListWidget()));//编辑完成后回调信号通知主窗口刷新事务列表
    emit signalSendAccount(userID);//发射传递账号信号
    kr->show();//新建窗口显示
}
//调用编辑界面
void kr_listwidget::callEditWidget(QListWidgetItem *c_item)//传入待编辑的item
{
    kr_addTask *kr=new kr_addTask();//新建事务详情窗口
    kr->isEdit=true;//使窗口显示模式为编辑状态
    connect(this,SIGNAL(signalSendAccount(QString*)),kr,SLOT(getAccount(QString*)));//用于传递账号以用于编辑后同步到云
    connect(kr,SIGNAL(destroyed(QObject*)),this,SLOT(refreshListWidget()));//编辑完成后回调信号通知主窗口刷新事务列表
    connect(this,SIGNAL(signalSendTaskInfo(QString)),kr,SLOT(getTaskID(QString)));//发送需要修改的任务ID
    emit signalSendAccount(userID);//发射传递账号信号
    //定位当前选中item的位置以获取任务ID
    int curIndex = ui->listWidget->row(c_item);
    int notDeletedCount = 0;
    int pos;//存储定位所要删除任务的位置
    for(pos = 0;pos<taskState.count()-1;pos++)
    {
        if(taskState.at(pos)=="0")
            notDeletedCount++;
        if(notDeletedCount==curIndex+1)
            break;
    }
    emit signalSendTaskInfo(QString(taskID.at(pos)));//传递任务编号
    kr->show();//编辑窗口显示
}

void kr_listwidget::callAppWidget()
{
    kr_app *ap=new kr_app();
    connect(this,SIGNAL(signalSendAccount(QString*)),ap,SLOT(getAccount(QString*)));
    emit signalSendAccount(account);//传账号名可扩展性较大
    ap->show();
}

void kr_listwidget::callSettingWidget()
{
    kr_setting *s=new kr_setting();
    connect(this,SIGNAL(signalSendAccount(QString*)),s,SLOT(getAccount(QString*)));
    emit signalSendAccount(userID);
    s->show();
    connect(s,SIGNAL(destroyed(QObject*)),this,SLOT(sendSettingRequest()));
}

void kr_listwidget::callTrashStationWidget()
{
    kr_TrashStation *t =new kr_TrashStation();
    connect(t,SIGNAL(signalRefreshListWidget()),this,SLOT(refreshListWidget()));
    connect(this,SIGNAL(signalSendAccount(QString*)),t,SLOT(getAccount(QString*)));
    emit signalSendAccount(userID);
    t->show();
}

void kr_listwidget::sendSettingRequest()
{
    refreshListWidget();//加载那些元素设置
    kr_mainwindow *k=(kr_mainwindow*)parentWidget();
    k->loadSetting();
}

void kr_listwidget::seeLvInfo()
{
    judgeSqlConnectionState();
    QSqlQuery *sql_query=new QSqlQuery();
    QString query;
    query=QString("CALL getAccountInfo(\"%1\")").arg(*account);
    sql_query->exec(query);
    while(sql_query->next())
    {
        Kmessagebox::showMyMessageBox(this,"等级信息","等级: "+sql_query->value(1).toString()+"\t|  余额: "+sql_query->value(2).toString()+"\n当前经验: "+sql_query->value(0).toString()+"| 距离升级经验: "+sql_query->value(3).toString(),MESSAGE_INFORMATION,BUTTON_OK,true);
    }
}

void kr_listwidget::updateLvButton()
{
    judgeSqlConnectionState();
    QSqlQuery *sql_query=new QSqlQuery();
    QString query;
    query=QString("SELECT MAX(l.Level) FROM k_comuser AS u,k_level AS l WHERE u.Name = :Name AND u.Exp>l.Exp");
    sql_query->prepare(query);
    sql_query->bindValue(":Name",*account);
    sql_query->exec();
    while(sql_query->next())
    {
        level=sql_query->value(0).toInt();
    }
    ui->LvBtn->setText(QString("Lv.%1").arg(level));
    QString btnss;
    if(level<20)
    {
        btnss="QPushButton[Lv=true]{color: rgb(0,0,0);background-color:rgb(255,255,255);border: 0px solid rgb(100,100,100);border-radius:10px;}QPushButton:pressed[Lv=true]{padding-left:3px;padding-top:3px;font: 9pt \"Arial\";}";
    }else if(level>=20&&level<40)
    {
        btnss="QPushButton[Lv=true]{color: rgb(34,172,56);background-color:rgb(255,255,255);border: 0px solid rgb(100,100,100);border-radius:10px;}QPushButton:pressed[Lv=true]{padding-left:3px;padding-top:3px;font: 9pt \"Arial\";}";
    }else if(level>=40&&level<60)
    {
        btnss="QPushButton[Lv=true]{color: rgb(62,121,223);background-color:rgb(255,255,255);border: 0px solid rgb(100,100,100);border-radius:10px;}QPushButton:pressed[Lv=true]{padding-left:3px;padding-top:3px;font: 9pt \"Arial\";}";
    }else if(level>=60&&level<80)
    {
        btnss="QPushButton[Lv=true]{color: rgb(174,93,161);background-color:rgb(255,255,255);border: 0px solid rgb(100,100,100);border-radius:10px;}QPushButton:pressed[Lv=true]{padding-left:3px;padding-top:3px;font: 9pt \"Arial\";}";
    }else{
        btnss="QPushButton[Lv=true]{color: rgb(172, 118, 11);background-color:rgb(255,255,255);border: 0px solid rgb(100,100,100);border-radius:10px;}QPushButton:pressed[Lv=true]{padding-left:3px;padding-top:3px;font: 9pt \"Arial\";}";
    }
    ui->LvBtn->setStyleSheet(btnss);
}

////增加经验
//void kr_listwidget::receiveExp()
//{
//    judgeSqlConnectionState();
//    int exp;
//    QSqlQuery *sql_query=new QSqlQuery();
//    QString query;
//    sql_query->prepare("SELECT Exp FROM User WHERE Name = :Name");
//    sql_query->bindValue(":Name",*account);
//    sql_query->exec();
//    while(sql_query->next())
//    {
//        exp=sql_query->value(0).toInt();
//        exp+=5000;
//    }
//    query=QString("UPDATE User SET Exp = :Exp WHERE Name = :Name");
//    sql_query->prepare(query);
//    sql_query->bindValue(":Exp",exp);
//    sql_query->bindValue(":Name",*account);
//    sql_query->exec();
//}

void kr_listwidget::alarmclock()
{
    judgeSqlConnectionState();
    for(int i=0;i<taskTime.count();++i)
   {
    if(taskState.at(i)=="1")
    {
        if(taskTime.at(i)==QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:00")&&taskDetail.at(i)!="重启"&&taskDetail.at(i)!="注销"&&taskDetail.at(i)!="关机")
        {
         if(!closeTimerText->contains(taskDetail.at(i)))//假如已经被设定为不开闹钟的项目
        {
            //闹钟声音
            QSound *ringsound=new QSound(QString(":/res/sound/Sound%1.wav").arg(rand()%6));
            ringsound->play();
            int ch=Kmessagebox::showMyMessageBox(0,QString("任务:%1时间到啦").arg(taskDetail.at(i)),"确定则删除任务,取消则关闭闹钟",MESSAGE_WARNNING,BUTTON_OK_AND_CANCEL,true);
            if(ch==0)
            {
                QSqlQuery *sql_query=new QSqlQuery();
                QString query;
                query=QString("UPDATE k_mission SET m_alarmState = 0 WHERE m_id = :TaskID");
                sql_query->prepare(query);
                sql_query->bindValue(":TaskID",taskID.at(i));
                sql_query->exec();
                //删除完刷新任务表
                refreshListWidget();
                ringsound->stop();
            }else {*closeTimerText<<taskDetail.at(i);ringsound->stop();}
         }else {continue;}
        }
    }
    }
}

void kr_listwidget::listenSystemMove()
{
    judgeSqlConnectionState();
    //监听关机,注销，重启信号
    for(int i=0;i<taskTime.count();++i){
    int index_action=-1;
    if(taskDetail.at(i)=="关机"&&taskDetail.at(i)!="已设置"){index_action=0;}
    else if(taskDetail.at(i)=="注销"&&taskDetail.at(i)!="已设置"){index_action=1;}
    else if(taskDetail.at(i)=="重启"&&taskDetail.at(i)!="已设置"){index_action=2;}
    if(index_action!=-1)
    {
        QStringList timeNum;
        QRegExp re("[0-9]{2}");
        re.setMinimal(true);
        int pos=0;
        while ((pos = re.indexIn(taskTime.at(i),pos)) != -1)
        {
             QString str=re.cap(0);
             timeNum<<str;
             pos += re.matchedLength();
        }
        //提醒用户指令已设置
        QSqlQuery *sql_query=new QSqlQuery();
        QString query;
        Kmessagebox::showMyMessageBox(0,"开启成功",QString("指令已设置,将在%1:%2进行%3").arg(timeNum.at(4)).arg(timeNum.at(5)).arg(taskDetail.at(i)),MESSAGE_INFORMATION,BUTTON_OK,true);
        query=QString("UPDATE k_mission SET m_content = :TaskDetail WHERE m_id = :TaskID");
        sql_query->prepare(query);
        sql_query->bindValue(":TaskDetail",QString("已设置").toUtf8());
        sql_query->bindValue(":TaskID",taskID.at(i));
        sql_query->exec();
        //更新完刷新任务表
        refreshListWidget();
        switch(index_action)
        {
         case 0:                 //关机指令
         commend = "shutdown /s";
         break;
         case 1:                 //注销指令
         commend = "shutdown /l";
         break;
         case 2:                 //重启指令
         commend = "shutdown /r";
         break;
        }
        //执行操作
        if(taskTime.at(i)==QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:00"))
        {
            QTimer *counting=new QTimer(this);
            connect(counting,SIGNAL(timeout()),this,SLOT(shutDownComputer()));
            counting->start(10000);
            int ch=Kmessagebox::showMyMessageBox(0,"询问","确定立即执行操作指令?10秒后自动执行",MESSAGE_INFORMATION,BUTTON_OK_AND_CANCEL,true);
            if(ch==0)
            {
                shutDownComputer();
            }
            else{
                counting->stop();
                query=QString("DELETE FROM k_mission WHERE m_id=:m_id");
                sql_query->prepare(query);
                sql_query->bindValue(":m_id",taskID.at(i));
                sql_query->exec();
                refreshListWidget();
            }
        }else{
            QString timeCommend;
            timeCommend=QString("schtasks /CREATE /TN KTMMission /TR \"%1\" /SC ONCE /ST %2:%3 /F").arg(commend).arg(timeNum.at(4)).arg(timeNum.at(5));
            system(timeCommend.toLatin1().data());
        }
    }
  }
}

void kr_listwidget::getAccount(QString *userAccount)
{
    judgeSqlConnectionState();
    account=userAccount;
    userID = new QString();
    QSqlQuery *sql_query = new QSqlQuery();
    sql_query->prepare("SELECT UserID FROM k_comuser WHERE Name = :Name");
    sql_query->bindValue(":Name",*account);
    sql_query->exec();
    while(sql_query->next())
    {
        *userID=sql_query->value(0).toString();
        refreshListWidget();
    }
    //更新时间label,需要account已经收到，否则会出错
    connect(alarm_time,SIGNAL(timeout()),this,SLOT(alarmclock()));
    connect(system_time,SIGNAL(timeout()),this,SLOT(listenSystemMove()));
    //获取任务信息
    getTaskDetail();
    //监听
    alarm_time->start(3000);
    system_time->start(6000);
}

void kr_listwidget::shutDownComputer()
{
    commend+=" /t 0";
    system(commend.toLatin1().data());
}

//刷新任务表
void kr_listwidget::refreshListWidget()
{  
    updateLvButton();
    getTaskDetail();
    judgeSqlConnectionState();
    loadListWidgetBackground();
    int i=loadColorChooseFromSetting();
    int orignalColorChoose = i;
    this->setCursor(Qt::WaitCursor);//鼠标状态
    //每次用必须清空
    ui->listWidget->clear();
    QSqlQuery *sql_query=new QSqlQuery();
    QString query=QString("SELECT k.m_datetime,k.m_content,k.m_state,k.m_id,t.t_name FROM k_mission AS k,k_missionType as t WHERE m_userid = :UserID AND k.m_type = t.t_id ORDER BY m_datetime");
    sql_query->prepare(query);
    sql_query->bindValue(":UserID",*userID);
    sql_query->exec();
    while(sql_query->next())
    {
        if(sql_query->value(2).toInt() == 1||sql_query->value(2).toInt() == 2)continue;
        //清除再刷新
        QLabel *pix=new QLabel();
        pix->setPixmap(QPixmap(QString("://Resources/ListWidget/ListItemLogo/%1.png").arg(i)));
        QListWidgetItem *item=new QListWidgetItem(QString("   时间: "+sql_query->value(0).toString().replace("T"," ")+"\n   类型: "+sql_query->value(4).toString()+"\n   内容: "+QUrl::fromPercentEncoding(sql_query->value(1).toString().replace("\n"," ").toUtf8())));
        item->setToolTip("内容:"+QUrl::fromPercentEncoding(sql_query->value(1).toString().replace("\n"," ").toUtf8()));
        //存储位置的任务ID
        taskID.append(sql_query->value(3).toString());
        item->setBackgroundColor(QColor(255,255,255));
        item->setTextColor(QColor(100,100,100));
        item->setFont(QFont(loadFont(),11,3,false));
        item->setSizeHint(QSize(300,65));
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item,pix);
        i++;
        if(i>orignalColorChoose+7)
        {
            i=orignalColorChoose;
        }
    }
    this->setCursor(Qt::ArrowCursor);//鼠标状态
    deleteLabel();
    vl = new QVBoxLayout(this);
    printLabel(0);
    printLabel(1);
}

void kr_listwidget::printLabel(int type)
{
    switch (type) {
    case 0:
        //重新绘制line
        for(int num = 0,space = 0;num<ui->listWidget->count();++num,space+=69)
        {
            QLabel *line = new QLabel("<hr style=\"height:2px;border:none;border-top:2px solid #117117117;\" />");
            line->setGeometry(16,62+space,378,9);
            vl->addWidget(line);
        }
        vl->setEnabled(false);
        break;
    case 1:
        //绘制倒计时
        QStringList *tmp = new QStringList();
        for(int i = 0;i<taskTime.count();i++){
            if(taskState.at(i) != "1"&&taskState.at(i) != "2"){
                tmp->append(taskTime.at(i));
            }
        }
        //将未完成任务存入一个临时链表
        int startIndex = ui->listWidget->indexAt(QPoint(238,37)).row();
        int endIndex = (ui->listWidget->count()<7?ui->listWidget->count():7)+startIndex;
        for(int num = startIndex,space = 0;num<endIndex;++num,space+=69)
        {
            QLabel *Counting;
            QDateTime cDateTime = QDateTime::currentDateTime();
            qint64 difference = cDateTime.secsTo(QDateTime().fromString(tmp->at(num),"yyyy-MM-dd hh:mm:ss"));
            int day,hour;
            day = difference/86400;
            hour = (difference%86400)/3600;
            if(difference<0)
            {
                Counting = new QLabel("已完成");
                Counting->setStyleSheet("color:rgb(169,169,169);");
            }
            else if(hour>=0&&day>0)
            {
                Counting = new QLabel(QString("%1%2").arg(QString("%1").arg(day)+"天").arg(QString("%1").arg(hour)+"小时"));
                Counting->setStyleSheet("color:rgb(60,179,113);");
            }else
            {
                Counting = new QLabel(QString("%1%2").arg(QString("%1").arg(day)+"天").arg(QString("%1").arg(hour)+"小时"));
                Counting->setStyleSheet("color:rgb(205,92,92);");
            }
            Counting->setGeometry(285,29+space,100,15);
            Counting->setAlignment(Qt::AlignRight);
            vl->addWidget(Counting);
        }
        vl->setEnabled(false);
        break;
    }
}

void kr_listwidget::deleteLabel()
{
    //清空line
    int itemCount = this->layout()->count(); // layout是你前一个布局
    for (int i = (itemCount - 1); i >= 0; --i) //从末尾开始是因为你删除会影响布局的顺序。例如你删掉第一个，后面的会往前移，第二就变成第一个，然后这时你要是++i的话，就是删掉原来布局里的第三个，这第二个被跳过了。
    {
        QLayoutItem *item = this->layout()->takeAt(i);
        if (item != 0)
        {
            this->layout()->removeWidget(item->widget());
            delete item->widget(); //（ps：如果是子控件不是QWidget，这里会出错，要注意）
        }
    }
    delete this->layout();
}

void kr_listwidget::paintEvent(QPaintEvent *)
{
}
int kr_listwidget::loadColorChooseFromSetting()
{
    QString filepath=QCoreApplication::applicationDirPath();
    filepath+="/setting.ini";
    filepath.replace("/","\\");
    QSettings setting(filepath,QSettings::IniFormat);
    setting.beginGroup("config");
    QString colorName = setting.value("ListItemColor").toString();
    if(colorName=="Blue")
    {
        return 32;
    }else if(colorName=="Yellow")
    {
        return 0;
    }
    else if(colorName=="Orange")
    {
        return 8;
    }
    else if(colorName=="Pink")
    {
        return 16;
    }
    else if(colorName=="Green")
    {
        return 24;
    }else
    {
        return 40;
    }
    setting.endGroup();
}

void kr_listwidget::loadListWidgetBackground()
{
    QString filepath=QCoreApplication::applicationDirPath();
    filepath+="/setting.ini";
    filepath.replace("/","\\");
    QSettings setting(filepath,QSettings::IniFormat);
    setting.beginGroup("config");
    if(setting.value("BackgroundPicture").toInt()!=0)
    {
        ui->listWidget->setStyleSheet(QString("QListWidget{border:1px soild rgb(255,255,255);border-radius:8px;	background-image: url(:/Resources/ListWidget/Background/%1.png);padding:3px;}").arg(setting.value("BackgroundPicture").toString()));
    }
    else
    {
        ui->listWidget->setStyleSheet(QString("QListWidget{border:1px soild rgb(255,255,255);border-radius:8px;	background-image: url(%1);padding:3px;}").arg(setting.value("BackgroundPictureUrl").toString()));
    }
}

QString kr_listwidget::loadFont()
{
    QString filepath=QCoreApplication::applicationDirPath();
    filepath+="/setting.ini";
    filepath.replace("/","\\");
    QSettings setting(filepath,QSettings::IniFormat);
    setting.beginGroup("config");
    return setting.value("Font").toString();
}

void kr_listwidget::showMenu()
{
    static bool isShow;
    if(isShow==false)
    {
        ui->TrashStationBtn->show();
        ui->SettingBtn->show();
        ui->AppBtn->show();
        QPropertyAnimation *animation = new QPropertyAnimation(ui->addBtn,"geometry");
        animation->setDuration(300);
        animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->setStartValue(QRect(ui->addBtn->x(),ui->addBtn->y(),ui->addBtn->width(),ui->addBtn->height()));
        animation->setEndValue(QRect(ui->MenuBtn->x()-287,ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        animation->start();
        QPropertyAnimation *Canimation = new QPropertyAnimation(ui->TrashStationBtn,"geometry");
        Canimation->setDuration(300);
        Canimation->setEasingCurve(QEasingCurve::OutBounce);
        Canimation->setStartValue(QRect(ui->MenuBtn->x(),ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Canimation->setEndValue(QRect(ui->MenuBtn->x()-215,ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Canimation->start();
        QPropertyAnimation *Tanimation = new QPropertyAnimation(ui->AppBtn,"geometry");
        Tanimation->setDuration(300);
        Tanimation->setEasingCurve(QEasingCurve::OutBounce);
        Tanimation->setStartValue(QRect(ui->MenuBtn->x(),ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Tanimation->setEndValue(QRect(ui->MenuBtn->x()-143,ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Tanimation->start();
        QPropertyAnimation *Panimation = new QPropertyAnimation(ui->SettingBtn,"geometry");
        Panimation->setDuration(300);
        Panimation->setEasingCurve(QEasingCurve::OutBounce);
        Panimation->setStartValue(QRect(ui->MenuBtn->x(),ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Panimation->setEndValue(QRect(ui->MenuBtn->x()-71,ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Panimation->start();
        ui->Timelabel->hide();
        ui->LvBtn->hide();
        ui->line->hide();
        currentDatetime->stop();
        isShow=true;
    }else{
        QPropertyAnimation *animation = new QPropertyAnimation(ui->addBtn,"geometry");
        animation->setDuration(100);
        //animation->setEasingCurve(QEasingCurve::OutBounce);
        animation->setStartValue(QRect(ui->addBtn->x(),ui->addBtn->y(),ui->addBtn->width(),ui->addBtn->height()));
        animation->setEndValue(QRect(ui->MenuBtn->x()-71,ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        animation->start();
        QPropertyAnimation *Canimation = new QPropertyAnimation(ui->TrashStationBtn,"geometry");
        Canimation->setDuration(300);
        Canimation->setEasingCurve(QEasingCurve::OutBounce);
        Canimation->setStartValue(QRect(ui->MenuBtn->x(),ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Canimation->setEndValue(QRect(ui->MenuBtn->x()+215,ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Canimation->start();
        QPropertyAnimation *Tanimation = new QPropertyAnimation(ui->AppBtn,"geometry");
        Tanimation->setDuration(300);
        Tanimation->setEasingCurve(QEasingCurve::OutBounce);
        Tanimation->setStartValue(QRect(ui->MenuBtn->x(),ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Tanimation->setEndValue(QRect(ui->MenuBtn->x()+143,ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Tanimation->start();
        QPropertyAnimation *Panimation = new QPropertyAnimation(ui->SettingBtn,"geometry");
        Panimation->setDuration(300);
        Panimation->setEasingCurve(QEasingCurve::OutBounce);
        Panimation->setStartValue(QRect(ui->MenuBtn->x(),ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Panimation->setEndValue(QRect(ui->MenuBtn->x()+71,ui->MenuBtn->y(),ui->MenuBtn->width(),ui->MenuBtn->height()));
        Panimation->start();
        ui->TrashStationBtn->hide();
        ui->SettingBtn->hide();
        ui->AppBtn->hide();
        ui->Timelabel->show();
        ui->LvBtn->show();
        ui->line->show();
        currentDatetime->start(1000);
        isShow=false;
    }
}

void kr_listwidget::timeUpdate()
{
    QDateTime getCurrentTime=QDateTime::currentDateTime();
    QString timeData=getCurrentTime.toString("yyyy-MM-dd hh:mm:ss");
    ui->Timelabel->setText(timeData);
}

void kr_listwidget::initWidgets()
{
    //更新时间label
    connect(currentDatetime,SIGNAL(timeout()),this,SLOT(timeUpdate()));
    timeUpdate();
    currentDatetime->start(1000);
    //隐藏功能栏
    ui->TrashStationBtn->hide();
    ui->SettingBtn->hide();
    ui->AppBtn->hide();
    ui->listWidget->setFocusPolicy(Qt::NoFocus);
}

void kr_listwidget::initAllSignalSlot()
{
    connect(ui->addBtn,SIGNAL(clicked()),this,SLOT(callAddWidget()));
    connect(ui->MenuBtn,SIGNAL(clicked()),this,SLOT(showMenu()));
    //调用编辑任务
    connect(ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(callEditWidget(QListWidgetItem*)));
    //调用翻译界面
    connect(ui->AppBtn,SIGNAL(clicked()),this,SLOT(callAppWidget()));
    //调用设置界面
    connect(ui->SettingBtn,SIGNAL(clicked()),this,SLOT(callSettingWidget()));
    //调用项目界面
    connect(ui->TrashStationBtn,SIGNAL(clicked()),this,SLOT(callTrashStationWidget()));
    //查看等级
    connect(ui->LvBtn,SIGNAL(clicked()),this,SLOT(seeLvInfo()));
    //解决只有单独close时后台进程未清理及只有qApp->quit时析构函数无法执行的问题
    connect(qApp,SIGNAL(aboutToQuit()),this,SLOT(close()));
    connect(ui->listWidget->verticalScrollBar(),SIGNAL(valueChanged(int)),this,SLOT(updateCountdownLabel()));
}

//用于关于任务的操作,减少sql通信次数
void kr_listwidget::getTaskDetail()
{
    //清空容器
    taskTime.clear();
    taskType.clear();
    taskDetail.clear();
    taskID.clear();
    taskState.clear();
    QSqlQuery *sql_query=new QSqlQuery();
    QString query=QString("SELECT k.m_datetime,k.m_content,k.m_state,k.m_id,t.t_name FROM k_mission AS k,k_missionType as t WHERE m_userid = :UserID AND k.m_type = t.t_id ORDER BY m_datetime");
    sql_query->prepare(query);
    sql_query->bindValue(":UserID",*userID);
    sql_query->exec();
    for(int i=0;sql_query->next();++i)
   {
        taskTime<<sql_query->value(0).toString().replace("T"," ");
        taskType<<sql_query->value(4).toString();
        taskDetail<<sql_query->value(1).toString();
        taskID<<sql_query->value(3).toString();
        taskState<<sql_query->value(2).toString();
    }
}

void kr_listwidget::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem *curItem = ui->listWidget->itemAt( pos );
    //关闭自动隐藏
    emit signalStopAutoHideListWidget();
    QMenu *popMenu = new QMenu( this );
    if( curItem != NULL )
    {
        QAction *copySeed = new QAction(tr("复制"), this);
        popMenu->addAction( copySeed );
        QAction *refreshSeed = new QAction(tr("刷新"), this);
        popMenu->addAction( refreshSeed );
        popMenu->addSeparator();
        QAction *deleteSeed = new QAction(tr("删除"), this);
        popMenu->addAction( deleteSeed );
        popMenu->addSeparator();
        QAction *minimizeAction = new QAction(tr("最小化"), this);
        popMenu->addAction(minimizeAction);

        connect( copySeed, SIGNAL(triggered() ), this, SLOT( copySlot()) );
        connect( deleteSeed, SIGNAL(triggered() ), this, SLOT( deleteSlot()) );
        connect( refreshSeed, SIGNAL(triggered() ), this, SLOT( refreshListWidget() ));
        connect(minimizeAction, SIGNAL(triggered()), this, SLOT(sendMinimizeSignal()));
        popMenu->exec( QCursor::pos() );
        delete copySeed;
        delete deleteSeed;
        delete refreshSeed;
        delete minimizeAction;
    }else{
        QAction *refreshSeed = new QAction(tr("刷新"), this);
        popMenu->addAction( refreshSeed );
        popMenu->addSeparator();
        QAction *changeBackgroundPictureSeed = new QAction(tr("更换壁纸"), this);
        popMenu->addAction( changeBackgroundPictureSeed );
        QAction *minimizeAction = new QAction(tr("最小化"), this);
        popMenu->addAction(minimizeAction);

        connect( refreshSeed, SIGNAL(triggered() ), this, SLOT( refreshListWidget() ));
        connect( changeBackgroundPictureSeed, SIGNAL(triggered() ), this, SLOT( setBackgroundPictureUrl()) );
        connect(minimizeAction, SIGNAL(triggered()), this, SLOT(sendMinimizeSignal()));
        popMenu->exec( QCursor::pos() );
        delete refreshSeed;
        delete changeBackgroundPictureSeed;
        delete minimizeAction;
    }
    delete popMenu;
    //使用完毕右键菜单，开启自动隐藏
    emit signalStopAutoHideListWidget();
}

void kr_listwidget::deleteSlot()
{
    judgeSqlConnectionState();
    int ch = Kmessagebox::showMyMessageBox(0,"询问","确定删除?已删除任务可到回收站查看",MESSAGE_INFORMATION,BUTTON_OK_AND_CANCEL,true);
    if(ch == 0)
    {
        QListWidgetItem *item = ui->listWidget->currentItem();
        //判断是否是关机重启注销命令，是的话就取消掉
        if(item->text().contains("重启")||item->text().contains("注销")||item->text().contains("关机"))
        {
            QString commend="schtasks /delete /tn KTMMission /F";
            system(commend.toLatin1().data());
            Kmessagebox::showMyMessageBox(0,"指令取消","您的系统指令已经取消",MESSAGE_INFORMATION,BUTTON_OK,true);
        }
        if( item == NULL )
            return;
        Kmessagebox::showMyMessageBox(0,"操作成功","已删除任务可到回收站查看",MESSAGE_INFORMATION,BUTTON_OK,true);
        int curIndex = ui->listWidget->row(item);
        int notDeletedCount = 0;
        int pos;//存储定位所要删除任务的位置
        for(pos = 0;pos<taskState.count()-1;pos++)
        {
            if(taskState.at(pos)=="0")
                notDeletedCount++;
            if(notDeletedCount==curIndex+1)
                break;
        }
        QSqlQuery *sql_query=new QSqlQuery();
        QString query=QString("UPDATE k_mission SET m_state = 1 WHERE m_id = :TaskID");
        sql_query->prepare(query);
        sql_query->bindValue(":TaskID",taskID.at(pos));
        sql_query->exec();
        refreshListWidget();
    }
}

void kr_listwidget::copySlot()
{
    QClipboard *cb = QApplication::clipboard();
    cb->setText(ui->listWidget->currentItem()->text().replace(" ",""));
}

void kr_listwidget::judgeSqlConnectionState()
{
    QSqlQuery *checkState = new QSqlQuery();
    checkState->prepare("SELECT 1");
    if(checkState->lastError().text()!=" ")
    {
        //程序重启操作
        qApp->quit();
        QProcess::startDetached(qApp->applicationFilePath(), QStringList());
    }
}

void kr_listwidget::setBackgroundPictureUrl()
{
    int money = 0;
    QSqlQuery *sql_query=new QSqlQuery();
    QString query;
    query = "SELECT Money FROM k_comuser WHERE UserID = :UserID";
    sql_query->prepare(query);
    sql_query->bindValue(":UserID",*userID);
    sql_query->exec();
    while(sql_query->next()){
        money=sql_query->value(0).toInt();
    }
    int i = Kmessagebox::showMyMessageBox(this,"提示","更换一次壁纸需要扣除5000余额\n是否购买一次",MESSAGE_QUESTION,BUTTON_OK_AND_CANCEL,true);
    if(i==1)
        return;
    if(money<CHANGE_BACKGROUND_COST)
    {
        Kmessagebox::showMyMessageBox(this,"提示","余额不足\n多增加任务吧!",MESSAGE_INFORMATION,BUTTON_OK,true);
        return;
    }
    money-=CHANGE_BACKGROUND_COST;
    query = "UPDATE k_comuser SET Money = :Money WHERE UserID = :UserID";
    sql_query->prepare(query);
    sql_query->bindValue(":Money",money);
    sql_query->bindValue(":UserID",*userID);
    sql_query->exec();
    Kmessagebox::showMyMessageBox(this,"提示","扣除成功,壁纸最好预先裁剪为400 x 500",MESSAGE_INFORMATION,BUTTON_OK,true);
    QString path = QFileDialog::getOpenFileName(this, tr("打开背景图片"), ".", tr("Image Files(*.jpg *.png)"));
    if(path.length() == 0) {
        qDebug()<<"empty";
    }else
    {
        QString filepath=QCoreApplication::applicationDirPath();
        filepath+="/setting.ini";
        filepath.replace("/","\\");
        QSettings setting(filepath,QSettings::IniFormat);
        setting.beginGroup("config");
        setting.setValue("BackgroundPictureUrl",path);
        Kmessagebox::showMyMessageBox(this,"设置成功","已设置背景图片",MESSAGE_INFORMATION,BUTTON_OK,true);
        refreshListWidget();//更换完刷新
    }
}

void kr_listwidget::sendMinimizeSignal()
{
    emit signalSendMinimizeRequest();
}

void kr_listwidget::updateCountdownLabel()
{
    deleteLabel();
    vl = new QVBoxLayout(this);
    printLabel(0);
    printLabel(1);
}

