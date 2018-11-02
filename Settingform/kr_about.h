#ifndef KR_ABOUT_H
#define KR_ABOUT_H

#include <QWidget>
#include "BasicForm/kwindows.h"
namespace Ui {
class kr_about;
}

class kr_about : public Kwindows
{
    Q_OBJECT

public:
    explicit kr_about(QWidget *parent = 0);
    ~kr_about();
private slots:
    //打开新浪微博
    void callWeiboSite();
    //换页
    void changePage();
    //个人主页跳转
    void goPersonalWebSite();
private:
    Ui::kr_about *ui;
    void paintEvent(QPaintEvent*);
    void initFirst();
    void initSecond();
    void initAllSignalSlot();
};

#endif // KR_ABOUT_H
