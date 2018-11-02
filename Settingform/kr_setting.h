#ifndef KR_SETTING_H
#define KR_SETTING_H

#include <QWidget>
#include "BasicForm/kwindows.h"
namespace Ui {
class kr_setting;
}

class kr_setting : public Kwindows
{
    Q_OBJECT

public:
    explicit kr_setting(QWidget *parent = 0);
    ~kr_setting();
private slots:
    //保存设置
    void saveSetting();
    //加载设置
    void loadSetting();
    //显示滑动条数值
    void seeSliderValue(int value);
    //重置设置
    void resetSetting();
    //跳转到网址
    void callUpdate();
    //跳转到关于
    void callAbout();
    void on_Back2LoginBtn_clicked();

private:
    Ui::kr_setting *ui;
    void paintEvent(QPaintEvent *event);
    void initAllSignalSlot();
    void initTitleBar();
    //初始化队伍combobox
    void initTeamNameComboBox();
    //原透明度
    int lastOpactiy;

};

#endif // KR_SETTING_H
