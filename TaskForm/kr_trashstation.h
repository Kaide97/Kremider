#ifndef KR_TRASHSTATION_H
#define KR_TRASHSTATION_H

#include <QWidget>
#include "BasicForm/kwindows.h"
namespace Ui {
class kr_TrashStation;
}

class kr_TrashStation : public Kwindows
{
    Q_OBJECT

public:
    explicit kr_TrashStation(QWidget *parent = 0);
    ~kr_TrashStation();
    //用于存储任务ID
    QStringList *taskID = new QStringList();

private slots:
    void on_ExitButton_clicked();
    //刷新任务栏
    void refreshListWidget();
    //获得账号信息 虚函数重载
    void getAccount(QString *userAccount);
    void on_RecoverBtn_clicked();
    void on_RefreshBtn_clicked();
    void on_DeleteBtn_clicked();
    void on_DeleteAllBtn_clicked();

signals:
    void signalRefreshListWidget();
private:
    Ui::kr_TrashStation *ui;
    void paintEvent(QPaintEvent *);
    void initWidget();
};

#endif // KR_TRASHSTATION_H
