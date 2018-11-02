#include "BasicForm//kr_login.h"
#include "ScreenShot/CHotKeyEventFilter.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    kr_login *w=new kr_login();
    //新建主窗口控件
    w->createMainwindow();
    //绑定热键
    CHotKeyEventFilter hk(w->km->winId());
    QObject::connect(&hk, &CHotKeyEventFilter::HotKeyTrigger, w->km, &kr_mainwindow::Screenshot);
    a.installNativeEventFilter(&hk);
    w->show();

    return a.exec();
}
