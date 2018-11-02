#ifndef FULLSCREEN_H
#define FULLSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QDesktopWidget>
#include <QApplication>
#include <QPixmap>
#include <QGraphicsDropShadowEffect>
#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QRubberBand>
#include <QDir>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QApplication>
#include <QClipboard>
#include <QCursor>
#include <QPalette>
#include <QDebug>
#include <QFont>
#include <QKeyEvent>
#include <QAction>
#include <math.h>
#include <QRegion>
#include <QBitmap>
#include "windows.h"
#include <QTimer>
struct MyRect
{
    QRect myRect_;  //矩形
    int distance;   //鼠标当前点到 所有边的距离之和，用于比较
};

class FullScreen : public QLabel
{
    Q_OBJECT
public:
    void calculateRectDistance(QRect rect);
    explicit FullScreen(QLabel *QLabel = 0);
    FullScreen(int index);
    ~FullScreen();
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseDoubleClickEvent(QMouseEvent);
    void keyPressEvent(QKeyEvent *e);
    void intiaView();
    void setSelectedLabel();
    void moveSelectLabel(int startX,int startY);
    void resizeSelectLabel(int x,int y,int changeWidth,int changeHeight);
    void setSelectedRound();
    void addButton();
    void deleteButton();
private slots:
    void savePng();
    void clipPng();
signals:

public slots:
    void drawMiniRec();
private:
    QTimer *timer;
    bool isPressed,isDraging,hasSelectArea,isCompleteSelect;
    int index;
    QPainter *painter;
    QPixmap *pixmap,*pix_flog,*fog;
    QPoint origin,endPoint;
    int g_nActScreenX;
    int g_nActScreenY;
    QPixmap image;
    QPixmap *rectPix;
    MyRect miniRect;
    //上一次鼠标位置 用于移动
    QPoint currentCursorPos;
    //大小的改变
    QSize sizeChange;
    //移动的大小
    QPoint movePos;
    //选择区域大小
    QSize SelectAreaSize;
    //根据输入鼠标位置设置鼠标类型以及返回状态
    int setCursorState(int x,int y);
    int CursorState;
    bool reverse;
};

#endif // FULLSCREEN_H
