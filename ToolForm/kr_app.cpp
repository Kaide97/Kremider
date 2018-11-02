#include "kr_app.h"
#include "ui_kr_app.h"
#include "BasicForm/kwindows.h"
#include <QPainter>
#include "kr_translate.h"
#include "BasicForm/kmessagebox.h"
#include "kr_class.h"
#include "kr_welcome.h"
#include "kr_book.h"
#include <QString>
#include <QNetworkAccessManager>
kr_app::kr_app(QWidget *parent) :
    Kwindows(parent),
    ui(new Ui::kr_app)
{
    ui->setupUi(this);
    initTitleBar();
    //子窗口都需要,解决长时间使用内存不释放问题
    setAttribute(Qt::WA_DeleteOnClose, true);
}

kr_app::~kr_app()
{
    delete ui;
}

void kr_app::initTitleBar()
{
    m_titleBar->move(1, 2);
    m_titleBar->setBackgroundColor(255,255, 255);
    m_titleBar->setTitleContent(QString("K管理应用"));
    m_titleBar->setButtonType(ONLY_CLOSE_BUTTON);
    m_titleBar->setTitleWidth(this->width());
    m_titleBar->setWindowBorderWidth(2);
}

void kr_app::paintEvent(QPaintEvent *event)
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

void kr_app::on_TransBtn_clicked()
{
    kr_translate *kt = new kr_translate();
    kt->show();
    this->close();
}

void kr_app::on_ClassBtn_clicked()
{
    kr_class *kc = new kr_class();
    kc->show();
    this->close();
}

void kr_app::on_WelcomeBtn_clicked()
{
    kr_welcome *kw = new kr_welcome();
    connect(this,SIGNAL(signalSendAccount(QString*)),kw,SLOT(getAccount(QString*)));
    emit signalSendAccount(account);
    kw->show();
}


void kr_app::on_PhoneNumSerachBtn_clicked()
{
    if(IsNumber(ui->PhoneNumSerachlineEdit->text())&&ui->PhoneNumSerachlineEdit->text().length()==11)
    {
        ui->PhoneNumSerachtextEdit->clear();
        ui->PhoneNumSerachtextEdit->setPlaceholderText("查询中...");
        QNetworkAccessManager *manager = new QNetworkAccessManager();
        connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(loadPhoneNumInfo(QNetworkReply*)));
        manager->get(*new QNetworkRequest(QString("https://way.jd.com/jisuapi/query4?shouji=%1&appkey=6a6abc99924a2fc7c61f1b1ca882981d").arg(ui->PhoneNumSerachlineEdit->text())));
    }else{
        Kmessagebox::showMyMessageBox(this,"输入错误","键入值长度或格式不正确",MESSAGE_WARNNING,BUTTON_OK);
    }
}

void kr_app::loadPhoneNumInfo(QNetworkReply *reply)
{
    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if (reply->error() == QNetworkReply::NoError)
    {
        //加载新闻标题
        QByteArray bytes = reply->readAll();
        QString phoneNumInfo = QString::fromUtf8(bytes);
        phoneNumInfo.replace("\"","");
        phoneNumInfo.replace("{","");
        phoneNumInfo.replace("}","");
        phoneNumInfo.replace(",",":");
        QStringList phoneNumInfoList = phoneNumInfo.split(":");
        if(phoneNumInfoList.length()>19)
        {
            ui->PhoneNumSerachtextEdit->setText("手机:"+phoneNumInfoList.at(13)+"\n号码归属地:"+phoneNumInfoList.at(15)+"\n所属:"+phoneNumInfoList.at(17)+phoneNumInfoList.at(19));

        }else{
            ui->PhoneNumSerachtextEdit->setText("手机号不正确,重新输入");
        }
    }
}


bool kr_app::IsNumber(const QString &src)
{
    for(int i=0;i<src.length();i++)
    {
        if(src.at(i)<'0'||src.at(i)>'9')
        return false;  //当不是数字字符时返回false
    }
    return true;
}

void kr_app::on_BookBtn_clicked()
{
    kr_book *kb = new kr_book();
    connect(this,SIGNAL(signalSendAccount(QString*)),kb,SLOT(getAccount(QString*)));
    emit signalSendAccount(account);
    kb->show();
}
