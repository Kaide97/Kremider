#ifndef KTITLEBAR_H
#define KTITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

enum ButtonType
{
    MIN_BUTTON=0,//最小化按钮和关闭按钮
    MIN_MAX_BUTTON,//最小化最大化和关闭
    ONLY_CLOSE_BUTTON//只有关闭按钮
};

class Ktitlebar : public QWidget
{
    Q_OBJECT

public:
    explicit Ktitlebar(QWidget *parent = 0);
    ~Ktitlebar();
    // 设置标题栏背景颜色
    void setBackgroundColor(int r,int g,int b);
    // 设置标题栏图标,图标默认25X25
    void setTitleIcon(QString filePath,QSize IconSize = QSize(25,25));
    // 设置标题内容,字体默认9PX
    void setTitleContent(QString titleContent,int titleFontSize = 9);
    // 设置标题栏长度
    void setTitleWidth(int width);
    // 设置标题栏上按钮类型
    void setButtonType(ButtonType buttonType);
    // 设置标题栏是否开启走马灯效果
    void setTitleRoll();
    // 设置窗口边框宽度
    void setWindowBorderWidth(int borderWidth);

    // 保存/获取 最大化前窗口的位置及大小
    void saveRestoreInfo(const QPoint point,const QSize size);
    void getRestoreInfo(QPoint &point,QSize &size);

private:
    void paintEvent(QPaintEvent *event);//绘图事件函数
    void mouseDoubleClickEvent(QMouseEvent *event);//标题栏双击事件函数
    void mousePressEvent(QMouseEvent *event);//移动窗口事件1/3
    void mouseMoveEvent(QMouseEvent *event);//移动窗口事件2/3
    void mouseReleaseEvent(QMouseEvent *event);//移动窗口事件3/3

    //控件初始化函数
    void initControl();
    //信号槽绑定函数
    void initConnection();
    //加载样式文件
    void loadStyleSheet(const QString &sheetName);

signals:
    //按钮触发信号
    void signalButtonMinClicked();
    void signalButtonRestoreClicked();
    void signalButtonMaxClicked();
    void signalButtonCloseClicked();

private slots:
    //按钮信号槽
    void onButtonMinClicked();
    void onButtonMaxClicked();
    void onButtonRestoreClicked();
    void onButtonCloseClicked();
    void onRollTitle();

private:
    QLabel* m_pIcon;                //标题图标
    QLabel* m_pTitleContent;        //标题内容
    QPushButton* m_pButtonMin;      //最小化按钮
    QPushButton* m_pButtonMax;      //最大化按钮
    QPushButton* m_pButtonRestore;  //还原按钮
    QPushButton* m_pButtonClose;    //关闭按钮

    //标题栏背景色
    int m_colorR;
    int m_colorG;
    int m_colorB;

    //最大化最小化存储变量
    QPoint m_restorePos;
    QSize m_restoreSize;
    //移动窗口的变量
    bool m_isPressed;
    QPoint m_startMovePos;
    //标题走马灯计时器
    QTimer m_titleRollTimer;
    // 标题栏内容
    QString m_titleContent;
    // 按钮类型
    ButtonType m_buttonType;
    //窗口边框宽度
    int m_windowBorderWidth;
};

#endif // KTITLEBAR_H
