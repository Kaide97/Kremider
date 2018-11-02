#include "kr_addtask.h"
#include "ui_kr_addtask.h"
#include "BasicForm/kmessagebox.h"
#include <QPainter>
#include <QDateTime>
#include <QMouseEvent>
#include <QSqlQuery>
#include "BasicForm/kwindows.h"
#include <QDebug>
#include <QSqlError>
kr_addTask::kr_addTask(QWidget *parent) :
    Kwindows(parent),
    ui(new Ui::kr_addTask)
{
    ui->setupUi(this);
    initAllSignalSlot();
    initWidget();
    initTitleBar();
    //子窗口都需要,解决长时间使用内存不释放问题
    setAttribute(Qt::WA_DeleteOnClose, true);
}

kr_addTask::~kr_addTask()
{
    delete ui;
}

void kr_addTask::sendTask()
{
    QSqlQuery *sql_query=new QSqlQuery();
    QString openid ="";
    QString query;
    query = "SELECT wxopenid FROM k_comuser WHERE UserID = :UserID";
    sql_query->prepare(query);
    sql_query->bindValue(":UserID",*account);
    sql_query->exec();
    while(sql_query->next()){
        openid = sql_query->value(0).toString();
    }
    if(isEdit==false)
    {
        if(openid != ""){
            query=QString("INSERT INTO k_mission(m_openid,m_content,m_datetime,m_userid,m_state,m_type) VALUES (:Openid,:TaskDetail,:TaskTime,:UserID,0,:MissionType)");
        }else{
            query=QString("INSERT INTO k_mission(m_content,m_datetime,m_userid,m_state,m_type) VALUES (:TaskDetail,:TaskTime,:UserID,0,:MissionType)");
        }
    }else{
        int ch=Kmessagebox::showMyMessageBox(0,"询问","是否保存修改",MESSAGE_QUESTION,BUTTON_OK_AND_CANCEL,true);
        if(ch == 0)
        {
            query=QString("UPDATE k_mission SET m_content=:TaskDetail,m_datetime=:TaskTime,m_type=:MissionType WHERE m_id = :TaskID");
        }else{
            this->close();
            return;
        }
    }
    sql_query->prepare(query);
    sql_query->bindValue(":Openid",openid);
    sql_query->bindValue(":TaskDetail",ui->DetailEdit->toPlainText().toUtf8());
    sql_query->bindValue(":TaskTime",ui->dateTimeEdit->text().toUtf8());
    sql_query->bindValue(":MissionType",ui->TaskThemeComBoBox->currentIndex());
    sql_query->bindValue(":UserID",*account);
    if(isEdit==true)
    {
        sql_query->bindValue(":TaskID",*taskID);//编辑页面下才需要
    }
    sql_query->exec();
    qDebug()<<sql_query->lastError();
    this->close();
}

void kr_addTask::deleteTask()
{
    int ch = Kmessagebox::showMyMessageBox(0,"询问","确定删除?已删除任务可到回收站查看",MESSAGE_INFORMATION,BUTTON_OK_AND_CANCEL,true);//弹出消息框二次确认删除
    if(ch == 0)
    {
        //判断是否是关机重启注销命令，是的话就取消掉系统指令
        if(ui->DetailEdit->toPlainText()=="重启"||ui->DetailEdit->toPlainText()=="注销"||ui->DetailEdit->toPlainText()=="关机")
        {
            QString commend="schtasks /delete /tn KTMMission /F";
            system(commend.toLatin1().data());
            Kmessagebox::showMyMessageBox(0,"指令取消","您的系统指令已经取消",MESSAGE_INFORMATION,BUTTON_OK,true);
        }
        QSqlQuery *sql_query=new QSqlQuery();//同步数据库改变任务状态
        QString query;
        query=QString("UPDATE k_mission SET m_state = 1 WHERE m_id = :TaskID");
        sql_query->prepare(query);
        sql_query->bindValue(":TaskID",*taskID);
        sql_query->exec();
        this->close();
    }else{
        return;
    }
}

void kr_addTask::getTaskID(QString TaskID)
{
    //获取时间后再获取任务ID
    taskID=new QString();
    *taskID=TaskID;
    ui->ExitBtn->setText("删除");
    ui->EditExitButton->show();
    disconnect(ui->ExitBtn,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->ExitBtn,SIGNAL(clicked()),this,SLOT(deleteTask()));
    QSqlQuery *sql_query=new QSqlQuery();
    QString query;
    query=QString("SELECT k.m_content,k.m_datetime,t.t_name FROM k_mission as k,k_missionType as t WHERE m_id = :TaskID AND k.m_type = t.t_id");
    sql_query->prepare(query);
    sql_query->bindValue(":TaskID",*taskID);
    sql_query->exec();
    while (sql_query->next()) {
        ui->DetailEdit->setText(QUrl::fromPercentEncoding(sql_query->value(0).toString().toUtf8()));
        ui->dateTimeEdit->setDateTime(sql_query->value(1).toDateTime());
        ui->TaskThemeComBoBox->setCurrentText(sql_query->value(2).toString());
    }
}

void kr_addTask::initAllSignalSlot()
{
    connect(ui->ExitBtn,SIGNAL(clicked()),this,SLOT(close()));
    connect(ui->SaveBtn,SIGNAL(clicked()),this,SLOT(sendTask()));
}

void kr_addTask::paintEvent(QPaintEvent *event)
{
    //绘制窗口白色背景色;
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRoundedRect(QRect(10, 10, this->width()-20, this->height()-20),8,8);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(255, 255, 255)));
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundedRect(QRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2),8,8);
        if(i<3)
        {color.setAlpha(100 - i*50);}
        painter.setPen(color);
        painter.drawPath(path);
    }

    return QWidget::paintEvent(event);
}

void kr_addTask::initWidget()
{
    //初始化时间选择
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->EditExitButton->hide();
}

// 初始化标题栏
void kr_addTask::initTitleBar()
{
    m_titleBar->move(1, 2);
    m_titleBar->setWindowBorderWidth(2);
    m_titleBar->setBackgroundColor(255, 255, 255);
    m_titleBar->setButtonType(ONLY_CLOSE_BUTTON);
    m_titleBar->setTitleWidth(this->width());
    m_titleBar->hide();
}

void kr_addTask::on_EditExitButton_clicked()
{
    this->close();
}
