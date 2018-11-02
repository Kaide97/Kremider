#ifndef KR_CLASS_H
#define KR_CLASS_H

#include <QWidget>
#include "BasicForm/kwindows.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QListWidgetItem>
namespace Ui {
class kr_class;
}

class kr_class : public Kwindows
{    Q_OBJECT

public:
    explicit kr_class(QWidget *parent = 0);
    ~kr_class();
signals:
     void signalCampusNewsUrl(QString url);
private slots:
     //登录验证
     void verifyIdentity();
     void IdentityInfoloaded();
     //获取信息
     void getPersonalInfo();
     void personalInfoloaded();
     //加载验证码
     void loadVcode();
     //处理图片
     void replyFinished(QNetworkReply*reply);
     void on_LoginBtn_clicked();
private slots:
     //加载校内新闻
     void campusNewsLoaded(QNetworkReply*);
     //打开该新闻页面
     void openDetailWebsite(QListWidgetItem*currentItem);
     //更换新闻类型
     void changeWebsite(int index);
     //刷新新闻
     void on_RefreshBtn_clicked();
private slots:
     //初始化课表
     void initClassList();
     //查询课表
     void queryClassList();
     //课表已经加载完毕
     void classListloaded(QNetworkReply*cm_reply);
private slots:
     //加载考级成绩
     void loadCETSGrade(QNetworkReply *Cets_reply);
     //加载考试成绩
     void loadTestGrade(QNetworkReply *Test_reply);
     void loadSemTestGrade(QString str);//更换学期的时候显示
     void calculateGPA(int pos);
     //转页按钮函数
     void stackWidgetPageChanged();
     void on_LastPageBtn_clicked();
     void on_NextPageBtn_clicked();
private:
    Ui::kr_class *ui;
    void initTitleBar();
    void paintEvent(QPaintEvent *);
    void initAllSignalSlot();
    void initWidget();
    //登录
    QNetworkAccessManager *d_netmanager;
    QNetworkReply *d_reply;
    QNetworkRequest *d_request;
    //获取个人信息
    QString *str;
    //验证码cookies
    QList<QNetworkCookie>  cookies;
    //获取学号密码
    void get_ac_pwd();
    //存储学号密码
    void save_ac_pwd();
private:
     //加载校内新闻
     void initCampusNews();
     void loadCampusNews();
     QNetworkAccessManager *m_netmanager;
     QNetworkReply *m_reply;
     QStringList m_News_time,m_News_title,m_News_href,m_News_department,m_News_all;
     QUrl m_news_website;
     //课表加载
     void getClassList();
     void getCurrentWeek_Sem();
     void initSemComboBox();
     QDate currentDate;
     QNetworkAccessManager *cm_netmanager;
     QNetworkRequest *cm_request;
     QString m_class_url;
     int currentSem,currentWeek;
     //随机采用生成一种颜色
     QList<int> chooseColor(int);
     //加载考级成绩
     void getCETSGrade();
     //加载考试成绩
     void getTestGrade();
     QStringList m_Test_xq,m_Test_name,m_Test_totalGrade,m_Test_xf;
     double GPA,xf;
     bool hasLoadGrade;
};

#endif // KR_CLASS_H
