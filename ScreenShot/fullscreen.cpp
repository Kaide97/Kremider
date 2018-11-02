#include "fullscreen.h"
#include "QFileDialog"
#include <QPushButton>
#include <QVBoxLayout>
#include <QScreen>
std::vector<QRect> allWindowRect;     //用于存储所有的窗口
std::vector<HWND> allWindowHwnd;      //用于存储所有的窗口句柄
std::vector<MyRect> myRectRestlt;     // 找到所有包含 鼠标当前移动点的矩形，并保存其到各边的距离之和。


//声明回调函数
bool CALLBACK MyEnumWindowsProc(HWND hwnd,LPARAM lParam);
FullScreen::FullScreen(QLabel *parent) : QLabel(parent)
{
    this->index = 0;

    isPressed = false;
    isDraging = false;
    hasSelectArea = false;
    isCompleteSelect = false;
    reverse = false;
    intiaView();
    this->setPixmap(*pix_flog);
    ::EnumWindows((WNDENUMPROC)MyEnumWindowsProc,0);

    this->setMouseTracking(true);
    this->show();
    this->setFocusPolicy(Qt::StrongFocus);
}


void FullScreen::drawMiniRec(){
    //rectPix = new QPixmap;
    if(currentCursorPos==cursor().pos())
    {
        return;
    }
    else{
        currentCursorPos=cursor().pos();
    }
    myRectRestlt.clear();
        if(isPressed == false){
                  QCursor cursor;
                           for (std::vector<QRect>::iterator it = allWindowRect.begin()+1;it != allWindowRect.end();it++)
                           {
                               if (it->contains(cursor.pos().x(),cursor.pos().y()))
                               {
                                   calculateRectDistance(*it);
                               }
                           }
                           MyRect tempMinRect;
                           for(std::vector<MyRect>::iterator it = myRectRestlt.begin();it != myRectRestlt.end();it++)
                           {
                               if (it->distance < tempMinRect.distance)  //找到最小的矩形
                               {
                                   tempMinRect = *it;      //
                               }
                           }
                           miniRect = tempMinRect;  //找到最小的矩形
                           QPixmap pix = *pixmap;
                           QPainter painter(&pix);
                           painter.drawPixmap(0,0,*fog);
                           int x = miniRect.myRect_.topLeft().x();
                           int y = miniRect.myRect_.topLeft().y();
                           int width = miniRect.myRect_.width();
                           int height = miniRect.myRect_.height();
                           QPixmap pixmap_ =  pixmap->copy(x,y,width,height);
                           painter.setPen(QPen(QColor(Qt::white),8));
                           painter.drawText(x+5,y-5,QString("%1 x %2").arg(width).arg(height));
                           painter.setPen(QPen(QColor(46,159,238),8));
                           painter.drawPoint(x,y);
                           painter.drawPoint(x+width/2,y);
                           painter.drawPoint(x+width,y);
                           painter.drawPoint(x,y+height/2);
                           painter.drawPoint(x+width,y+height/2);
                           painter.drawPoint(x,y+height);
                           painter.drawPoint(x+width/2,y+height);
                           painter.drawPoint(x+width,y+height);
                           painter.setPen(QPen(QColor(46,159,238),3));
                           painter.drawRect(QRect(x,y,width,height));
                           painter.setPen(QPen(QColor(46,159,238),3));
                           painter.drawPixmap(x,y,pixmap_);
                           painter.drawRect(x, y, width, height);
                           this->setPixmap(pix);
                          // *rectPix =  pixmap->copy(miniRect.myRect_.topLeft().x(),miniRect.myRect_.topLeft().y(),miniRect.myRect_.width(),miniRect.myRect_.height());
        }
}

int FullScreen::setCursorState(int x, int y)
{
    QPoint startPoint = origin.x() < endPoint.x() ? origin:endPoint;
    QPoint completePoint = origin.x() < endPoint.x() ? endPoint:origin;
    const int ChangeCursorSpan = 15;//鼠标与更改线的匹配范围
    bool inLeftX = abs(x-startPoint.x())<ChangeCursorSpan;
    bool inRightX = abs(x-(completePoint.x()-10))<ChangeCursorSpan;
    bool inTopY = abs(y-startPoint.y())<ChangeCursorSpan;
    bool inBottomY = abs(y-completePoint.y())<ChangeCursorSpan;
    //更改鼠标类型 0是会影响区域的startPoint 1是影响endPoint
    if((inLeftX&&inTopY)||(inRightX&&inBottomY))
    {
        setCursor(Qt::SizeFDiagCursor);
        if(inLeftX&&inTopY){
            return 0;
        }else{
            return 1;
        }
    }else if((inLeftX&&inBottomY)||(inRightX&&inTopY)){
        setCursor(Qt::SizeBDiagCursor);
        return 0;
    }else if(inLeftX||inRightX){
        setCursor(Qt::SizeHorCursor);
        if(inLeftX){
            return 0;
        }else{
            return 1;
        }
    }else if(inTopY||inBottomY){
        setCursor(Qt::SizeVerCursor);
        if(inTopY){
            return 0;
        }else{
            return 1;
        }
    }else{
        setCursor(Qt::SizeAllCursor);
        return 2;
    }
}
void FullScreen::intiaView(){
  //初始化鼠标的状态为左键未点下
  setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
  QDesktopWidget* desktopWidget = QApplication::desktop();
  //获取可用桌面大小
  //QRect deskRect = desktopWidget->availableGeometry();
  //获取设备屏幕大小
  QRect screenRect = desktopWidget->screenGeometry();
  g_nActScreenX = screenRect.width();
  g_nActScreenY = screenRect.height();
  this->resize(g_nActScreenX,g_nActScreenY);

  this->setFocusPolicy(Qt::StrongFocus);
  pixmap = new QPixmap;
  QScreen *screen = QGuiApplication::primaryScreen();
  *pixmap = screen->grabWindow(QApplication::desktop()->winId());

  //设置雾层
  pix_flog = new QPixmap(*pixmap);
  fog = new QPixmap(g_nActScreenX,g_nActScreenY);
  //QPixmap fog(g_nActScreenX,g_nActScreenY);
  fog->fill(QColor(110,110,110,192));
  painter = new QPainter(pix_flog);

  painter->setFont(QFont("黑体",23));
  QPen pen(QColor(0,0,0));
  pen.setWidth(20);

  painter->drawPixmap(0,0,*fog);
  painter->setPen(QPen(QColor(0,0,0)));
  //  this->setFocusPolicy();
}

//回调函数
bool CALLBACK MyEnumWindowsProc(HWND hwnd,LPARAM lParam)
{
    if (::IsWindow(hwnd) && ::IsWindowVisible(hwnd) && ::GetParent(hwnd)==NULL)//GetParent(hwnd)==NULL不捕捉控件句柄
    {
        RECT tempRect;
        QRect tempQRect;
        ::GetWindowRect(hwnd,&tempRect);

        tempQRect.setTopLeft(QPoint(tempRect.left,tempRect.top));
        tempQRect.setBottomRight(QPoint(tempRect.right,tempRect.bottom));

        allWindowRect.push_back(tempQRect);
        allWindowHwnd.push_back(hwnd);

        ::EnumChildWindows(hwnd,(WNDENUMPROC)MyEnumWindowsProc,0);
    }
    return true;
}

void FullScreen::calculateRectDistance(QRect rect)
{
    int dis = rect.width() + rect.height();
    MyRect tempMyRect;
    tempMyRect.myRect_ = rect;
    tempMyRect.distance = dis;
    //添加进入
    myRectRestlt.push_back(tempMyRect);
}
FullScreen::FullScreen(int index){
    this->index = index;
    intiaView();
    if(index == 1){
    painter->drawText(g_nActScreenX / 2 - 120,g_nActScreenY / 2 - 50,tr("已截取全屏，点击屏幕退出"));
    this->setPixmap(*pix_flog);
    this->show();
    }
    else if(index == 2){
        this->setPixmap(*pix_flog);
        this->show();
    }
}

void FullScreen::setSelectedLabel(){

    int width = abs(endPoint.x() - origin.x());
    int height = abs(endPoint.y() - origin.y());
    int x = origin.x() < endPoint.x() ? origin.x():endPoint.x();
    int y = origin.y() < endPoint.y() ? origin.y():endPoint.y();
    QPixmap pix = *pixmap;
    QPainter painter(&pix);

    painter.drawPixmap(0,0,*fog);
    if(width!=0 && height != 0){
        painter.drawPixmap(x,y,pixmap->copy(x,y,width,height));
        painter.setPen(QPen(QColor(Qt::white),8));
        painter.drawText(x+5,y-5,QString("%1 x %2").arg(width).arg(height));
        painter.setPen(QPen(QColor(46,159,238),8));
        painter.drawPoint(x,y);
        painter.drawPoint(x+width/2,y);
        painter.drawPoint(x+width,y);
        painter.drawPoint(x,y+height/2);
        painter.drawPoint(x+width,y+height/2);
        painter.drawPoint(x,y+height);
        painter.drawPoint(x+width/2,y+height);
        painter.drawPoint(x+width,y+height);
        painter.setPen(QPen(QColor(46,159,238),3));
        painter.drawRect(QRect(x,y,width,height));
        this->setPixmap(pix);
        SelectAreaSize = QSize(width,height);
    }
}

void FullScreen::moveSelectLabel(int startX, int startY)
{
    int width = SelectAreaSize.width();
    int height = SelectAreaSize.height();
    int x = origin.x() < endPoint.x() ? origin.x():endPoint.x();
    int y = origin.y() < endPoint.y() ? origin.y():endPoint.y();
    x+=startX;
    y+=startY;
    movePos = QPoint(startX,startY);
    QPixmap pix = *pixmap;
    QPainter painter(&pix);

    painter.drawPixmap(0,0,*fog);
    if(width!=0 && height != 0){
        painter.drawPixmap(x,y,pixmap->copy(x,y,width,height));
        painter.setPen(QPen(QColor(Qt::white),8));
        painter.drawText(x+5,y-5,QString("%1 x %2").arg(width).arg(height));
        painter.setPen(QPen(QColor(46,159,238),8));
        painter.drawPoint(x,y);
        painter.drawPoint(x+width/2,y);
        painter.drawPoint(x+width,y);
        painter.drawPoint(x,y+height/2);
        painter.drawPoint(x+width,y+height/2);
        painter.drawPoint(x,y+height);
        painter.drawPoint(x+width/2,y+height);
        painter.drawPoint(x+width,y+height);
        painter.setPen(QPen(QColor(46,159,238),3));
        painter.drawRect(QRect(x,y,width,height));
        this->setPixmap(pix);
    }
}

void FullScreen::resizeSelectLabel(int x,int y,int changeWidth, int changeHeight)
{
    int width = SelectAreaSize.width()+changeWidth;
    int height = SelectAreaSize.height()+changeHeight;
    sizeChange = QSize(width,height);
    QPixmap pix = *pixmap;
    QPainter painter(&pix);

    painter.drawPixmap(0,0,*fog);
    if(width!=0 && height != 0){
        painter.drawPixmap(x,y,pixmap->copy(x,y,width,height));
        painter.setPen(QPen(QColor(Qt::white),8));
        painter.drawText(x+5,y-5,QString("%1 x %2").arg(width).arg(height));
        painter.setPen(QPen(QColor(46,159,238),8));
        painter.drawPoint(x,y);
        painter.drawPoint(x+width/2,y);
        painter.drawPoint(x+width,y);
        painter.drawPoint(x,y+height/2);
        painter.drawPoint(x+width,y+height/2);
        painter.drawPoint(x,y+height);
        painter.drawPoint(x+width/2,y+height);
        painter.drawPoint(x+width,y+height);
        painter.setPen(QPen(QColor(46,159,238),3));
        painter.drawRect(QRect(x,y,width,height));
        this->setPixmap(pix);
    }
}

void FullScreen::setSelectedRound(){
    int x = origin.x()  < endPoint.x() ? origin.x():endPoint.x();
    int y = origin.y() < endPoint.y() ? origin.y():endPoint.y();
    int width = abs(endPoint.x() - origin.x());
    int height = abs(endPoint.y() - origin.y());
//    int t1 = endPoint.x() - origin.x();
//    int t2 = endPoint.y() - origin.y();
//    double r = sqrt(t1 * t1 + t2 * t2);

    if(width > 5 &&height >5){

        QPixmap pix = *pixmap;
        QPainter painter(&pix);
        painter.setPen(QPen(QColor(46,159,238),4));
        painter.drawPixmap(0,0,*fog);
        QSize size(width,height);
        QBitmap mask(size);
        QPainter painter2(&mask);
        painter2.setRenderHint(QPainter::Antialiasing);
        painter2.setRenderHint(QPainter::SmoothPixmapTransform);
        painter2.fillRect(0,0,width,height,Qt::white);
        painter2.setBrush(QColor(0,0,0));
        painter2.drawRoundedRect(0,0,width,height,width / 2,height / 2);

        image = pixmap->copy(x,y,width,height);
        image.setMask(mask);

        painter.drawPixmap(x,y,image);
        painter.drawRoundedRect(x,y,width,height,width / 2,height / 2);

        this->setPixmap(pix);
    }
    hasSelectArea = true;

}

void FullScreen::addButton()
{
    int x = origin.x() < endPoint.x() ? origin.x():endPoint.x();
    int y = origin.y() < endPoint.y() ? origin.y():endPoint.y();
    QPushButton *b = new QPushButton(this);
    QVBoxLayout *vl;
    vl = new QVBoxLayout(this);
    QPoint buttonPos;
    if(y+SelectAreaSize.height()+60>QApplication::desktop()->screenGeometry().height()){
         buttonPos = QPoint(x+SelectAreaSize.width()-60,y+SelectAreaSize.height()-60);
    }else{
         buttonPos = QPoint(x+SelectAreaSize.width()-50,y+SelectAreaSize.height()+10);
    }
    b->setGeometry(buttonPos.x(),buttonPos.y(),50,50);
    b->setStyleSheet("QPushButton{	color:white;background-color:rgb(14 , 150 , 254);border: 1px solid rgb(11 , 137 , 234);border-radius:6px;}QPushButton:hover{color:white;background-color:rgb(44 , 137 , 255);border: 1px solid rgb(11 , 137 , 234);}QPushButton:pressed{color:white;background-color:rgb(14 , 135 , 228);border: 1px solid rgb(12 , 138 , 235);padding-left:3px;padding-top:3px;}");
    b->setText("保存");
    connect(b,SIGNAL(clicked()),this,SLOT(savePng()));
    vl->addWidget(b);
    b = new QPushButton(this);
    b->setGeometry(buttonPos.x()-50-10,buttonPos.y(),50,50);
    b->setStyleSheet("QPushButton{	color:white;background-color:rgb(14 , 150 , 254);border: 1px solid rgb(11 , 137 , 234);border-radius:6px;}QPushButton:hover{color:white;background-color:rgb(44 , 137 , 255);border: 1px solid rgb(11 , 137 , 234);}QPushButton:pressed{color:white;background-color:rgb(14 , 135 , 228);border: 1px solid rgb(12 , 138 , 235);padding-left:3px;padding-top:3px;}");
    b->setText("复制");
    connect(b,SIGNAL(clicked()),this,SLOT(clipPng()));
    vl->addWidget(b);
    vl->setEnabled(false);
}

void FullScreen::deleteButton()
{
    if(this->layout()==NULL)
        return;
    //清空按钮
    int itemCount = this->layout()->count(); // layout是你前一个布局
    for (int i = (itemCount - 1); i >= 0; --i) //从末尾开始是因为你删除会影响布局的顺序。例如你删掉第一个，后面的会往前移，第二就变成第一个，然后这时你要是++i的话，就是删掉原来布局里的第三个，这第二个被跳过了。
    {
        QLayoutItem *item = this->layout()->takeAt(i);
        if (item != 0)
        {
            this->layout()->removeWidget(item->widget());
            delete item->widget(); //（ps：如果是子控件不是QWidget，这里会出错，要注意）
        }
    }
    delete this->layout();
}

void FullScreen::savePng()
{
    if(index == 0 && isDraging){
    int width = abs(endPoint.x() - origin.x());
    int height = abs(endPoint.y() - origin.y());
    int x = origin.x()  < endPoint.x() ? origin.x():endPoint.x();
    int y = origin.y() < endPoint.y() ? origin.y():endPoint.y();
    // QRect *rect = new QRect(rubberBand->pos().x(),rubberBand->pos.y(),rubberBand->pos().x() + rubberBand->width(),rubberBand->pos().y()+rubberBand->height());

    QPixmap pix = pixmap->copy(x,y,width,height);
    this->setPixmap(pix);
    //rubberBand->hide();
    //rubberBand->deleteLater();

      //获取系统当前时间并将其作为图片文件名
     QDateTime dt;
     QTime time;
     QDate date;
     dt.setTime(time.currentTime());
     dt.setDate(date.currentDate());
     QString currentDate  = dt.toString("yyyyMMddhhmmss");
     this->hide();
     //保存到文件夹
     QString fileName = QFileDialog::getSaveFileName(this,tr("另存为.."),"KTM截图_"+currentDate,tr("[图片] (*jpg,*jpeg,*.png)"));
      if (!fileName.isNull())
     {
           pix.save(fileName);
     }

     //把文件放到剪切板上
     QApplication::clipboard()->setImage(pix.toImage());
     this->close();
    }
    else if(index == 2){
       // this->close();
        QDateTime dt;
        QTime time;
        QDate date;
        dt.setTime(time.currentTime());
        dt.setDate(date.currentDate());
        QString currentDate  = dt.toString("yyyyMMddhhmmss");

        //保存到文件夹
        QString fileName = QFileDialog::getSaveFileName(this,tr("另存为.."),"KTM截图_"+currentDate,tr("[图片] (*jpg,*jpeg,*.png)"));
         if (!fileName.isNull())
        {
             pixmap->save(fileName);
        }

        //把文件放到剪切板上
        QApplication::clipboard()->setImage(image.toImage());
        this->close();
    }
}

void FullScreen::clipPng()
{
    if(index == 0 && isDraging){
    int width = abs(endPoint.x() - origin.x());
    int height = abs(endPoint.y() - origin.y());
    int x = origin.x()  < endPoint.x() ? origin.x():endPoint.x();
    int y = origin.y() < endPoint.y() ? origin.y():endPoint.y();
    // QRect *rect = new QRect(rubberBand->pos().x(),rubberBand->pos.y(),rubberBand->pos().x() + rubberBand->width(),rubberBand->pos().y()+rubberBand->height());

    QPixmap pix = pixmap->copy(x,y,width,height);
    this->setPixmap(pix);
    //把文件放到剪切板上
    QApplication::clipboard()->setImage(pix.toImage());
    this->close();
    }
}
void FullScreen::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
        if(hasSelectArea){
            hasSelectArea = false;
            isCompleteSelect = false;
            setCursor(Qt::ArrowCursor);
            //恢復霧化
            QPixmap pix = *pixmap;
            QPainter painter(&pix);
            painter.drawPixmap(0,0,*fog);
            this->setPixmap(pix);
            //删除按钮
            deleteButton();
        }else{
            this->close();
        }
    }
    isPressed = true;

    if(index == 0&&!hasSelectArea&&event->button() == Qt::LeftButton){//普通自适应情况
        hasSelectArea = true;
        origin = QPoint(miniRect.myRect_.topLeft().x(),miniRect.myRect_.topLeft().y());
        endPoint = QPoint(miniRect.myRect_.topLeft().x()+miniRect.myRect_.width(),miniRect.myRect_.topLeft().y()+miniRect.myRect_.height());
        SelectAreaSize = miniRect.myRect_.size();
        deleteButton();
        addButton();
    }
    if(!isCompleteSelect&&event->button() == Qt::LeftButton)//只在未画区域情况下进行新点标记
    {
        origin = event->pos();
    }else if(isCompleteSelect&&event->button()==Qt::LeftButton){
        currentCursorPos = event->pos();
    }
      //rubberBand = new QRubberBand(QRubberBand::Line, this);
      //rubberBand->setGeometry(QRect(origin, QSize()));
}

void FullScreen::keyPressEvent(QKeyEvent *e){

    if(e->key() == Qt::Key_Escape){
        this->close();
    }else if(e->key() == Qt::Key_Return){
        savePng();
    }
}

void FullScreen::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->x()-currentCursorPos.x();
    int y = event->y()-currentCursorPos.y();
    //rubberBand->setGeometry(QRect(origin, event->pos()).normalized());
    isDraging = true;
    if(index == 0){
         if(isPressed&&!isCompleteSelect){//普通自定截图区域
           setSelectedLabel();
           endPoint = event->pos();
           //清空按钮
           deleteButton();
         }
         else if(!isPressed&&!hasSelectArea){//根据鼠标位置给出适应截图区域
           drawMiniRec();
         }else if(isPressed&&isCompleteSelect){//移动截图区域
             if(this->cursor().shape()!=Qt::SizeAllCursor){
                 if(CursorState==0){
                     x=-x;
                     y=-y;
                     int startX = origin.x() < endPoint.x()? origin.x():endPoint.x();
                     int startY = origin.y() < endPoint.y()? origin.y():endPoint.y();
                     if(this->cursor().shape()==Qt::SizeVerCursor){
                        resizeSelectLabel(startX,startY-y,0,y);
                     }else if(this->cursor().shape()==Qt::SizeHorCursor){
                        resizeSelectLabel(startX-x,startY,x,0);
                     }else if(this->cursor().shape()==Qt::SizeFDiagCursor){
                        resizeSelectLabel(startX-x,startY-y,x,y);
                     }else if(this->cursor().shape()==Qt::SizeBDiagCursor){
                        resizeSelectLabel(startX-x,startY-y,x,y);
                     }
                     reverse = true;
                 }else{
                     int startX = origin.x() < endPoint.x()? origin.x():endPoint.x();
                     int startY = origin.y() < endPoint.y()? origin.y():endPoint.y();
                     if(this->cursor().shape()==Qt::SizeVerCursor){
                        resizeSelectLabel(startX,startY,0,y);
                     }else if(this->cursor().shape()==Qt::SizeHorCursor){
                        resizeSelectLabel(startX,startY,x,0);
                     }else if(this->cursor().shape()==Qt::SizeFDiagCursor){
                        resizeSelectLabel(startX,startY,x,y);
                     }
                     reverse = false;
                 }
             }else{
                moveSelectLabel(x,y);
             }
             //清空按钮
             deleteButton();
         }else if(!isPressed&&isCompleteSelect){//区域截图大小更改
            CursorState = setCursorState(event->x(),event->y());
         }else{
            setCursor(Qt::SizeAllCursor);
         }
    }
    //动态获取裁剪图片
    else if(index == 2){
       setSelectedRound();
    }
}

void FullScreen::mouseDoubleClickEvent(QMouseEvent){

     QDateTime dt;
     QTime time;
     QDate date;
     dt.setTime(time.currentTime());
     dt.setDate(date.currentDate());
     QString currentDate  = dt.toString("yyyyMMddhhmmss");

     //保存到文件夹
     QString fileName = QFileDialog::getSaveFileName(this,tr("另存为.."),"KTM截图_"+currentDate,tr("[图片] (*jpg,*jpeg,*.png)"));
     QPixmap pix;
      if (!fileName.isNull())
     {
          pix = pixmap->copy(miniRect.myRect_.topLeft().x(),miniRect.myRect_.topLeft().y(),miniRect.myRect_.width(),miniRect.myRect_.height());
          pix.save(fileName);
     }

     //把文件放到剪切板上
     QApplication::clipboard()->setImage(pix.toImage());

    qDebug() <<"double click...............";
    //   timer->deleteLater();
    this->close();
}

void FullScreen::mouseReleaseEvent(QMouseEvent *event)
{
    isPressed=false;
    if(event->button()==Qt::LeftButton){
        if(isCompleteSelect){//跟随位置更新
            origin = QPoint(origin.x()+movePos.x(),origin.y()+movePos.y());
            endPoint = QPoint(endPoint.x()+movePos.x(),endPoint.y()+movePos.y());
            movePos.setX(0);
            movePos.setY(0);
            //有大小修改
            if(sizeChange.width()>0||sizeChange.height()>0){
                if(reverse){
                    origin = QPoint(origin.x()-(sizeChange.width()-SelectAreaSize.width()),origin.y()-(sizeChange.height()-SelectAreaSize.height()));
                }else{
                    endPoint = QPoint(endPoint.x()+sizeChange.width()-SelectAreaSize.width(),endPoint.y()+sizeChange.height()-SelectAreaSize.height());
                }
                SelectAreaSize = sizeChange;
                sizeChange.setWidth(0);
                sizeChange.setHeight(0);
            }
        }
        isCompleteSelect = true;
        addButton();
    }
}
void FullScreen::paintEvent(QPaintEvent *event){
    QLabel::paintEvent(event);

}
FullScreen::~FullScreen()
{
}

