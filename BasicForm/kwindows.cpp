#include "kwindows.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QPainter>
#include <QFile>
#include <QStyleOption>
#include <QMouseEvent>

Kwindows::Kwindows(QWidget *parent)
    : QWidget(parent)
{
    // Qt::WindowMinimizeButtonHint 属性设置在最小化后，点击任务栏能恢复
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    // 设置窗口背景透明 | 关闭窗口释放资源
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    // 初始化标题栏
    initTitleBar();
}


Kwindows::~Kwindows()
{
}

void Kwindows::initTitleBar()
{
    m_titleBar = new Ktitlebar(this);
    m_titleBar->move(0, 0);

    connect(m_titleBar, SIGNAL(signalButtonMinClicked()), this, SLOT(onButtonMinClicked()));
    connect(m_titleBar, SIGNAL(signalButtonRestoreClicked()), this, SLOT(onButtonRestoreClicked()));
    connect(m_titleBar, SIGNAL(signalButtonMaxClicked()), this, SLOT(onButtonMaxClicked()));
    connect(m_titleBar, SIGNAL(signalButtonCloseClicked()), this, SLOT(onButtonCloseClicked()));

}

void Kwindows::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    //设置背景色

    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(236, 241, 245)));

//	return QWidget::paintEvent(event);
}

void Kwindows::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/Resources/" + sheetName + ".css");
    file.open(QFile::ReadOnly);
    if (file.isOpen())
    {
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
    }
}

void Kwindows::getAccount(QString *userAccount)
{
    account=userAccount;
}

void Kwindows::onButtonMinClicked()
{
    if (Qt::Tool == (windowFlags() & Qt::Tool))
    {
        hide();
    }
    else
    {
        showMinimized();
    }
}

void Kwindows::onButtonRestoreClicked()
{
    QPoint windowPos;
    QSize windowSize;
    m_titleBar->getRestoreInfo(windowPos, windowSize);
    this->setGeometry(QRect(windowPos, windowSize));
}

void Kwindows::onButtonMaxClicked()
{
    m_titleBar->saveRestoreInfo(this->pos(), QSize(this->width(), this->height()));
    QRect desktopRect = QApplication::desktop()->availableGeometry();
    QRect FactRect = QRect(desktopRect.x() - 3, desktopRect.y() - 3, desktopRect.width() + 6, desktopRect.height() + 6);
    setGeometry(FactRect);
}

void Kwindows::onButtonCloseClicked()
{
    // 如果设置主窗口为Qt::tool
    // 需要qApp->quit()才能推出
    // 如果只是子窗口，那就close（）就行
    // 	if (Qt::Tool == (windowFlags() & Qt::Tool))
    // 	{
    // 		qApp->quit();
    // 	}
    // 	else
    //{
        this->close();
    //}
}

void Kwindows::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        isDrag = true;
        m_position = e->globalPos() - this->pos();
        e->accept();
    }
}

void Kwindows::mouseMoveEvent(QMouseEvent *e)
{
    if(isDrag && (e->buttons() && Qt::LeftButton))
    {
        move(e->globalPos() - m_position);
        e->accept();
    }
}

void Kwindows::mouseReleaseEvent(QMouseEvent*)
{
    isDrag = false;
}
