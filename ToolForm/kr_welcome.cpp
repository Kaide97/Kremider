/*缓存释放在initFrame中*/
#include "kr_welcome.h"
#include "ui_kr_welcome.h"
#include "BasicForm/kwindows.h"
#include "BasicForm/kmessagebox.h"
#include <QNetworkReply>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QStringList>
#include <QPainter>
#include <QHostInfo>
#include <QDebug>
#include <QSettings>
#include <QSqlQuery>
#include <QRegExp>
kr_welcome::kr_welcome(QWidget *parent) :
    Kwindows(parent),
    ui(new Ui::kr_welcome),
closeTimer(new QTimer(this)),
hasGetAccount(false)
{
    ui->setupUi(this);
    initTitleBar();
    getIp();
    initFrame();
    initFootballClurUrl();
    requestMatchesInfo();
}

kr_welcome::~kr_welcome()
{
    delete ui;
}

//获取用户信息
void kr_welcome::getAccount(QString *userAccount)
{
    account=userAccount;
    hasGetAccount=true;
}

void kr_welcome::getWeather(QString l)
{
    weatherUrl = "https://www.sojson.com/open/api/weather/json.shtml?city="+l;
    weather_netmanager = new QNetworkAccessManager;
    weather_reply = weather_netmanager->get(QNetworkRequest(weatherUrl));
    connect(weather_netmanager, SIGNAL(finished(QNetworkReply*)),this, SLOT(loadWeather(QNetworkReply*)));
}
void kr_welcome::getIp()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *reply = manager->get(QNetworkRequest(QUrl("http://whois.pconline.com.cn/")));
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(loadIp(QNetworkReply*)));
}

void kr_welcome::loadIp(QNetworkReply *reply)
{
    QByteArray responseData;
    responseData = reply->readAll();
    QString web = responseData.replace(" ", "");
    QRegExp re("\"([0-9]+\.){3}[0-9]+");
    int pos=0;
    while ((pos = re.indexIn(web,pos)) != -1)
    {
        QString str=re.cap(0);
        str.replace("\"","");
        my_ip=str;
        pos += re.matchedLength();
    }
    //getlocation
    locationUrl = "http://ip.taobao.com/service/getIpInfo.php?ip="+my_ip;
    location_netmanager = new QNetworkAccessManager;
    location_reply = location_netmanager->get(QNetworkRequest(locationUrl));
    connect(location_netmanager, SIGNAL(finished(QNetworkReply*)),this, SLOT(loadLocation(QNetworkReply*)));
}

void kr_welcome::loadLocation(QNetworkReply *)
{
    location_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    location_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (location_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = location_reply->readAll();
        QString string = QString::fromUtf8(bytes);
        string.replace("\"city\":\"","");
        QStringList s;
        s = string.split(",");
        QString city = s.at(5);
        do {
            int idx = city.indexOf("\\u");
            QString strHex = city.mid(idx, 6);
            strHex = strHex.replace("\\u", QString());
            int nHex = strHex.toInt(0, 16);
            city.replace(idx, 6, QChar(nHex));
        } while (city.indexOf("\\u") != -1);
        city.replace("\"","");
        getWeather(city);
    }
}

void kr_welcome::loadWeather(QNetworkReply *)
{
    weather_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    weather_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QStringList s;
    if (weather_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = weather_reply->readAll();
        QString string = QString::fromUtf8(bytes);
        qDebug()<<string;
        string.replace("\"","");
        string.replace("data:","");
        string.replace("yesterday:","");
        string.replace("forecast:","");
        string.replace("sunrise:","");
        string.replace("sunset:","");
        string.replace("[","");
        string.replace("{","");
        string.replace("}","");
        string.replace(",",":");
        s = string.split(":");
        QMap<QString,QString> *weatherInfo = new QMap<QString,QString>();
        int dateCount=0;
        qDebug()<<s.count();
        if(s.count()>9){
            for(int i = 0;i<s.count();i+=2)//在if语句中结束循环
        {
            if(s.at(i)=="date")
            {
                ++dateCount;
                if(dateCount>2)
                {
                    break;
                }
            }
            weatherInfo->insert(s.at(i),s.at(i+1));
        }
        QString weatherTitle = weatherInfo->value("city")+" "+weatherInfo->value("type")+" ";
        QString weatherText = "温度:"+weatherInfo->value("wendu")+"℃ 湿度:"+weatherInfo->value("shidu")+" pM2.5:"+weatherInfo->value("pm25")+"\n空气质量"+weatherInfo->value("quality")+" "+weatherInfo->value("high")+" "+weatherInfo->value("low")+"\n空气质量建议:"+weatherInfo->value("ganmao");
        ui->IPlabel->setText("IP:"+ my_ip);
        ui->WeathTitllabel->setText(weatherTitle);
        ui->Weatherlabel->setText(weatherText);
        ui->Weatherlabel->setToolTip(weatherInfo->value("ganmao"));
        }
    }
}

void kr_welcome::initTitleBar()
{
    m_titleBar->hide();
    /*m_titleBar->move(1, 2);
    m_titleBar->setBackgroundColor(255,255, 255);
    //m_titleBar->setTitleIcon(":/Resources/titleicon.png");
    m_titleBar->setTitleContent(QString("K管理今日动态"));
    m_titleBar->setButtonType(ONLY_CLOSE_BUTTON);
    m_titleBar->setTitleWidth(this->width());
    m_titleBar->setWindowBorderWidth(2);*/
}

void kr_welcome::initFrame()
{
    QRect deskRect = QApplication::desktop()->availableGeometry();
    this->move(deskRect.right()-this->width(),deskRect.height()+50);//位置
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow);
    //子窗口都需要,解决长时间使用内存不释放问题
    setAttribute(Qt::WA_DeleteOnClose, true);
    //初始化自动退出时钟
    connect(closeTimer,SIGNAL(timeout()),this,SLOT(closeMove()));
    //初始化退出按钮
    connect(ui->ExitButton,SIGNAL(clicked()),this,SLOT(closeMove()));
    //初始化比赛查看按钮
    connect(ui->LastGameBtn,SIGNAL(clicked()),this,SLOT(showLastGame()));
    connect(ui->NextGameBtn,SIGNAL(clicked()),this,SLOT(showNextGame()));
}

void kr_welcome::initMovement()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(1000);
    animation->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
    animation->setEndValue(QRect(this->x(),this->y()-this->height()-50,this->width(),this->height()));
    animation->start();
    closeTimer->setInterval(8000);
    closeTimer->setSingleShot(true);
    connect(animation,SIGNAL(finished()),closeTimer,SLOT(start()));
}

//鼠标焦点进入则不自动关闭
void kr_welcome::enterEvent(QEvent *)
{
    closeTimer->stop();
}

void kr_welcome::leaveEvent(QEvent *)
{
    closeTimer->start();
}

void kr_welcome::paintEvent(QPaintEvent *)
{
    // 绘制窗口白色背景色;
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRect(QRect(0, 0, this->width(), this->height()));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(255, 255, 255)));

    // 绘制窗口灰色边框;
    QPen pen(QColor(204, 204, 204));
    painter.setPen(pen);
    painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

    // 绘制窗口上方蓝条;
    QPainterPath pathHead;
    pathHead.setFillRule(Qt::WindingFill);
    pathHead.addRect(QRect(0, 0, this->width(), 3));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathHead, QBrush(QColor(15, 151, 255)));
}

void kr_welcome::initFootballClurUrl()
{
    QSqlQuery *sql_query=new QSqlQuery();
    sql_query->prepare("SELECT * FROM k_footballTeamInfo");
    sql_query->exec();
    while(sql_query->next())
    {
        footballClubUrl.insert(sql_query->value(0).toString(),sql_query->value(1).toString());
    }
}

void kr_welcome::closeMove()
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
    animation->setDuration(500);
    animation->setStartValue(QRect(this->x(),this->y(),this->width(),this->height()));
    animation->setEndValue(QRect(this->x(),this->y()+this->height()+50,this->width(),this->height()));
    animation->start();
    connect(animation,SIGNAL(finished()),this,SLOT(close()));
}

void kr_welcome::requestMatchesInfo()
{
    QString filepath=QCoreApplication::applicationDirPath();
    filepath+="/setting.ini";
    filepath.replace("/","\\");
    QSettings setting(filepath,QSettings::IniFormat);
    setting.beginGroup("config");
    QNetworkAccessManager *match_netmanager = new QNetworkAccessManager();
    QNetworkRequest *match_request = new QNetworkRequest();
    match_request->setUrl(QUrl(footballClubUrl.value(setting.value("FootBallClub").toString())));
    match_request->setRawHeader("Content-Type","application/x-www-form-urlencoded; charset=UTF-8");
    match_request->setRawHeader("Connection", "Keep-Alive");
    match_request->setRawHeader("Accept","application/json, text/javascript, */*; q=0.01");
    match_request->setRawHeader("Accept-Language","zh-CN,zh;q=0.8");
    match_request->setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
    match_reply = match_netmanager->get(*match_request);
    setting.endGroup();
    connect(match_netmanager, SIGNAL(finished(QNetworkReply*)),this, SLOT(loadMatchesInfo(QNetworkReply*)));
}

void kr_welcome::loadMatchesInfo(QNetworkReply *)
{
    match_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    match_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (match_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = match_reply->readAll();
        QString string = QString::fromUtf8(bytes);

        //队伍名称适配
        QRegExp re("team_name\">([^<]+)");
        //re.setMinimal(true);此处需要贪婪适配
        int pos=0;// = re.indexIn(string);
        while ((pos = re.indexIn(string,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("team_name\">","");
            m_team_name<<str;
            pos += re.matchedLength();
        }
        //队伍Logo图片适配 三种图片格式
        re.setPattern("https://img.dongqiudi.com/data/pic/[0-9]*.png|https://img.dongqiudi.com/soccer/data/logo/team/[0-9]*.png|https://img1.dongqiudi.com/fastdfs/M00/AE/4A/ooYBAFia1_aATVKcAAAuzVz-1sU[0-9]*.png|http://img.dongqiudi.com/data/pic/[0-9]*.png|http://img.dongqiudi.com/soccer/data/logo/team/[0-9]*.png|http://img1.dongqiudi.com/fastdfs/M00/AE/4A/ooYBAFia1_aATVKcAAAuzVz-1sU[0-9]*.png");
        //re.setMinimal(true);此处需要贪婪适配
        pos=0;// = re.indexIn(string);
        while ((pos = re.indexIn(string,pos)) != -1)
        {
            QString str=re.cap(0);
            m_team_logoUrl<<str;
            pos += re.matchedLength();
        }//此处logo从第二个开始才是赛程队伍Logo，第一个是关注队伍的logo
        re.setPattern("[0-9]{4}-[0-9]{2}-[0-9]{2}[^<]+");
        //re.setMinimal(true);此处需要贪婪适配
        pos=0;// = re.indexIn(string);
        while ((pos = re.indexIn(string,pos)) != -1)
        {
            QString str=re.cap(0);
            str=QDateTime::fromString(str,"yyyy-MM-dd HH:mm:ss").addSecs(28800).toString("yyyy-MM-dd HH:mm:ss");
            m_game_time<<str;
            pos += re.matchedLength();
        }
        //比赛轮次适配
        re.setPattern("gameweek\">[^<]+");
        //re.setMinimal(true);此处需要贪婪适配
        pos=0;// = re.indexIn(string);
        while ((pos = re.indexIn(string,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("gameweek\">","");
            str.replace("\n","");
            str.replace(" ","");
            m_game_week<<str;
            pos += re.matchedLength();
        }
        //比分适配-主场
        re.setPattern("fs_a\">[^/]+");
        //re.setMinimal(true);此处需要贪婪适配
        pos=0;// = re.indexIn(string);
        while ((pos = re.indexIn(string,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("fs_a\">","");
            str.replace("<","");//解决场次取消没比分的情况
            if(str=="")
            {
                str="-";
            }
            m_team_hostPoint<<str;
            pos += re.matchedLength();
        }
        //比分适配-客场
        re.setPattern("fs_b\">[^/]+");
        //re.setMinimal(true);此处需要贪婪适配
        pos=0;// = re.indexIn(string);
        while ((pos = re.indexIn(string,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("fs_b\">","");
            str.replace("<","");//解决场次取消没比分的情况
            if(str=="")
            {
                str="-";
            }
            m_team_guestPoint<<str;
            pos += re.matchedLength();
        }
        sortAndShowGame();
    }
}

void kr_welcome::sortAndShowGame()
{
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:hh:mm");
    pos = 0;
    for(;pos<m_game_time.count()-1;++pos)
    {
        if(m_game_time.at(pos)>currentTime)
        {
            break;
        }
    }
    ui->GameTimeAndWeekLabel->setText(m_game_week.at(pos)+"\t"+m_game_time.at(pos));
    if(m_team_hostPoint.count()-1<pos)
    {
        ui->GameInfoLabel->setText(m_team_name.at(2*pos)+" "+"-:-"+" "+m_team_name.at(2*pos+1));
    }
    else{
        ui->GameInfoLabel->setText(m_team_name.at(2*pos)+" "+m_team_hostPoint.at(pos)+":"+m_team_guestPoint.at(pos)+" "+m_team_name.at(2*pos+1));
    }
    QNetworkAccessManager *host_manager=new QNetworkAccessManager();
    QNetworkAccessManager *guest_manager=new QNetworkAccessManager();
    connect(host_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(loadHostTeamLogo(QNetworkReply *)));
    connect(guest_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(loadGuestTeamLogo(QNetworkReply *)));
    host_manager->get(QNetworkRequest(QUrl(m_team_logoUrl.at(2*pos+1))));
    guest_manager->get(QNetworkRequest(QUrl(m_team_logoUrl.at(2*(pos+1)))));
    //加载完毕再显示
    initMovement();
}


void kr_welcome::loadHostTeamLogo(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
            QByteArray bytes = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(bytes);
            ui->HostLogoLabel->setPixmap(pixmap);
   }
}

void kr_welcome::loadGuestTeamLogo(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
            QByteArray bytes = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(bytes);
            ui->GuestLogoLabel->setPixmap(pixmap);
   }
   ui->LastGameBtn->setEnabled(true);
   ui->NextGameBtn->setEnabled(true);
}

void kr_welcome::showLastGame()
{
    closeTimer->stop();//防止因为异步导致仍然自动隐藏
    ui->LastGameBtn->setEnabled(false);//阻塞按钮防止太快出现异常
    if(pos-1<0)
    {
        return;//防止溢出
    }
    --pos;
    ui->GameTimeAndWeekLabel->setText(m_game_week.at(pos)+"\t"+m_game_time.at(pos));
    if(m_team_hostPoint.count()-1<pos)
    {
        ui->GameInfoLabel->setText(m_team_name.at(2*pos)+" "+"-:-"+" "+m_team_name.at(2*pos+1));
    }
    else{
        ui->GameInfoLabel->setText(m_team_name.at(2*pos)+" "+m_team_hostPoint.at(pos)+":"+m_team_guestPoint.at(pos)+" "+m_team_name.at(2*pos+1));
    }
    QNetworkAccessManager *host_manager=new QNetworkAccessManager();
    QNetworkAccessManager *guest_manager=new QNetworkAccessManager();
    connect(host_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(loadHostTeamLogo(QNetworkReply *)));
    connect(guest_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(loadGuestTeamLogo(QNetworkReply *)));
    host_manager->get(QNetworkRequest(QUrl(m_team_logoUrl.at(2*pos+1))));
    guest_manager->get(QNetworkRequest(QUrl(m_team_logoUrl.at(2*(pos+1)))));
}

void kr_welcome::showNextGame()
{
    closeTimer->stop();//防止因为异步导致仍然自动隐藏
    ui->NextGameBtn->setEnabled(false);//阻塞按钮防止太快出现异常
    if(pos+1==m_game_time.count())
    {
        return;
    }
    ++pos;
    ui->GameTimeAndWeekLabel->setText(m_game_week.at(pos)+"\t"+m_game_time.at(pos));
    if(m_team_hostPoint.count()-1<pos)
    {
        ui->GameInfoLabel->setText(m_team_name.at(2*pos)+" "+"-:-"+" "+m_team_name.at(2*pos+1));
    }
    else{
        ui->GameInfoLabel->setText(m_team_name.at(2*pos)+" "+m_team_hostPoint.at(pos)+":"+m_team_guestPoint.at(pos)+" "+m_team_name.at(2*pos+1));
    }
    QNetworkAccessManager *host_manager=new QNetworkAccessManager();
    QNetworkAccessManager *guest_manager=new QNetworkAccessManager();
    connect(host_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(loadHostTeamLogo(QNetworkReply *)));
    connect(guest_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(loadGuestTeamLogo(QNetworkReply *)));
    host_manager->get(QNetworkRequest(QUrl(m_team_logoUrl.at(2*pos+1))));
    guest_manager->get(QNetworkRequest(QUrl(m_team_logoUrl.at(2*(pos+1)))));
}

//订阅比赛消息到任务栏
void kr_welcome::on_SubscribeBtn_clicked()
{
    if(hasGetAccount)
    {
        QSqlQuery *userId_query = new QSqlQuery();
        userId_query->prepare("SELECT UserID FROM k_comuser WHERE Name = :Name");
        userId_query->bindValue(":Name",*account);
        userId_query->exec();
        while(userId_query->next())
        {
            *account=userId_query->value(0).toString();
        }
        QSqlQuery *subscribe_query = new QSqlQuery();
        subscribe_query->prepare("INSERT INTO k_mission(m_content,m_datetime,m_userid,m_state) VALUES (:TaskDetail,:TaskTime,:UserID,0)");
        subscribe_query->bindValue(":TaskDetail",QString(m_game_week.at(pos)+" "+m_game_time.at(pos)).toUtf8()+"\n"+ui->GameInfoLabel->text().toUtf8());
        subscribe_query->bindValue(":TaskTime",m_game_time.at(pos).toUtf8());
        subscribe_query->bindValue(":UserID",*account);
        subscribe_query->exec();
        Kmessagebox::showMyMessageBox(0,"订阅比赛成功","右键任务栏菜单点击刷新,即可看见订阅的任务",MESSAGE_INFORMATION,BUTTON_OK,false);
    }
}
