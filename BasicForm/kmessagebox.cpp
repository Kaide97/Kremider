#include "kmessagebox.h"
#include "ui_kmessagebox.h"
#include <QPainter>
#include <QTimer>
#include <QCloseEvent>

Kmessagebox::Kmessagebox(QWidget *parent) :
    Kwindows(parent),
    ui(new Ui::Kmessagebox)
  , m_eventLoop(NULL)
  , m_chooseResult(ID_CANCEL)
{
    ui->setupUi(this);
    initWindow();
}

Kmessagebox::~Kmessagebox()
{
}

void Kmessagebox::initWindow()
{
    initTitleBar();
    loadStyleSheet("MyMessageBox/MyMessageBox");
    Qt::WindowFlags flags = this->windowFlags();
    this->setWindowFlags(flags | Qt::WindowStaysOnTopHint | Qt::Window);

    ui->inputContent->setVisible(false);

    connect(ui->pButtonOk, SIGNAL(clicked()), this, SLOT(onOkClicked()));
    connect(ui->pButtonCancel, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
}

// 初始化标题栏
void Kmessagebox::initTitleBar()
{
    m_titleBar->move(1, 2);
    m_titleBar->setWindowBorderWidth(2);
    m_titleBar->setBackgroundColor(255, 255, 255);
    m_titleBar->setButtonType(ONLY_CLOSE_BUTTON);
    m_titleBar->setTitleWidth(this->width());
}

void Kmessagebox::paintEvent(QPaintEvent *event)
{
    // 绘制窗口背景为白色
    QPainter painter(this);
    QPainterPath pathBack;
    pathBack.setFillRule(Qt::WindingFill);
    pathBack.addRect(QRect(0, 0, this->width(), this->height()));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathBack, QBrush(QColor(255, 255, 255)));

    // 绘制按钮部分灰色背景
    QPainterPath pathButtonBack;
    pathButtonBack.setFillRule(Qt::WindingFill);
    pathButtonBack.addRect(QRect(0, 110, this->width(), 48));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathButtonBack, QBrush(QColor(247, 247, 247)));

    // 绘制窗口灰色边框
    QPen pen(QColor(204, 204, 204));
    painter.setPen(pen);
    painter.drawRect(0, 0, this->width() - 1, this->height() - 1);

    // 绘制窗口上方蓝条
    QPainterPath pathHead;
    pathHead.setFillRule(Qt::WindingFill);
    pathHead.addRect(QRect(0, 0, this->width(), 2));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathHead, QBrush(QColor(15, 151, 255)));

    return QWidget::paintEvent(event);
}

void Kmessagebox::setWindowTitle(QString title , int titleFontSize)
{
    m_titleBar->setTitleContent(title, titleFontSize);
}

void Kmessagebox::setContentText(QString contentText)
{
    ui->MessageContent->setText(contentText);
}

void Kmessagebox::setMessageType(MessageType messageType)
{
    switch (messageType)
    {
    case MESSAGE_INFORMATION:
        ui->MessageIcon->setPixmap(QPixmap(":/Resources/MyMessageBox/information.png"));
        break;
    case MESSAGE_WARNNING:
        ui->MessageIcon->setPixmap(QPixmap(":/Resources/MyMessageBox/warnning.png"));
        break;
    case MESSAGE_QUESTION:
        ui->MessageIcon->setPixmap(QPixmap(":/Resources/MyMessageBox/question.png"));
        break;
    case MESSAGE_INPUT:
        ui->MessageIcon->setVisible(false);
        ui->inputContent->setVisible(true);
    default:
        break;
    }
}

void Kmessagebox::setButtonType(MessageButtonType buttonType)
{
    switch (buttonType)
    {
    case BUTTON_OK:
    {
        ui->pButtonOk->setText("确定");
        ui->pButtonCancel->setVisible(false);
    }
        break;
    case BUTTON_OK_AND_CANCEL:
    {
        ui->pButtonOk->setText("确定");
        ui->pButtonCancel->setText("取消");
    }
        break;
    case BUTTON_AUTO_OK_AND_CANEL:
    {
        ui->pButtonOk->setMinimumWidth(148);
        ui->pButtonCancel->setText("取消");
        WorkerThread *wt = new WorkerThread();
        wt->initTimeCount(5);
        connect(wt,SIGNAL(resultReady(int)),this,SLOT(onAutoOKStart(int)));
        wt->start();
    }
        break;
    default:
        break;
    }
}

void Kmessagebox::setMessageContent(QString messageContent)
{
    ui->MessageContent->setText(messageContent);
}

int Kmessagebox::showMyMessageBox(QWidget* parent, const QString &title, const QString &contentText, MessageType messageType, MessageButtonType messageButtonType, bool isModelWindow)
{
    Kmessagebox *myMessageBox = new Kmessagebox(parent);
    myMessageBox->setWindowTitle(title);
    myMessageBox->setContentText(contentText);
    myMessageBox->setMessageType(messageType);
    myMessageBox->setButtonType(messageButtonType);
    if (isModelWindow)
    {
        // 设置为模态窗口时，参数parent必须设置父窗口指针，否则模态设置无效;
        //设置为模态窗口时，参数parent必须有父窗口指针，否则无效
        // 因为 Qt::WindowModal 参数只对父窗口有效，如果想要模态对全局窗口都有效可以设置 Qt::ApplicationModal
        //因为Qt::windowModal 参数只对父窗口有效，如果想要模态对全局窗口有效可设置Qt::ApplicationModal
        return myMessageBox->exec();
    }
    else
    {
        myMessageBox->show();
    }

    return -1;
}

int Kmessagebox::exec()
{
    // 而exec()方法就是直接设置窗口显示为模态，并且窗口关闭结束后返回用户选择结果(按了确定还是取消按钮);
    // 而exec()方法就是设置窗口显示为模态，关闭窗口返回用户选择结果
    //show不返回东西，不设置窗口模态属性，需要设置setWindowModality()
    // 可继承QDialog,自有exec

    //设置窗口模态，也可设置应用程序及模态 Qt::ApplicationModal
    this->setWindowModality(Qt::ApplicationModal);
    show();
    //使用事件循环QEventLoop阻止进行下一步模仿模态
    m_eventLoop = new QEventLoop(this);
    m_eventLoop->exec();

    return m_chooseResult;
}

void Kmessagebox::onOkClicked()
{
    m_chooseResult = ID_OK;
    close();
}

void Kmessagebox::onCancelClicked()
{
    m_chooseResult = ID_CANCEL;
    close();
}

void Kmessagebox::onAutoOKStart(int TimerCount)
{
    if(TimerCount>0){
        ui->pButtonOk->setText(QString("%1").arg(TimerCount)+"s 后自动确认");
    }else{
        m_chooseResult = ID_OK;
        close();
    }
}

void Kmessagebox::closeEvent(QCloseEvent *event)
{
    //关闭窗口时结束时间循环，在exec(）方法返回结果
    if (m_eventLoop != NULL)
    {
        m_eventLoop->exit();
    }
    event->accept();
}
