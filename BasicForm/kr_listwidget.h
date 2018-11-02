#ifndef KR_LISTWIDGET_H
#define KR_LISTWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include <QList>
#include <QVBoxLayout>
#include <QLabel>
#include <QThread>
namespace Ui {
class kr_listwidget;
}

class kr_listwidget : public QWidget
{
    Q_OBJECT

public:
    explicit kr_listwidget(QWidget *parent = 0);
    //账号
    QString *account;
    QString *userID;//用于任务处理的传这个
    ~kr_listwidget();
signals:
    void signalSendAccount(QString *userAccount);
    void signalSendTaskInfo(QString time);
    //发送信息到mainwindow使leaveevent函数停止
    void signalStopAutoHideListWidget();
    //发送到mainwindow使其最小化
    void signalSendMinimizeRequest();
private slots:
    void getAccount(QString *userAccount);
    //刷新任务栏
    void refreshListWidget();
    //色块颜色选择
    int loadColorChooseFromSetting();
    //listwidget背景选择
    void loadListWidgetBackground();
    //listwidget字体选择
    QString loadFont();
    //菜单栏出现
    void showMenu();
    //时间label更新
    void timeUpdate();
    //增加任务界面
    void callAddWidget();
    //编辑界面
    void callEditWidget(QListWidgetItem *c_item);
    //应用界面
    void callAppWidget();
    //设置界面
    void callSettingWidget();
    //工具界面
    void callTrashStationWidget();
    //通知主界面读取设置
    void sendSettingRequest();
    //显示等级信息
    void seeLvInfo();
    //更新等级按钮
    void updateLvButton();
//  提升经验并存储信息 改到Mainwindow
//  void receiveExp();
    //闹钟判断与提醒
    void alarmclock();
    //监听系统操作
    void listenSystemMove();
    //关机操作函数
    void shutDownComputer();
    //右键菜单
    void on_listWidget_customContextMenuRequested(const QPoint &pos);
    void deleteSlot();
    void copySlot();
    //判断数据库是否断开
    void judgeSqlConnectionState();
    //设置个人定制背景图片
    void setBackgroundPictureUrl();
    //发送最小化信号
    void sendMinimizeSignal();
    //更新对应的倒计时标签
    void updateCountdownLabel();
private:
    Ui::kr_listwidget *ui;
    void initWidgets();
    void initAllSignalSlot();
    //时间label更新计时器
    QTimer *currentDatetime=new QTimer(this);
    //获取任务信息
    void getTaskDetail();
    QStringList taskTime,taskType,taskDetail,taskID,taskState;
    //一秒更新计时器
    QTimer *alarm_time=new QTimer(this);
    QTimer *system_time=new QTimer(this);
    int level;
    //不开闹钟的内容
    QStringList *closeTimerText;
    QString commend;//系统命令
    //画线
    QVBoxLayout *vl;
    void printLabel(int type = 0);//0为线1为倒计时
    void deleteLabel();
    void paintEvent(QPaintEvent*);
};

#endif // KR_LISTWIDGET_H
