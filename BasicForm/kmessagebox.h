#ifndef KMESSAGEBOX_H
#define KMESSAGEBOX_H

#include <QWidget>
#include "kwindows.h"
#include <QEventLoop>

namespace Ui {
class Kmessagebox;
}

enum ChooseResult
{
    ID_OK = 0,						// 确定
    ID_CANCEL						// 取消
};

enum MessageType
{
    MESSAGE_INFORMATION = 0,		// 提示信息
    MESSAGE_WARNNING,				// 提示警告
    MESSAGE_QUESTION,				// 提示询问
    MESSAGE_INPUT					// 提示输入框
};

enum MessageButtonType
{
    BUTTON_OK = 0,					// 只有确认按钮
    BUTTON_OK_AND_CANCEL,			// 确认和取消按钮
    BUTTON_AUTO_OK_AND_CANEL,       // 自动确认
    BUTTON_CLOSE					// 关闭按钮
};

class Kmessagebox : public Kwindows
{
    Q_OBJECT

public:
    explicit Kmessagebox(QWidget *parent = 0);
    ~Kmessagebox();

    void setWindowTitle(QString title, int titleFontSize = 10);
    void setContentText(QString contentText);
    void setMessageType(MessageType messageType);
    void setButtonType(MessageButtonType buttonType);
    void setMessageContent(QString messageContent);

public:
    int static showMyMessageBox(QWidget* parent, const QString &title,const QString &contentText , MessageType messageType, MessageButtonType messageButtonType , bool isModelWindow = false);

private:
    void initWindow();
    void initTitleBar();
    int exec();

    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onAutoOKStart(int TimerCount);

private:
    Ui::Kmessagebox *ui;
    QEventLoop* m_eventLoop;
    ChooseResult m_chooseResult;
};

#include <QThread>

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    int TimeCount = -1;
    explicit WorkerThread(QObject *parent = 0)
        : QThread(parent)
    {

    }
    void initTimeCount(int timeCount){
        this->TimeCount = timeCount;
    }

protected:
    virtual void run() Q_DECL_OVERRIDE {
        while (TimeCount > -1)
        {
            // 准备更新
            emit resultReady(TimeCount);
            TimeCount--;
            msleep(1000);
        }
        exit(0);
    }
signals:
    void resultReady(int value);
};

#endif // KMESSAGEBOX_H
