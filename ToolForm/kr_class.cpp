#include "kr_class.h"
#include "ui_kr_class.h"
#include "BasicForm/kwindows.h"
#include "BasicForm/kmessagebox.h"
#include "kr_translate.h"
#include <QPainter>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QRegExp>
#include <QSettings>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QVariant>
#include <QDebug>
#include <QDesktopServices>
#include <QStringList>
#include <QListWidgetItem>
#include <QVector>
kr_class::kr_class(QWidget *parent) :
    Kwindows(parent),
    ui(new Ui::kr_class),
m_news_website("http://news.gdut.edu.cn/ArticleList.aspx?category=4"),
currentDate(QDate::currentDate()),
hasLoadGrade(false)
{
    ui->setupUi(this);
    initTitleBar();
    initAllSignalSlot();
    initWidget();    //隐藏课表部分
    initSemComboBox();
    get_ac_pwd();
    loadVcode();
    initCampusNews();
    loadCampusNews();
    //子窗口都需要,解决长时间使用内存不释放问题
    setAttribute(Qt::WA_DeleteOnClose, true);
}

kr_class::~kr_class()
{
    delete ui;
}

void kr_class::initTitleBar()
{
    m_titleBar->move(1, 2);
    m_titleBar->setBackgroundColor(255,255, 255);
    m_titleBar->setTitleContent(QString("K管理 GDUT"));
    m_titleBar->setButtonType(ONLY_CLOSE_BUTTON);
    m_titleBar->setTitleWidth(this->width());
    m_titleBar->setWindowBorderWidth(2);
}

void kr_class::paintEvent(QPaintEvent *)
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

void kr_class::initAllSignalSlot()
{
    //新闻打开信号槽
    connect(ui->NewsWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(openDetailWebsite(QListWidgetItem*)));
    //新闻类型选定槽
    connect(ui->NewsTpyecomboBox,SIGNAL(activated(int)),this,SLOT(changeWebsite(int)));
    //课表选择学期与周加载槽
    //应该放在计算完成时间定位之后，否则容易查询错误
    //课表重置按钮槽
    connect(ui->ResetClassBtn,SIGNAL(clicked()),this,SLOT(initClassList()));
    //显示页面的选择
    connect(ui->stackedWidget,SIGNAL(currentChanged(int)),this,SLOT(stackWidgetPageChanged()));
}

void kr_class::initWidget()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->LastPageBtn->hide();
    ui->NextPageBtn->hide();
    //全部加载完才可切换过去看
    ui->NextPageBtn->setEnabled(false);
}

void kr_class::stackWidgetPageChanged()
{
    if(ui->stackedWidget->currentIndex()==1)
    {
        ui->NextPageBtn->show();
        ui->LastPageBtn->hide();
    }else if(ui->stackedWidget->currentIndex()==2){
        ui->NextPageBtn->hide();
        ui->LastPageBtn->show();
    }
}

void kr_class::get_ac_pwd()
{
    QString filepath=QCoreApplication::applicationDirPath();
    filepath+="/setting.ini";
    filepath.replace("/","\\");
    QSettings setting(filepath,QSettings::IniFormat);
    setting.beginGroup("config");
    if(!setting.value("ClassAccount").toString().isEmpty()&&!setting.value("ClassPassword").toString().isEmpty())
    {
        ui->accountLineEdit->setText(setting.value("ClassAccount").toString());
        ui->passwordEdit->setText(setting.value("ClassPassword").toString());
    }
    setting.endGroup();
}

void kr_class::save_ac_pwd()
{
    QString filepath=QCoreApplication::applicationDirPath();
    filepath+="/setting.ini";
    filepath.replace("/","\\");
    QSettings setting(filepath,QSettings::IniFormat);
    setting.beginGroup("config");
    if(true)
    {
        setting.setValue("ClassAccount",ui->accountLineEdit->text());
        setting.setValue("ClassPassword",ui->passwordEdit->text());
    }
    setting.endGroup();
}

void kr_class::initCampusNews()
{
    kr_translate *tr=new kr_translate();
    connect(this,SIGNAL(signalCampusNewsUrl(QString)),tr,SLOT(receiveUrl(QString)));
    emit signalCampusNewsUrl("http://news.gdut.edu.cn/ArticleList.aspx?category=4");
    tr->show();
    delete tr;
}

void kr_class::loadVcode()
{
    QNetworkRequest request;
    d_netmanager=new QNetworkAccessManager();
    d_netmanager->setCookieJar(new QNetworkCookieJar(this));
    request.setUrl(QUrl("http://222.200.98.147/yzm"));
    connect(d_netmanager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
    d_netmanager->get(request);
}

void kr_class::replyFinished(QNetworkReply *reply)
{
    disconnect(d_netmanager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));
    if (reply->error() == QNetworkReply::NoError)
    {
            QByteArray bytes = reply->readAll();
            QPixmap pixmap;
            pixmap.loadFromData(bytes);
            ui->VcLabel->setPixmap(pixmap);
   }
   cookies = d_netmanager->cookieJar()->cookiesForUrl(QUrl("http://222.200.98.147/yzm"));
}

void kr_class::verifyIdentity()
{
    QVariant var;
    var.setValue(cookies);
    d_request=new QNetworkRequest();
    QString post="account="+ui->accountLineEdit->text()+"&pwd="+ui->passwordEdit->text()+"&verifycode="+ui->VclineEdit->text();
    QByteArray postdata;
    postdata.append(post);
    d_request->setUrl(QUrl("http://222.200.98.147/new/login"));
    d_request->setHeader(QNetworkRequest::SetCookieHeader,var);
    d_request->setRawHeader("Content-Type","application/x-www-form-urlencoded; charset=UTF-8");
    d_request->setRawHeader("Connection", "Keep-Alive");
    d_request->setRawHeader("Accept","application/json, text/javascript, */*; q=0.01");
    d_request->setRawHeader("Accept-Language","zh-CN,zh;q=0.8");
    d_request->setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
    d_reply=d_netmanager->post(*d_request, postdata);
    connect(d_netmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(IdentityInfoloaded()));
}

void kr_class::IdentityInfoloaded()
{
    disconnect(d_netmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(IdentityInfoloaded()));
    d_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    d_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (d_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = d_reply->readAll();
        QString string = QString::fromUtf8(bytes);
        //思路是先判断账号密码，再验证码，都对才成功
        int pos=string.indexOf("您的帐号或密码不正确");//判断得到的信息里面有没有这个信息点
        if(pos>=0){
                Kmessagebox::showMyMessageBox(this,QString(tr("登录失败")),QString(tr("您的帐号或密码不正确")),MESSAGE_WARNNING,BUTTON_OK,true);
                //登录失败重新加载验证码
                loadVcode();
                ui->accountLineEdit->clear();
                ui->passwordEdit->clear();
                ui->accountLineEdit->setFocus();
        }
        else{
            pos=string.indexOf("验证码不正确");//判断得到的信息里面有没有这个信息点
            if(pos>=0){
            Kmessagebox::showMyMessageBox(this,QString(tr("登录失败")),QString(tr("验证码不正确")),MESSAGE_WARNNING,BUTTON_OK,true);
            //登录失败重新加载验证码
            loadVcode();
            }else{
                save_ac_pwd();//账号密码正确才进行存储
                getPersonalInfo();
                //定位当前日期所在周并查询课表
                getCurrentWeek_Sem();
                ui->stackedWidget->setCurrentIndex(1);
            }
        }
    }
}

void kr_class::getPersonalInfo()
{
    //QString post="account="+ui->account_LineEdit->text()+"&pwd="+ui->passwordEdit->text()+"&verifycode=";
    //QByteArray postdata;
    //postdata.append(post);
    d_request->setUrl(QUrl("http://222.200.98.147/login!welcome.action"));
    d_reply=d_netmanager->get(*d_request);
    connect(d_netmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(personalInfoloaded()));
}

void kr_class::personalInfoloaded()
{
    disconnect(d_netmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(personalInfoloaded()));
    str=new QString();
    if (d_reply->error() == QNetworkReply::NoError)
    {
        //登录人
        QByteArray bytes = d_reply->readAll();
        QString string = QString::fromUtf8(bytes);
        QRegExp reName("top\">\\\s{7}.{4}");
        int posName = 0;
        posName=reName.indexIn(string,posName);
        QString strName=reName.cap(0);
        strName.replace("top\">\r\n\t\t\t\t\t\t","");
        str->append(strName);str->append("同学你好  ");
        //登录时间
        QRegExp reTime("top\">\\\s{15}.{26}");
        int posTime = 0;
        posTime=reTime.indexIn(string,posTime);
        QString strTime=reTime.cap(0);
        strTime.replace("top\">\r\n\t\t\t\t\t\t\r\n\t\t\t\t\t\t\t","");
        str->append(strTime);
        //更改标题
        m_titleBar->setTitleContent("K管理 GDUT | "+*str);
    }
}

void kr_class::on_LoginBtn_clicked()
{
    if(!ui->accountLineEdit->text().isEmpty()&&!ui->passwordEdit->text().isEmpty())
    {
        verifyIdentity();
    }else{
        Kmessagebox::showMyMessageBox(this,QString(tr("登录失败")),QString(tr("密码/账号为空")),MESSAGE_WARNNING,BUTTON_OK,true);
    }
}

void kr_class::loadCampusNews()
{
    m_netmanager = new QNetworkAccessManager;
    m_reply = m_netmanager->get(QNetworkRequest(m_news_website));
    connect(m_netmanager, SIGNAL(finished(QNetworkReply*)),this, SLOT(campusNewsLoaded(QNetworkReply*)));
}

void kr_class::campusNewsLoaded(QNetworkReply *)
{
    m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if (m_reply->error() == QNetworkReply::NoError)
    {
        //加载新闻标题
        QByteArray bytes = m_reply->readAll();
        QString string = QString::fromUtf8(bytes);
        qDebug()<<string;
        QRegExp re("[^a-z1-9].title=\"([^\"]+)\"");
        re.setMinimal(true);
        int pos=0;// = re.indexIn(string);
        while ((pos = re.indexIn(string,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace(" title=\"","");
            str.replace("\"","");
            m_News_title<<str;
            pos += re.matchedLength();
        }m_News_title.replaceInStrings(" ","");
        //加载新闻链接
        re.setPattern("<a href=\"..viewarticle.aspx.articleid=[0-9]{6}");
        pos=0;
        while ((pos = re.indexIn(string,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("<a href=\".","");
            m_News_href<<str;
            pos += re.matchedLength();
        }
        m_News_href<<"ArticleList.aspx";//最后一项跳转到校内新闻主页
        //加载新闻时间
        re.setPattern("&nbsp;.{10}&nbsp");
        pos=0;
        while ((pos = re.indexIn(string,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("&nbsp","");
            str.replace(";","");
            m_News_time<<str;
            pos += re.matchedLength();
        }
        //加载新闻部门
        re.setPattern("<span title=\"[^\"]+\"");
        pos=0;
        while ((pos = re.indexIn(string,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("<span title=\"","");
            str.replace("\"","");
            m_News_department<<str;
            pos += re.matchedLength();
        }
    }
    //把新闻加到界面上，添加上上面的时间
    for(int index=0;index<m_News_title.count();index++)
    {
        QString str=m_News_time.at(index)+" "+m_News_title.at(index)+"——"+m_News_department.at(index);
        ui->NewsWidget->addItem(str);
    }
    ui->NewsWidget->addItem("点我查看更多校内新闻");
    //connect(ui->NewsWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(openDetailWebsite(QListWidgetItem*)));
    m_reply->deleteLater();
}
void kr_class::on_RefreshBtn_clicked()
{
    ui->NewsWidget->clear();
    m_News_title.clear();
    m_News_href.clear();
    m_News_department.clear();
    m_News_time.clear();
    loadCampusNews();
}

void kr_class::openDetailWebsite(QListWidgetItem *currentItem)
{
    int pos=ui->NewsWidget->row(currentItem);
    QString *surl=new QString("http://news.gdut.edu.cn/"+m_News_href.at(pos));
    kr_translate *tr=new kr_translate();
    connect(this,SIGNAL(signalCampusNewsUrl(QString)),tr,SLOT(receiveUrl(QString)));
    emit signalCampusNewsUrl(*surl);
    tr->show();
}

void kr_class::changeWebsite(int index)
{
    switch (index) {
    case 0:
        m_news_website="http://news.gdut.edu.cn/ArticleList.aspx?category=4";
        break;
    case 1:
        m_news_website="http://news.gdut.edu.cn/ArticleList.aspx?category=6";
        break;
    case 2:
        m_news_website="http://news.gdut.edu.cn/ArticleList.aspx?category=8";
        break;
    }
    //清除已有的新闻一切信息
    ui->NewsWidget->clear();
    m_News_title.clear();
    m_News_href.clear();
    m_News_title.clear();
    loadCampusNews();//更换完立刻加载网页内容
}

void kr_class::initSemComboBox()//更易接受的显示方式
{
    QStringList year;
    year<<"201402"<<"201501"<<"201502"<<"201601"<<"201602"<<"201701"<<"201702"<<"201801"<<"201802"<<"201901";
    for(int i=0;i<year.count();++i)
    {
        ui->SemcomboBox->setItemData(i,year.at(i));
    }
}

void kr_class::getCurrentWeek_Sem()
{
    //初始化网络对象
    cm_netmanager = d_netmanager;
    cm_request = new QNetworkRequest();
    switch (currentDate.toString("yyyy").toInt()) {
    case 2017:
        if(currentDate>=QDate(2017,2,20)&&currentDate<=QDate(2017,7,7))
        {
            int day=currentDate.daysTo(QDate(2017,2,20));
            currentWeek=((-day)/7)+1;
            currentSem=201602;
        }
        if(currentDate>=QDate(2017,9,4)&&currentDate<=QDate(2017,12,31))
        {
            int day=currentDate.daysTo(QDate(2017,9,4));
            currentWeek=((-day)/7)+1;
            currentSem=201701;
        }
        break;
    case 2018:
        if(currentDate>=QDate(2018,1,1)&&currentDate<=QDate(2018,1,19))
        {
            int day=currentDate.daysTo(QDate(2017,9,4));
            currentWeek=((-day)/7)+1;
            currentSem=201701;
        }
        if(currentDate>=QDate(2018,3,5)&&currentDate<=QDate(2018,7,6))
        {
            int day=currentDate.daysTo(QDate(2017,3,5));
            currentWeek=((-day)/7)+1;
            currentSem=201702;
        }
        if(currentDate>=QDate(2018,9,3)&&currentDate<=QDate(2018,12,31))
        {
            int day=currentDate.daysTo(QDate(2018,9,3));
            currentWeek=((-day)/7)+1;
            currentSem=201801;
        }
        break;
    case 2019:
        if(currentDate>=QDate(2019,1,1)&&currentDate<=QDate(2019,1,11))
        {
            int day=currentDate.daysTo(QDate(2018,9,3));
            currentWeek=((-day)/7)+1;
            currentSem=201801;
        }
        if(currentDate>=QDate(2019,2,25)&&currentDate<=QDate(2019,7,5))
        {
            int day=currentDate.daysTo(QDate(2019,2,25));
            currentWeek=((-day)/7)+1;
            currentSem=201802;
        }
        break;
    }
    initClassList();//得到目前的学期和周数和显示课表
}

//初始化默认课表
void kr_class::initClassList()
{
    //防止因为加载过慢导致后面的加载成绩与更换课表产生争用networkaccesmanager
    ui->SemcomboBox->setEnabled(false);
    ui->WeekcomboBox->setEnabled(false);
    QString currentSemText;
    switch (currentSem) {
    case 201602:
        currentSemText="2017 上学期";
        break;
    case 201701:
        currentSemText="2017 下学期";
        break;
    case 201702:
        currentSemText="2018 上学期";
        break;
    case 201801:
        currentSemText="2018 下学期";
        break;
    case 201802:
        currentSemText="2019 上学期";
        break;
    }

    ui->SemcomboBox->setCurrentText(QString("%1").arg(currentSemText));//初始化目前的combobox显示
    ui->WeekcomboBox->setCurrentText(QString("%1").arg(currentWeek));
    QString sem=ui->SemcomboBox->currentData().toString();
    QString week=ui->WeekcomboBox->currentText();
    m_class_url=QString("http://222.200.98.147/xsgrkbcx!getKbRq.action?xnxqdm=%1&zc=%2").arg(sem,week);
    //修复初始化没有显示的问题
    QTimer *initClass = new QTimer(this);
    initClass->setSingleShot(true);
    connect(initClass,SIGNAL(timeout()),this,SLOT(queryClassList()));
    initClass->start(50);
    //放在这里是为了防止因为时间定位导致的查询错误
    //课表选择学期与周加载槽
    connect(ui->WeekcomboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(queryClassList()));
    connect(ui->SemcomboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(queryClassList()));
}

void kr_class::queryClassList()
{
    QString sem=ui->SemcomboBox->currentData().toString();
    QString week=ui->WeekcomboBox->currentText();
    m_class_url=QString("http://222.200.98.147/xsgrkbcx!getKbRq.action?xnxqdm=%1&zc=%2").arg(sem,week);
    getClassList();
}

//查询课表
void kr_class::getClassList()
{
    ui->ClassNoticeWidget->clear();//保证每次查询都是空的
    m_News_all.clear();//清除原有的缓存
    cm_request->setUrl(QUrl(m_class_url));
    cm_netmanager->get(*cm_request);
    connect(cm_netmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(classListloaded(QNetworkReply*)));
}

void kr_class::classListloaded(QNetworkReply *cm_reply)
{
    cm_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    cm_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QStringList m_class_name,m_class_xq,m_class_jc,m_class_pos;
    if (cm_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = cm_reply->readAll();
        QString classinfo = QString::fromUtf8(bytes);
        //加载课名
        QRegExp re("kcmc\":\"[^\"]+\"");
        re.setMinimal(true);
        int pos=0;
        while ((pos = re.indexIn(classinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("kcmc\":\"","");
            str.replace("\"","");
            m_class_name<<str;
            pos += re.matchedLength();
        }
        //加载课程节次
        re.setPattern("jcdm\":\"[^\"]+\"");
        pos=0;
        while ((pos = re.indexIn(classinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("jcdm\":\"","");
            str.replace("\"","");
            m_class_jc<<str;
            pos += re.matchedLength();
        }
        //加载课程地点
        re.setPattern("jxcdmc\":\"[^\"]*\"");
        pos=0;
        while ((pos = re.indexIn(classinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("jxcdmc\":\"","");
            str.replace("\"","");
            m_class_pos<<str;
            pos += re.matchedLength();
        }
        //加载课程星期
        re.setPattern("xq\":\"[0-9]\"");
        pos=0;
        while ((pos = re.indexIn(classinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("xq\":\"","");
            str.replace("\"","");
            m_class_xq<<str;
            pos += re.matchedLength();
        }
    }
    //拼接课程信息
    for(int index=0;index<m_class_name.count();index++)
    {
        m_News_all<<m_class_xq.at(index)+"  "+m_class_jc.at(index)+"节  "+m_class_name.at(index)+"  【"+m_class_pos.at(index)+"】";
    }
    //排序
    m_News_all.sort(Qt::CaseSensitive);
    //显示
    QList<int> colorDepot;//rgb值保存容器
    for(int index=0;index<m_class_name.count();index++)
    {
        if(index>0&&QString(m_News_all.at(index)).left(1)!=QString(m_News_all.at(index-1)).left(1)||index==0)
        {
            colorDepot = chooseColor(QString(m_News_all.at(index)).left(1).toInt());
            QListWidgetItem *day=new QListWidgetItem();
            day->setText("星期"+QString(m_News_all.at(index)).left(1));
            day->setBackgroundColor(QColor(colorDepot[0],colorDepot[1],colorDepot[2]));
            ui->ClassNoticeWidget->addItem(day);
        }
        QListWidgetItem *i=new QListWidgetItem();
        i->setText(m_News_all.at(index).mid(3));
        i->setBackgroundColor(QColor(colorDepot[0],colorDepot[1],colorDepot[2]));
        ui->ClassNoticeWidget->addItem(i);
    }
    if(m_class_name.count()==0)
    {
        QListWidgetItem *i=new QListWidgetItem();
        i->setText("无课");
        i->setTextAlignment(Qt::AlignCenter);
        i->setBackgroundColor(QColor(160,160,160));
        ui->ClassNoticeWidget->addItem(i);
    }
    disconnect(cm_netmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(classListloaded(QNetworkReply*)));
    //等用完再加载
    if(!hasLoadGrade)
    {
        getCETSGrade();
        hasLoadGrade=true;
    }
}

//选择颜色
QList<int> kr_class::chooseColor(int xq)
{
    QList<int> temp;
    switch (xq) {
    case 1:
        temp<<245<<160<<200;
        return temp;
        break;
    case 2:
        temp<<238<<160<<245;
        return temp;
        break;
    case 3:
        temp<<160<<196<<245;
        return temp;
        break;
    case 4:
        temp<<160<<230<<245;
        return temp;
        break;
    case 5:
        temp<<160<<245<<212;
        return temp;
        break;
    case 6:
        temp<<196<<245<<160;
        return temp;
        break;
    case 7:
        temp<<245<<222<<160;
        return temp;
        break;
    }
}

void kr_class::getCETSGrade()
{
    //用同一个networkaccessmanager保存登录状态
    cm_netmanager->get(QNetworkRequest(QUrl("http://222.200.98.147/xskjcjxx!getDataList.action?page=1&rows=50&sort=xnxqdm&order=asc")));
    connect(cm_netmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(loadCETSGrade(QNetworkReply*)));
}

void kr_class::getTestGrade()
{
    //用同一个networkaccessmanager保存登录状态
    cm_netmanager->get(QNetworkRequest(QUrl("http://222.200.98.147/xskccjxx!getDataList.action")));
    connect(cm_netmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(loadTestGrade(QNetworkReply*)));
}


void kr_class::loadCETSGrade(QNetworkReply *Cets_reply)
{
    //断掉绑定
    disconnect(cm_netmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(loadCETSGrade(QNetworkReply*)));
    Cets_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    Cets_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    QStringList m_CETS_xq,m_CETS_level,m_CETS_id,m_CETS_totalgrade,m_CETS_grade;
    if (Cets_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = Cets_reply->readAll();
        QString Cetsinfo = QString::fromUtf8(bytes);
        QRegExp re("xnxqmc\":\"[^\"]*");
        int pos=0;
        while ((pos = re.indexIn(Cetsinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("xnxqmc\":\"","");
            m_CETS_xq<<str;
            pos += re.matchedLength();
        }
        re.setPattern("kjkcmc\":\"[^\"]*");
        pos=0;
        while ((pos = re.indexIn(Cetsinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("kjkcmc\":\"","");
            m_CETS_level<<str;
            pos += re.matchedLength();
        }
        re.setPattern("zkzh\":\"[^\"]*");
        pos=0;
        while ((pos = re.indexIn(Cetsinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("zkzh\":\"","准考证号:");
            m_CETS_id<<str;
            pos += re.matchedLength();
        }
        re.setPattern("zcj\":\"[^\"]*");
        pos=0;
        while ((pos = re.indexIn(Cetsinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("zcj\":\"","总成绩:");
            m_CETS_totalgrade<<str;
            pos += re.matchedLength();
        }
        re.setPattern("xm.cj\":\"[^\"]*");
        pos=0;
        while ((pos = re.indexIn(Cetsinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("xm1cj\":\"","听力:");
            str.replace("xm2cj\":\"","阅读:");
            str.replace("xm3cj\":\"","写作:");
            str.replace("xm4cj\":\"","综合:");
            str.replace("xm5cj\":\"","口语:");
            m_CETS_grade<<str;
            pos += re.matchedLength();
        }
        for(int pos =0,gradePos=0;pos<m_CETS_xq.count();++pos,gradePos+=5)
        {
            ui->EnglishLevelGPAlistWidget->addItem(m_CETS_xq.at(pos)+" "+m_CETS_level.at(pos)+" "+m_CETS_id.at(pos)+"\n"+m_CETS_totalgrade.at(pos)+" "+m_CETS_grade.at(gradePos)+" "+m_CETS_grade.at(gradePos+1)+" "+m_CETS_grade.at(gradePos+2)+" "+m_CETS_grade.at(gradePos+3)+" "+m_CETS_grade.at(gradePos+4));
        }
    }
    //接力使用networkaccessmanager
    getTestGrade();
}

void kr_class::loadTestGrade(QNetworkReply *Test_reply)
{
    //断掉绑定
    disconnect(cm_netmanager,SIGNAL(finished(QNetworkReply*)),this,SLOT(loadTestGrade(QNetworkReply*)));
    Test_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    Test_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (Test_reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = Test_reply->readAll();
        QString Testinfo = QString::fromUtf8(bytes);
        QRegExp re("xnxqmc\":\"[^\"]*");
        int pos=0;
        while ((pos = re.indexIn(Testinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("xnxqmc\":\"","");
            m_Test_xq<<str;
            pos += re.matchedLength();
        }
        re.setPattern("kcmc\":\"[^\"]*");
        pos=0;
        while ((pos = re.indexIn(Testinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("kcmc\":\"","");
            m_Test_name<<str;
            pos += re.matchedLength();
        }
        re.setPattern("zcj\":\"[^\"]*");
        pos=0;
        while ((pos = re.indexIn(Testinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("zcj\":\"","");
            m_Test_totalGrade<<str;
            pos += re.matchedLength();
        }
        re.setPattern("xf\":\"[^\"]*");
        pos=0;
        while ((pos = re.indexIn(Testinfo,pos)) != -1)
        {
            QString str=re.cap(0);
            str.replace("xf\":\"","");
            m_Test_xf<<str;
            pos += re.matchedLength();
        }
        //去重处理载入combobox及显示处理
        QStringList sem;
        for(int pos = 0;pos<m_Test_xq.count();++pos)
        {
            if(sem.indexOf(m_Test_xq.at(pos))==-1)
            {
                sem<<m_Test_xq.at(pos);
                ui->GradeSemcomboBox->addItem(sem.at(sem.count()-1));
            }
        }
        //解锁
        ui->SemcomboBox->setEnabled(true);
        ui->WeekcomboBox->setEnabled(true);
        //查看不同学期的成绩
        loadSemTestGrade("全部");
        //绑定combobox
        connect(ui->GradeSemcomboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(loadSemTestGrade(QString)));
    }
}

void kr_class::loadSemTestGrade(QString str)
{
    //重置容器
    GPA=0.0;
    xf=0.0;
    bool moreThan90 = false;
    ui->GPAlistWidget->clear();
    ui->GradeSemcomboBox->setEnabled(false);//阻塞直到计算完成
    if(str=="全部")
    {
        for(int pos = 0;pos<m_Test_xq.count();++pos)
        {
            calculateGPA(pos);
            if(m_Test_totalGrade.at(pos)>="90"||m_Test_totalGrade.at(pos)=="优秀")
                moreThan90 = true;
            else moreThan90 = false;
            if(moreThan90)
            {
                QListWidgetItem *gpaitem = new QListWidgetItem();
                gpaitem->setText(m_Test_xq.at(pos)+" "+m_Test_name.at(pos)+" 成绩:"+m_Test_totalGrade.at(pos));
                gpaitem->setTextColor(QColor(143,122,16));
                ui->GPAlistWidget->addItem(gpaitem);
            }
            else
                ui->GPAlistWidget->addItem(m_Test_xq.at(pos)+" "+m_Test_name.at(pos)+" 成绩:"+m_Test_totalGrade.at(pos));
        }
    }else{
        for(int pos = 0;pos<m_Test_xq.count();++pos)
        {
            if(m_Test_xq.at(pos)==str)
            {
                calculateGPA(pos);
                if(m_Test_totalGrade.at(pos)>="90"||m_Test_totalGrade.at(pos)=="优秀")
                    moreThan90 = true;
                else moreThan90 = false;
                if(moreThan90)
                {
                    QListWidgetItem *gpaitem = new QListWidgetItem();
                    gpaitem->setText(m_Test_xq.at(pos)+" "+m_Test_name.at(pos)+" 成绩:"+m_Test_totalGrade.at(pos));
                    gpaitem->setTextColor(QColor(143,122,16));
                    ui->GPAlistWidget->addItem(gpaitem);
                }
                else
                    ui->GPAlistWidget->addItem(m_Test_xq.at(pos)+" "+m_Test_name.at(pos)+" 成绩:"+m_Test_totalGrade.at(pos));
            }
        }
    }
    ui->GPAlabel->setText("绩点: "+QString("%1").arg(GPA/xf));
    ui->GradeSemcomboBox->setEnabled(true);
    //全部搞定再可切换成绩页面
    ui->NextPageBtn->setEnabled(true);
}

void kr_class::calculateGPA(int pos)
{
    if(QString(m_Test_totalGrade.at(pos)).indexOf(QRegExp("[0-9]"))!=-1&&QString(m_Test_totalGrade.at(pos)).toInt()>=60)
    {
        GPA+=(double)((((QString(m_Test_totalGrade.at(pos)).toDouble())/10)-5)*QString(m_Test_xf.at(pos)).toDouble());
    }else if(m_Test_totalGrade.at(pos)=="优秀"){
        GPA+=4.5*QString(m_Test_xf.at(pos)).toDouble();
    }else if(m_Test_totalGrade.at(pos)=="良好")
    {
        GPA+=3.5*QString(m_Test_xf.at(pos)).toDouble();
    }else if(m_Test_totalGrade.at(pos)=="中等")
    {
        GPA+=2.5*QString(m_Test_xf.at(pos)).toDouble();
    }else if(m_Test_totalGrade.at(pos)=="及格")
    {
        GPA+=1.5*QString(m_Test_xf.at(pos)).toDouble();
    }
    //学分加起来
    xf+=QString(m_Test_xf.at(pos)).toDouble();
}

void kr_class::on_LastPageBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()-1);
}

void kr_class::on_NextPageBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()+1);
}
