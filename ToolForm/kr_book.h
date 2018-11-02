#ifndef KR_BOOK_H
#define KR_BOOK_H

#include <QWidget>
#include "BasicForm/kwindows.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTableWidgetItem>
namespace Ui {
class kr_book;
}

class kr_book : public Kwindows
{
    Q_OBJECT

public:
    explicit kr_book(QWidget *parent = 0);
    ~kr_book();

private slots:
    void getAccount(QString *userAccount);
    void on_SearchBtn_clicked();
    void loadBookInfo(QNetworkReply *);
    void loadBookLogo(QNetworkReply *reply);
    void on_AcceptBtn_clicked();
    void on_AddBtn_clicked();
    void on_EditBtn_clicked();
    void on_tableWidget_book_itemChanged(QTableWidgetItem *item);

    void on_BackBtn_clicked();

private:
    //获取用户位置
    void getBookInfo();
    QNetworkAccessManager *book_netmanager;
    QNetworkReply *book_reply;
    QUrl bookUrl;
    QNetworkAccessManager *booklogo_netmanager;
    QNetworkReply *booklogo_reply;
    QMap<QString,QString> *map;

private:
    Ui::kr_book *ui;
    void initTitleBar();
    void initFrame();
    void paintEvent(QPaintEvent *);
    int UserID;
    int getUserID();
    void loadBookRecord();
    bool isInitFinished;
};

#endif // KR_BOOK_H
