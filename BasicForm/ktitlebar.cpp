#include "ktitlebar.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QFile>
#include <QMouseEvent>

#define BUTTON_HEIGHT 30    //按钮高度
#define BUTTON_WIDTH 30     //按钮宽度
#define TITLE_HEIGHT 30     //标题栏高度
Ktitlebar::Ktitlebar(QWidget *parent) :
    QWidget(parent)
,m_colorR(125)
,m_colorG(125)//初始化颜色，默认灰色
,m_colorB(125)
,m_isPressed(false)
,m_buttonType(MIN_MAX_BUTTON)//默认三种按钮都有
,m_windowBorderWidth(0)

{
    //初始化
    initControl();
    initConnection();
    loadStyleSheet("MyTitle");//可选可不选
}

Ktitlebar::~Ktitlebar()
{

}

//初始化控件
void Ktitlebar::initControl()
{
    m_pIcon = new QLabel;
    m_pTitleContent = new QLabel;

    m_pButtonMin = new QPushButton;
    m_pButtonRestore = new QPushButton;
    m_pButtonMax = new QPushButton;
    m_pButtonClose = new QPushButton;

    m_pButtonMin->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonRestore->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonMax->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));
    m_pButtonClose->setFixedSize(QSize(BUTTON_WIDTH, BUTTON_HEIGHT));

    m_pTitleContent->setObjectName("TitleContent");
    m_pButtonMin->setObjectName("ButtonMin");
    m_pButtonRestore->setObjectName("ButtonRestore");
    m_pButtonMax->setObjectName("ButtonMax");
    m_pButtonClose->setObjectName("ButtonClose");

    m_pButtonMin->setToolTip(QString("最小化"));
    m_pButtonRestore->setToolTip(QString("还原"));
    m_pButtonMax->setToolTip(QString("最大化"));
    m_pButtonClose->setToolTip(QString("关闭"));

    QHBoxLayout* mylayout = new QHBoxLayout(this);
    mylayout->addWidget(m_pIcon);
    mylayout->addWidget(m_pTitleContent);

    mylayout->addWidget(m_pButtonMin);
    mylayout->addWidget(m_pButtonRestore);
    mylayout->addWidget(m_pButtonMax);
    mylayout->addWidget(m_pButtonClose);

    mylayout->setContentsMargins(5, 0, 0, 0);
    mylayout->setSpacing(0);

    m_pTitleContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->setFixedHeight(TITLE_HEIGHT);
    this->setWindowFlags(Qt::FramelessWindowHint);
}
//信号槽绑定
void Ktitlebar::initConnection()
{
    connect(m_pButtonMin, SIGNAL(clicked()), this, SLOT(onButtonMinClicked()));
    connect(m_pButtonRestore, SIGNAL(clicked()), this, SLOT(onButtonRestoreClicked()));
    connect(m_pButtonMax, SIGNAL(clicked()), this, SLOT(onButtonMaxClicked()));
    connect(m_pButtonClose, SIGNAL(clicked()), this, SLOT(onButtonCloseClicked()));
}
//设置背景颜色
void Ktitlebar::setBackgroundColor(int r, int g, int b)
{
    m_colorR =r;
    m_colorG =g;
    m_colorB =b;
    //调用update()刷新paintevent重绘标题
    update();
}

//设置标题栏图标
void Ktitlebar::setTitleIcon(QString filePath, QSize IconSize)
{
    QPixmap titleIcon(filePath);
    m_pIcon->setPixmap(titleIcon.scaled(IconSize));
}

//设置标题内容
void Ktitlebar::setTitleContent(QString titleContent, int titleFontSize)
{
    //设置标题字体大小
    QFont font = m_pTitleContent->font();
    font.setPointSize(titleFontSize);
    m_pTitleContent->setFont(font);
    //设置标题内容
    m_pTitleContent->setText(titleContent);
    m_titleContent = titleContent;
}

// 标题栏长度
void Ktitlebar::setTitleWidth(int width)
{
    this->setFixedWidth(width);
}

// 设置标题栏按钮
void Ktitlebar::setButtonType(ButtonType buttonType)
{
    m_buttonType = buttonType;

    switch (buttonType)
    {
    case MIN_BUTTON:
    {
                       m_pButtonRestore->setVisible(false);
                       m_pButtonMax->setVisible(false);
    }
        break;
    case MIN_MAX_BUTTON:
    {
                        m_pButtonRestore->setVisible(false);
    }
        break;
    case ONLY_CLOSE_BUTTON:
    {
                        m_pButtonMin->setVisible(false);
                        m_pButtonRestore->setVisible(false);
                        m_pButtonMax->setVisible(false);
    }
        break;
    default:
        break;
    }
}

// 设置标题栏跑马灯效果
// 默认关闭
void Ktitlebar::setTitleRoll()
{
    connect(&m_titleRollTimer, SIGNAL(timeout()), this, SLOT(onRollTitle()));
    m_titleRollTimer.start(200);
}

// 设置标题栏宽度
void Ktitlebar::setWindowBorderWidth(int borderWidth)
{
    m_windowBorderWidth = borderWidth;
}

// 保存窗口最大化前的位置和大小
void Ktitlebar::saveRestoreInfo(const QPoint point, const QSize size)
{
    m_restorePos = point;
    m_restoreSize = size;
}

// 获取窗口最大化前的位置和大小
void Ktitlebar::getRestoreInfo(QPoint& point, QSize& size)
{
    point = m_restorePos;
    size = m_restoreSize;
}

// 绘制标题背景色
void Ktitlebar::paintEvent(QPaintEvent *event)
{
    //设置背景色
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRoundedRect(QRect(0, 0, this->width(), this->height()), 3, 3);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(m_colorR, m_colorG, m_colorB)));

    // 窗口变化时随着变化
    // 减去窗口设置的不同宽度
    // 如果窗口有边框要设置m_windowborder,默认为0
    if (this->width() != (this->parentWidget()->width() - m_windowBorderWidth))
    {
        this->setFixedWidth(this->parentWidget()->width() - m_windowBorderWidth);
    }
    QWidget::paintEvent(event);
}

// 双击响应事件，主要是实现双击标题栏进行最大化最小化操作
void Ktitlebar::mouseDoubleClickEvent(QMouseEvent *event)
{
    //存在最大化、最小化按钮时双击才生效
    if (m_buttonType == MIN_MAX_BUTTON)
    {
        //通过最大化按钮状态判断当前窗口的最大化还是原始大小
        // 可通过设置其他变量判断
        if (m_pButtonMax->isVisible())
        {
            onButtonMaxClicked();
        }
        else
        {
            onButtonRestoreClicked();
        }
    }

    return QWidget::mouseDoubleClickEvent(event);
}

//通过三个鼠标事件实现移动窗口
void Ktitlebar::mousePressEvent(QMouseEvent *event)
{
    if (m_buttonType == MIN_MAX_BUTTON)
    {
        // 在窗口最大化时候禁止移动
        if (m_pButtonMax->isVisible())
        {
            m_isPressed = true;
            m_startMovePos = event->globalPos();
        }
    }
    else
    {
        m_isPressed = true;
        m_startMovePos = event->globalPos();
    }

    return QWidget::mousePressEvent(event);
}

void Ktitlebar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPressed)
    {
        QPoint movePoint = event->globalPos() - m_startMovePos;
        QPoint widgetPos = this->parentWidget()->pos();
        m_startMovePos = event->globalPos();
        this->parentWidget()->move(widgetPos.x() + movePoint.x(), widgetPos.y() + movePoint.y());
    }
    return QWidget::mouseMoveEvent(event);
}

void Ktitlebar::mouseReleaseEvent(QMouseEvent *event)
{
    m_isPressed = false;
    return QWidget::mouseReleaseEvent(event);
}

// 加载本地样式文件
void Ktitlebar::loadStyleSheet(const QString &sheetName)
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

// 按钮操作响应槽
void Ktitlebar::onButtonMinClicked()
{
    emit signalButtonMinClicked();
}

void Ktitlebar::onButtonRestoreClicked()
{
    m_pButtonRestore->setVisible(false);
    m_pButtonMax->setVisible(true);
    emit signalButtonRestoreClicked();
}

void Ktitlebar::onButtonMaxClicked()
{
    m_pButtonMax->setVisible(false);
    m_pButtonRestore->setVisible(true);
    emit signalButtonMaxClicked();
}

void Ktitlebar::onButtonCloseClicked()
{
    emit signalButtonCloseClicked();
}

// 该方法为开启走马灯
void Ktitlebar::onRollTitle()
{
    static int nPos = 0;
    QString titleContent = m_titleContent;
    // 当截取位置比字符串长时，从新开始
    if (nPos > titleContent.length())
        nPos = 0;

    m_pTitleContent->setText(titleContent.mid(nPos));//移动标题栏中心位置来实现
    nPos++;
}
