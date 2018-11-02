#ifndef KR_APP_H
#define KR_APP_H

#include <QWidget>
#include "BasicForm/kwindows.h"
#include <QNetworkReply>
namespace Ui {
class kr_app;
}

class kr_app : public Kwindows
{
    Q_OBJECT

public:
    explicit kr_app(QWidget *parent = 0);
    ~kr_app();
private slots:
    void on_TransBtn_clicked();
    void on_ClassBtn_clicked();
    void on_WelcomeBtn_clicked();
    void on_PhoneNumSerachBtn_clicked();
    //收到号码归属地查询回馈
    void loadPhoneNumInfo(QNetworkReply *reply);
    void on_BookBtn_clicked();

signals:
    void signalSendAccount(QString *userAccount);
private:
    Ui::kr_app *ui;
    void initTitleBar();
    void paintEvent(QPaintEvent *event);
    //号码归属地查询
    bool IsNumber(const QString &src);
};

#endif // KR_APP_H
