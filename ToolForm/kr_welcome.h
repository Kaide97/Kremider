#ifndef KR_WELCOME_H
#define KR_WELCOME_H

#include <QWidget>
#include "BasicForm/kwindows.h"
#include <QNetworkReply>
namespace Ui {
class kr_welcome;
}

class kr_welcome : public Kwindows
{
    Q_OBJECT

public:
    explicit kr_welcome(QWidget *parent = 0);
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    bool hasGetAccount;
    ~kr_welcome();
private slots:
    //获得账号信息
    void getAccount(QString *userAccount);
    void loadIp(QNetworkReply *reply);
    void loadLocation(QNetworkReply *);
    void loadWeather(QNetworkReply *);
    void closeMove();
    //俱乐部赛事信息收到
    void loadMatchesInfo(QNetworkReply *);
    //显示俱乐部logo
    void loadHostTeamLogo(QNetworkReply *reply);
    void loadGuestTeamLogo(QNetworkReply *reply);
    //移动显示其他时间的比赛
    void showLastGame();
    void showNextGame();
    //比赛订阅
    void on_SubscribeBtn_clicked();
private:
    Ui::kr_welcome *ui;
    void initTitleBar();
    void initFrame();
    void initMovement();
    void paintEvent(QPaintEvent *);
    //获取IP
    void getIp();
    //获取用户位置
    QNetworkAccessManager *location_netmanager;
    QNetworkReply *location_reply;
    QUrl locationUrl;
    //获取天气
    void getWeather(QString l);
    QNetworkAccessManager *weather_netmanager;
    QNetworkReply *weather_reply;
    QUrl weatherUrl;
    QString my_ip;
    //关闭倒计时
    QTimer *closeTimer;
    //获取足球赛事信息
    void initFootballClurUrl();
    void requestMatchesInfo();
    void sortAndShowGame();
    int pos;//定位比赛显示的位置
    QNetworkReply *match_reply;
    QMap<QString,QString> footballClubUrl;
    QStringList m_game_week,m_game_time,m_team_name,m_team_logoUrl,m_team_hostPoint,m_team_guestPoint;
};

#endif // KR_WELCOME_H
