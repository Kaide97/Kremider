#include "kr_about.h"
#include "ui_kr_about.h"
#include "BasicForm/kwindows.h"
#include <QPainter>
#include <QDesktopServices>
#include <QPropertyAnimation>
#include <QUrl>
kr_about::kr_about(QWidget *parent) :
    Kwindows(parent),
    ui(new Ui::kr_about)
{
    ui->setupUi(this);
    m_titleBar->hide();
    initFirst();
    initAllSignalSlot();
    //子窗口都需要,解决长时间使用内存不释放问题
    setAttribute(Qt::WA_DeleteOnClose, true);
}

kr_about::~kr_about()
{
    delete ui;
}

void kr_about::callWeiboSite()
{
    QDesktopServices::openUrl(QUrl("http://weibo.com/js51"));
}

void kr_about::changePage()
{
    if(ui->stackedWidget->currentIndex()==0)
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->HeaderLabel->move(-100,ui->HeaderLabel->y());
        ui->NameLabel->move(410,ui->NameLabel->y());
        ui->ChangePageBtn->setText("返回");
        initSecond();
    }else{
        ui->stackedWidget->setCurrentIndex(0);
        ui->textBrowser->move(ui->textBrowser->x(),370);
        initFirst();
        ui->ChangePageBtn->setText("更新日志");
    }
}

void kr_about::goPersonalWebSite()
{
    QDesktopServices::openUrl(QUrl(QString("www.kaispace.cn")));
}

void kr_about::paintEvent(QPaintEvent *event)
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

void kr_about::initFirst()
{
    QPropertyAnimation *animation = new QPropertyAnimation(ui->HeaderLabel,"geometry");
    animation->setDuration(500);
    animation->setStartValue(QRect(ui->HeaderLabel->x(),ui->HeaderLabel->y(),ui->HeaderLabel->width(),ui->HeaderLabel->height()));
    animation->setEndValue(QRect(ui->HeaderLabel->x()+190,ui->HeaderLabel->y(),ui->HeaderLabel->width(),ui->HeaderLabel->height()));
    animation->start();
    QPropertyAnimation *lanimation = new QPropertyAnimation(ui->NameLabel,"geometry");
    lanimation->setDuration(500);
    lanimation->setStartValue(QRect(ui->NameLabel->x(),ui->NameLabel->y(),ui->NameLabel->width(),ui->NameLabel->height()));
    lanimation->setEndValue(QRect(ui->NameLabel->x()-200,ui->NameLabel->y(),ui->NameLabel->width(),ui->NameLabel->height()));
    lanimation->start();
}

void kr_about::initSecond()
{
    QPropertyAnimation *animation = new QPropertyAnimation(ui->textBrowser,"geometry");
    animation->setDuration(500);
    animation->setStartValue(QRect(ui->textBrowser->x(),ui->textBrowser->y(),ui->textBrowser->width(),ui->textBrowser->height()));
    animation->setEndValue(QRect(ui->textBrowser->x(),ui->textBrowser->y()-360,ui->textBrowser->width(),ui->textBrowser->height()));
    animation->start();
}

void kr_about::initAllSignalSlot()
{
      connect(ui->SinaBtn,SIGNAL(clicked()),this,SLOT(callWeiboSite()));
      connect(ui->CloseBtn,SIGNAL(clicked()),this,SLOT(close()));
      connect(ui->ChangePageBtn,SIGNAL(clicked()),this,SLOT(changePage()));
      connect(ui->PersonalWebSiteBtn,SIGNAL(clicked()),this,SLOT(goPersonalWebSite()));
}
