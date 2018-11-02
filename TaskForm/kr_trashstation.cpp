#include "kr_trashstation.h"
#include "ui_kr_trashstation.h"
#include "BasicForm/kwindows.h"
#include <QPainter>
#include <QSqlQuery>
#include <QCheckBox>
#include <QDebug>
#include "BasicForm/kr_listwidget.h"
#include "BasicForm/kmessagebox.h"
kr_TrashStation::kr_TrashStation(QWidget *parent) :
    Kwindows(parent),
    ui(new Ui::kr_TrashStation)
{
    ui->setupUi(this);
    initWidget();
    //子窗口都需要,解决长时间使用内存不释放问题
    setAttribute(Qt::WA_DeleteOnClose, true);
}

kr_TrashStation::~kr_TrashStation()
{
    delete ui;
}

void kr_TrashStation::paintEvent(QPaintEvent *)
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
        if(i<4)
        {color.setAlpha(120 - i*40);}
        painter.setPen(color);
        painter.drawPath(path);
    }
}

void kr_TrashStation::initWidget()
{
    m_titleBar->hide();
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
}

void kr_TrashStation::on_ExitButton_clicked()
{
    this->close();
}

void kr_TrashStation::refreshListWidget()
{
    int i=0;
    //每次用必须清空
    taskID->clear();
    ui->listWidget->clear();
    QSqlQuery *sql_query=new QSqlQuery();
    QString query=QString("SELECT m_datetime,m_content,m_state,m_id FROM k_mission WHERE m_userid = :UserID ORDER BY m_datetime");
    sql_query->prepare(query);
    sql_query->bindValue(":UserID",*account);
    sql_query->exec();
    while(sql_query->next())
    {
        if(sql_query->value(2).toInt() == 0)continue;
        //清除再刷新
        QLabel *pix=new QLabel();
        pix->setPixmap(QPixmap(QString("://Resources/ListWidget/ListItemLogo/%1.png").arg(i)));
        QListWidgetItem *item=new QListWidgetItem(QString("   时间:"+sql_query->value(0).toString().replace("T"," ")+"\n   内容:"+QUrl::fromPercentEncoding(sql_query->value(1).toString().toUtf8())));
        //存储位置的任务ID
        taskID->append(sql_query->value(3).toString());
        item->setBackgroundColor(QColor(230,230,230));
        item->setTextColor(QColor(0,0,0));
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item,pix);
        i++;
        if(i>39)
        {
            i=0;
        }
    }
}

void kr_TrashStation::getAccount(QString *userAccount)
{
    account=userAccount;
    refreshListWidget();
}

void kr_TrashStation::on_RecoverBtn_clicked()
{
    for(int i = ui->listWidget->count()-1;i!=-1;--i)
    {
        if(ui->listWidget->item(i)->isSelected())
        {
            Kmessagebox::showMyMessageBox(this,"操作成功","已恢复至未完成任务",MESSAGE_INFORMATION,BUTTON_OK,false);
            QSqlQuery *sql_query=new QSqlQuery();
            QString query=QString("UPDATE k_mission SET State = 0 WHERE m_id = :TaskID");
            sql_query->prepare(query);
            sql_query->bindValue(":TaskID",QString(taskID->at(i)).toInt());
            sql_query->exec();
            emit signalRefreshListWidget();
            refreshListWidget();
        }
    }
}

void kr_TrashStation::on_RefreshBtn_clicked()
{
    refreshListWidget();
}
void kr_TrashStation::on_DeleteBtn_clicked()
{
    for(int i = ui->listWidget->count()-1;i!=-1;--i)
    {
        if(ui->listWidget->item(i)->isSelected())
        {
            int ch = Kmessagebox::showMyMessageBox(this,"询问","确定删除?确认后将不可再恢复!",MESSAGE_QUESTION,BUTTON_OK_AND_CANCEL,true);
            if(ch == 0)
            {
                Kmessagebox::showMyMessageBox(this,"操作成功","已删除该任务",MESSAGE_INFORMATION,BUTTON_OK,false);
                QSqlQuery *sql_query=new QSqlQuery();
                QString query=QString("DELETE FROM k_mission WHERE m_id = :TaskID");
                sql_query->prepare(query);
                sql_query->bindValue(":TaskID",QString(taskID->at(i)).toInt());
                sql_query->exec();
                refreshListWidget();
            }
        }
    }
}

void kr_TrashStation::on_DeleteAllBtn_clicked()
{
    int ch = Kmessagebox::showMyMessageBox(this,"询问","确定清空?确认后将不可再恢复!",MESSAGE_QUESTION,BUTTON_OK_AND_CANCEL,true);
    if(ch == 0)
    {
        Kmessagebox::showMyMessageBox(this,"操作成功","已清空所有任务",MESSAGE_INFORMATION,BUTTON_OK,false);
        QSqlQuery *sql_query=new QSqlQuery();
        QString query=QString("DELETE FROM k_mission WHERE m_userid = :UserID AND State = 1");
        sql_query->prepare(query);
        sql_query->bindValue(":UserID",*account);
        sql_query->exec();
        refreshListWidget();
    }
}
