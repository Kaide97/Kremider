#ifndef KR_ADDTASK_H
#define KR_ADDTASK_H

#include <QWidget>
#include "BasicForm/kwindows.h"

namespace Ui {
class kr_addTask;
}

class kr_addTask : public Kwindows
{
    Q_OBJECT

public:
    explicit kr_addTask(QWidget *parent = 0);
    //编辑模式
    bool isEdit;
    ~kr_addTask();

private slots:
    //获取编辑用索引
    void getTaskID(QString TaskID);
    //直接写入数据库
    void sendTask();
    //删除
    void deleteTask();
    void on_EditExitButton_clicked();

private:
    Ui::kr_addTask *ui;
    void initTitleBar();
    void initAllSignalSlot();
    void paintEvent(QPaintEvent *event);
    //初始化控件
    void initWidget();
    //编辑页面索引
    QString *taskID;
};

#endif // KR_ADDTASK_H
