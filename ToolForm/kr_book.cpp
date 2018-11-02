#include "kr_book.h"
#include "ui_kr_book.h"
#include "BasicForm/kwindows.h"
#include <QPainter>
#include <QMap>
#include <QSqlQuery>
#include <QSqlError>
kr_book::kr_book(QWidget *parent) :
    Kwindows(parent),
isInitFinished(false),
  ui(new Ui::kr_book)
{
    ui->setupUi(this);
    initTitleBar();
    initFrame();
}

kr_book::~kr_book()
{
    delete ui;
}

void kr_book::getAccount(QString *userAccount)
{
    //开始初始的入口
    account = userAccount;
    UserID = getUserID();
    loadBookRecord();
}

void kr_book::initTitleBar()
{
    m_titleBar->move(1, 2);
    m_titleBar->setBackgroundColor(255,255, 255);
    m_titleBar->setTitleContent(QString("读书记录"));
    m_titleBar->setButtonType(ONLY_CLOSE_BUTTON);
    m_titleBar->setTitleWidth(this->width());
    m_titleBar->setWindowBorderWidth(2);
}

void kr_book::initFrame()
{
    ui->stackedWidget->setGeometry(10,40,this->width()-20,this->height()-50);
    ui->tableWidget_book->setGeometry(0,0,ui->stackedWidget->width(),ui->stackedWidget->height());

    QStringList headers;
    headers<<"序号"<<"书名"<<"当前页"<<"总页数"<<"进度"<<"开始时间"<<"结束时间";
    ui->tableWidget_book->setColumnCount(7);
    ui->tableWidget_book->setHorizontalHeaderLabels(headers);
    ui->tableWidget_book->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void kr_book::paintEvent(QPaintEvent *)
{
    // 绘制窗口白色背景色;
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRect(QRect(0, 0, this->width(), this->height()));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(255, 255, 255)));

    // 绘制窗口灰色边框;
    QPen pen(QColor(204, 204, 204));
    painter.setPen(pen);
    painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

    // 绘制窗口上方蓝条;
    QPainterPath pathHead;
    pathHead.setFillRule(Qt::WindingFill);
    pathHead.addRect(QRect(0, 0, this->width(), 3));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathHead, QBrush(QColor(15, 151, 255)));
}

int kr_book::getUserID()
{
    QSqlQuery *sql_query=new QSqlQuery();
    QString query;
    query=QString("SELECT UserID FROM k_comuser WHERE Name = :Name");
    sql_query->prepare(query);
    sql_query->bindValue(":Name",*account);
    sql_query->exec();
    while(sql_query->next()){
        return sql_query->value(0).toInt();
    }
}

void kr_book::loadBookRecord()
{
    QSqlQuery *sql_query=new QSqlQuery();
    QString query;
    query=QString("SELECT BookName,CurrentPage,TotalPage,StartTime,EndTime,BookRecordID FROM k_bookrecord WHERE UserID = :UserID");
    sql_query->prepare(query);
    sql_query->bindValue(":UserID",UserID);
    sql_query->exec();
    int rowindex;
    QTableWidgetItem *item;

    while(sql_query->next()){
        rowindex = ui->tableWidget_book->rowCount();
        ui->tableWidget_book->setRowCount( rowindex + 1);//总行数增加1
        item = new QTableWidgetItem (sql_query->value(5).toString());
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_book->setItem(rowindex, 0, item);

        item = new QTableWidgetItem (sql_query->value(0).toString());
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item->setToolTip(sql_query->value(0).toString());
        ui->tableWidget_book->setItem(rowindex, 1, item);

        item = new QTableWidgetItem (sql_query->value(1).toString());
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_book->setItem(rowindex, 2, item);

        item = new QTableWidgetItem (sql_query->value(2).toString());
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_book->setItem(rowindex, 3, item);

        item = new QTableWidgetItem (QString("%1%").arg((sql_query->value(1).toDouble()/sql_query->value(2).toDouble())*100));
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_book->setItem(rowindex, 4, item);

        item = new QTableWidgetItem (sql_query->value(3).toString().replace("T"," "));
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item->setToolTip(sql_query->value(3).toString().replace("T"," "));
        ui->tableWidget_book->setItem(rowindex, 5, item);

        item = new QTableWidgetItem (sql_query->value(4).toString().replace("T"," "));
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item->setToolTip(sql_query->value(4).toString().replace("T"," "));
        ui->tableWidget_book->setItem(rowindex, 6, item);
    }

    if(!isInitFinished)
        isInitFinished = true;
}

void kr_book::on_SearchBtn_clicked()
{
    getBookInfo();
}

void kr_book::loadBookInfo(QNetworkReply*)
{
    QStringList s;
    map = new QMap<QString,QString>();
    book_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    book_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (book_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = book_reply->readAll();
        QString string = QString::fromUtf8(bytes);
        string.remove("\\");
        string.remove("{");
        string.remove("}");
        string.remove("\"");
        s = string.split(",");
        foreach (QString str, s) {
            QStringList st = str.split(":");
            if(st.count()>=2){
                if(st.count()>=3){
                    map->insert(st.at(0),st.at(1)+":"+st.at(2));
                }else{
                    map->insert(st.at(0),st.at(1));
                }
            }
        }
        ui->label_Author->setText("作者: "+map->value("author"));
        ui->label_ISBN->setText("ISBN: "+map->value("isbn13"));
        ui->label_Pages->setText("页数: "+map->value("pages"));
        ui->label_Press->setText("出版社: "+map->value("publisher"));
        ui->label_PressDate->setText("出版年: "+map->value("pubdate"));
        ui->label_Price->setText("价格: "+map->value("price"));
        ui->label_Subheading->setText("副标题: "+map->value("subtitle"));
        ui->label_Translator->setText("译者: "+map->value("translator"));
        ui->label_Title->setText(map->value("title"));
        ui->label_Score->setText("豆瓣评分: "+map->value("average")+"分 ");
        ui->label_numRater->setText(map->value("numRaters")+"人评价");
        booklogo_netmanager = new QNetworkAccessManager;
        booklogo_netmanager->get(QNetworkRequest(QUrl(QString(map->value("medium")))));
        connect(booklogo_netmanager, SIGNAL(finished(QNetworkReply*)),this, SLOT(loadBookLogo(QNetworkReply*)));
    }
}

void kr_book::loadBookLogo(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();
        QPixmap pixmap;
        pixmap.loadFromData(bytes);
        ui->label_BookPicture->setPixmap(pixmap);
        ui->AcceptBtn->setEnabled(true);
   }
}

void kr_book::getBookInfo()
{
    bookUrl = "https://api.douban.com/v2/book/isbn/"+ui->lineEdit_isbn->text();
    book_netmanager = new QNetworkAccessManager;
    book_reply = book_netmanager->get(QNetworkRequest(bookUrl));
    connect(book_netmanager, SIGNAL(finished(QNetworkReply*)),this, SLOT(loadBookInfo(QNetworkReply*)));
}

void kr_book::on_AcceptBtn_clicked()
{
    isInitFinished = false;//关闭信号槽

    ui->stackedWidget->setCurrentIndex(0);
    int rowindex = ui->tableWidget_book->rowCount();
    ui->tableWidget_book->setRowCount( rowindex + 1);//总行数增加1
    QTableWidgetItem *item;

    item = new QTableWidgetItem (QString::number(rowindex+1, 10));
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_book->setItem(rowindex, 0, item);

    item = new QTableWidgetItem (map->value("title"));
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_book->setItem(rowindex, 1, item);

    item = new QTableWidgetItem ("0");
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_book->setItem(rowindex, 2, item);

    item = new QTableWidgetItem (map->value("pages"));
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_book->setItem(rowindex, 3, item);

    item = new QTableWidgetItem ("0%");
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_book->setItem(rowindex, 4, item);

    item = new QTableWidgetItem (QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget_book->setItem(rowindex, 5, item);
    QSqlQuery *sql_query=new QSqlQuery();
    QString query;
    query=QString("INSERT INTO k_bookrecord(UserID,BookName,CurrentPage,TotalPage,StartTime) VALUES (:UserID,:BookName,:CurrentPage,:TotalPage,:StartTime)");
    sql_query->prepare(query);
    sql_query->bindValue(":UserID",UserID);
    sql_query->bindValue(":BookName",ui->tableWidget_book->item(rowindex,1)->text().toUtf8());
    sql_query->bindValue(":CurrentPage",ui->tableWidget_book->item(rowindex,2)->text());
    sql_query->bindValue(":TotalPage",ui->tableWidget_book->item(rowindex,3)->text());
    sql_query->bindValue(":StartTime",ui->tableWidget_book->item(rowindex,5)->text());
    sql_query->exec();

    isInitFinished = true;
}

void kr_book::on_AddBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void kr_book::on_EditBtn_clicked()
{
    QSqlQuery *sql_query=new QSqlQuery();
    QString query;
    query=QString("UPDATE k_bookrecord SET CurrentPage = :CurrentPage WHERE BookRecordID = :BookRecordID");
    for(int i = 0;i<ui->tableWidget_book->rowCount();i++){
        sql_query->prepare(query);
        if(ui->tableWidget_book->item(i,2)->text()==""){
            return;//非数字不让更新
        }
        sql_query->bindValue(":CurrentPage",ui->tableWidget_book->item(i,2)->text().toInt());
        sql_query->bindValue(":BookRecordID",ui->tableWidget_book->item(i,0)->text());
        sql_query->exec();
    }
}

void kr_book::on_tableWidget_book_itemChanged(QTableWidgetItem *item)
{
    if(isInitFinished){
    if(item->column()==2){
        int i =item->row();
        if((!(ui->tableWidget_book->item(i,2)->text().isEmpty())&&!(ui->tableWidget_book->item(i,3)->text().isEmpty()))){
            ui->tableWidget_book->item(i,4)->setText(QString("%1%").arg((ui->tableWidget_book->item(i,2)->text().toDouble()/ui->tableWidget_book->item(i,3)->text().toDouble())*100));
        }else{
                return;
            }
        }
    }
}

void kr_book::on_BackBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
