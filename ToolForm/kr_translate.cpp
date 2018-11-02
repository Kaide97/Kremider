/*缓存释放在initFrame中*/
#include "kr_translate.h"
#include "ui_kr_translate.h"
#include <QPainter>
#include <QMouseEvent>
kr_translate::kr_translate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::kr_translate)
{
    ui->setupUi(this);
    initFrame();
}

kr_translate::~kr_translate()
{
    delete ui;
}

void kr_translate::paintEvent(QPaintEvent *event)
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

void kr_translate::loadNavigate()
{
  sUrl = ui->UrllineEdit->text().trimmed();
  ui->webWidget->dynamicCall("Navigate(const QString&)",sUrl);
}

void kr_translate::initFrame()
{
    setWindowFlags(Qt::FramelessWindowHint);
    //子窗口都需要,解决长时间使用内存不释放问题
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAutoFillBackground(false);
    //设置ActiveX控件为IEMicrosoft Web Browser
    //设置ActiveX控件的id，最有效的方式就是使用UUID
    //此处的{8856F961-340A-11D0-A96B-00C04FD705A2}就是Microsoft Web Browser控件的UUID
    ui->webWidget->setFocusPolicy(Qt::StrongFocus);//设置控件接收键盘焦点的方式：鼠标单击、Tab键
    ui->webWidget->setProperty("DisplayAlerts",false); //不显示任何警告信息。
    ui->webWidget->setProperty("DisplayScrollBars",true); // 显示滚动条
    ui->UrllineEdit->setText("http://fanyi.baidu.com/");
    loadNavigate();
}

void kr_translate::on_CloseBtn_clicked()
{
    this->close();
}

void kr_translate::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        isDrag = true;
        m_position = e->globalPos() - this->pos();
        e->accept();
    }
}

void kr_translate::mouseMoveEvent(QMouseEvent *e)
{
    if(isDrag && (e->buttons() && Qt::LeftButton))
    {
        move(e->globalPos() - m_position);
        e->accept();
    }
}

void kr_translate::mouseReleaseEvent(QMouseEvent*)
{
    isDrag = false;
}

void kr_translate::on_GOButton_clicked()
{
    loadNavigate();
}

void kr_translate::receiveUrl(QString url)
{
    sUrl=url;
    ui->UrllineEdit->setText(sUrl);
    loadNavigate();
}
