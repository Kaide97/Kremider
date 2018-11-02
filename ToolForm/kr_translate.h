/*一开始只想用作翻译作用,后发现可以做成内置浏览器*/
#ifndef KR_TRANSLATE_H
#define KR_TRANSLATE_H

#include <QDialog>

namespace Ui {
class kr_translate;
}

class kr_translate : public QDialog
{
    Q_OBJECT

public:
    explicit kr_translate(QWidget *parent = 0);
    QString sUrl;
    //加载网页
    void loadNavigate();
    ~kr_translate();
private slots:
    void on_CloseBtn_clicked();
    void on_GOButton_clicked();
    void receiveUrl(QString url);

private:
    Ui::kr_translate *ui;
    void paintEvent(QPaintEvent*);
    void initFrame();
    void initWebWidget();
    //移动窗口事件
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    //拉拽实现部分
    bool isDrag;
    QPoint m_position;
};

#endif // KR_TRANSLATE_H
