#ifndef KWINDOWS_H
#define KWINDOWS_H

#include <QWidget>
#include "ktitlebar.h"

class Kwindows : public QWidget
{
    Q_OBJECT

public:
    Kwindows(QWidget *parent = 0);
    //子窗口传递账号信息用
    QString *account;
    ~Kwindows();

protected:
    void initTitleBar();
    void paintEvent(QPaintEvent *event);
    void loadStyleSheet(const QString &sheetName);
public slots:
    virtual void getAccount(QString *userAccount);
private slots:
    void onButtonMinClicked();
    void onButtonRestoreClicked();
    void onButtonMaxClicked();
    void onButtonCloseClicked();
protected:
    Ktitlebar* m_titleBar;
    //移动窗口事件
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    //拉拽实现部分
    bool isDrag;
    QPoint m_position;
};

#endif // KWINDOWS_H
