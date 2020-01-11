#include "WorkLog.h"
#include "ui_WorkLog.h"
#include "Global.h"
#include "MyMessageBox.h"
#include <QPainter>
#include <QPainterPath>
#include <qmath.h>
#include <QDebug>
#include <QDateTime>
#include <QStringListModel>
#include <QHostInfo>

WorkLog::WorkLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorkLog)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags( Qt::Tool |Qt::WindowStaysOnTopHint |Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    isAlter = false;

    //设置教师下拉列表
    initTeacherList();
    completer->setCaseSensitivity(Qt::CaseInsensitive); //设置不区分大小写
    completer->setFilterMode(Qt::MatchContains);  //设置内容匹配
    ui->leTeacherName->setCompleter(completer);

    //设置课程下拉列表
    completerForCourseName = new QCompleter();
    completerForCourseName->setCaseSensitivity(Qt::CaseInsensitive); //设置不区分大小写
    completerForCourseName->setFilterMode(Qt::MatchContains);  //设置内容匹配
    completerForCourseName->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    ui->leLessonName->setCompleter(completerForCourseName);
    ui->leLessonName->installEventFilter(this);


    //设置班别下拉列表
    completerForClass = new QCompleter();
    completerForClass->setCaseSensitivity(Qt::CaseInsensitive); //设置不区分大小写
    completerForClass->setFilterMode(Qt::MatchContains);  //设置内容匹配
    completerForClass->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    ui->leClassName->setCompleter(completerForClass);
    ui->leClassName->installEventFilter(this);

    //connect(ui->leTeacherName,SIGNAL(textChanged(QString)),this,SLOT(on_tNameTextChanged(QString)));

    ui->pushButton->setObjectName("surebutton");

    ui->returnBtn->setObjectName("commonbutton");
    ui->returnBtn->setVisible(false);
    ui->leEtime->setText("2");
    ui->leWeek->setFocus();
    // 设置tab控件焦点切换顺序
    QWidget::setTabOrder(ui->textDate, ui->leWeek);
    QWidget::setTabOrder(ui->leWeek, ui->leXingqi);
    QWidget::setTabOrder(ui->leXingqi, ui->classTimeStartCbx);
    QWidget::setTabOrder(ui->classTimeStartCbx, ui->classTimeEndCbx);
    QWidget::setTabOrder(ui->classTimeEndCbx, ui->leTeacherName);
    QWidget::setTabOrder(ui->leTeacherName, ui->leLessonName);
    QWidget::setTabOrder(ui->leTeacherName, ui->leLessonName);
    QWidget::setTabOrder(ui->leLessonName, ui->leMajor);
    QWidget::setTabOrder(ui->leMajor, ui->leClassName);
    QWidget::setTabOrder(ui->leClassName, ui->leCnum);
    QWidget::setTabOrder(ui->leCnum, ui->leRepresentNum);
    QWidget::setTabOrder(ui->leRepresentNum, ui->textGroupNum);
    QWidget::setTabOrder(ui->textGroupNum, ui->leEtime);
    QWidget::setTabOrder(ui->leEtime, ui->cbxEtype);
    QWidget::setTabOrder(ui->cbxEtype, ui->cbxEcategory);
    QWidget::setTabOrder(ui->cbxEcategory, ui->cbxRequire);
    QWidget::setTabOrder(ui->cbxRequire, ui->pushButton);
    QWidget::setTabOrder(ui->pushButton, ui->textDate);

    loop = new QEventLoop(this);
    ui->leClassName->setEnabled(false);
    classButtonGroup = new QButtonGroup;
    classButtonGroup->addButton(ui->classRadio1,1);
    classButtonGroup->addButton(ui->classRadio2,2);
    classButtonGroup->addButton(ui->classRadio3,3);
    classButtonGroup->addButton(ui->classRadio4,4);

    connect(classButtonGroup,SIGNAL(buttonClicked(int)),this,SLOT(on_ButtonGroupClick(int)));
}


void WorkLog::paintEvent(QPaintEvent *)
{
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(10, 10, this->width()-20, this->height()-20);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for(int i=0; i<10; i++)
    {
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(10-i, 10-i, this->width()-(10-i)*2, this->height()-(10-i)*2);
        color.setAlpha(150 - qSqrt(i)*50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}

void WorkLog::on_ButtonGroupClick(const int &index)
{
    if(index == 1)
    {
        classSeletedChange(ui->classRadio1->text());
    }
    else if(index == 2)
    {
        classSeletedChange(ui->classRadio2->text());
    }
    else if(index == 3)
    {
        classSeletedChange(ui->classRadio3->text());
    }
    else
    {
        ui->leMajor->clear();
        ui->leLessonName->clear();
    }
}


/**
* @brief 改变选择的班别
* @date 2017/12/30
* @author han
* @return void
*/
void WorkLog::classSeletedChange(QString className)
{
    Global g;
    g.connectdb();
    QString sql = QString("select major,lname,tablename,erequire,etype,ecategory"
                          " from classinfo left join lesson on classinfo.classname = lesson.cname "
                          " where classname = '%1';").arg(className);
    g.sql.exec(sql);
    QString tablename = "";
    if(g.sql.next())  //获取专业，课程名
    {
        ui->leMajor->setText(g.sql.value(0).toString());
        ui->leLessonName->setText(g.sql.value(1).toString());
        tablename = g.sql.value(2).toString();

        ui->cbxRequire->setCurrentText(g.sql.value(3).toString());
        ui->cbxEtype->setCurrentText(g.sql.value(4).toString());
        ui->cbxEcategory->setCurrentText(g.sql.value(5).toString());
    }

    if(!tablename.trimmed().isEmpty())  //获取该班的人数
    {
        g.sql.clear();

        sql = QString("SELECT count(*) from %1").arg(tablename);
        g.sql.exec(sql);
        if(g.sql.next())
        {
            ui->leCnum->setText(g.sql.value(0).toString());
        }
    }

    getCourseInfo();
}
/**
* @brief exec
* @date 2017/12/19
* @author han
* @return void
*/
void WorkLog::exec()
{
    setAttribute(Qt::WA_ShowModal, true);
    show();
    loop->exec();  //开启事件循环
}

/**
* @brief 设置返回按钮是否可用
* @date 2017/12/12
* @author han
* @return void
*/
void WorkLog::setReturnBtnEnable(bool f)
{
    ui->returnBtn->setVisible(f);
}
/**
* @brief 确定，存入数据库
* @date 2017/12/9
* @author han
* @return void
*/
void WorkLog::onClick()
{
    Global g;
    if(g.LoginType == Global::TYPE1)
    {
        if(ui->textDate->text().trimmed().isEmpty() ||ui->leWeek->text().trimmed().isEmpty()
                ||ui->leXingqi->text().trimmed().isEmpty()||ui->leMajor->text().trimmed().isEmpty()
                ||ui->leTeacherName->text().trimmed().isEmpty() ||ui->leLessonName->text().trimmed().isEmpty()
                ||ui->leCnum->text().trimmed().isEmpty()||ui->leRepresentNum->text().trimmed().isEmpty()
                ||ui->leEtime->text().trimmed().isEmpty()||ui->textGroupNum->text().trimmed().isEmpty())
        {
            MyMessageBox box;
            box.iniMsgBox(MyMessageBox::alert,"除了报修选项，其他都不能填空");
            box.isShowDialog(true);
            box.exec();
            return;
        }
    }
    else if(g.LoginType == Global::TYPE2)
    {
        if(ui->textDate->text().trimmed().isEmpty() ||ui->leWeek->text().trimmed().isEmpty()
                ||ui->leXingqi->text().trimmed().isEmpty()
                ||ui->leTeacherName->text().trimmed().isEmpty() ||ui->leLessonName->text().trimmed().isEmpty())
        {
            MyMessageBox box;
            box.iniMsgBox(MyMessageBox::alert,"请填写课程名称、班级");
            box.isShowDialog(true);
            box.exec();
            return;
        }
    }
    else
    {
        if(ui->textDate->text().trimmed().isEmpty() ||ui->leWeek->text().trimmed().isEmpty()
                ||ui->leXingqi->text().trimmed().isEmpty()||ui->textGroupNum->text().trimmed().isEmpty()
                ||ui->leLessonName->text().trimmed().isEmpty()
                ||ui->leCnum->text().trimmed().isEmpty()||ui->leRepresentNum->text().trimmed().isEmpty())
        {
            MyMessageBox box;
            box.iniMsgBox(MyMessageBox::alert,"请填写以上内容.");
            box.isShowDialog(true);
            box.exec();
            return;
        }
    }

    if(ui->leClassName->text().trimmed().isEmpty() && ui->classRadio4->isChecked())
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::alert,"请输入班别");
        box.isShowDialog(true);
        box.exec();
        return;
    }

    int start = ui->classTimeStartCbx->currentIndex() + 1;
    int end = ui->classTimeEndCbx->currentIndex() + 1;
    QString useTime = QString::number((end - start + 1) * 45.0 / 60, 10, 2);  //使用时长
    QString date = ui->textDate->text();
    QString week = ui->leWeek->text();
    QString day = ui->leXingqi->text();
    QString lesson = QString("%1/%2").arg(QString::number(start,10),QString::number(end,10));
    QString tname = ui->leTeacherName->text();
    QString lname = ui->leLessonName->text();
    QString major = ui->leMajor->text();
    QString cnum = ui->leCnum->text();
    QString rnum = ui->leRepresentNum->text();
    QString etime = ui->leEtime->text();
    QString etype = ui->cbxEtype->currentText().trimmed();
    QString erequire = ui->cbxRequire->currentText().trimmed();
    QString groupnum = ui->textGroupNum->text();
    QString ecategory = ui->cbxEcategory->currentText().trimmed();
    QString labName = QHostInfo::localHostName();
    QString cname;

    if(ui->classRadio1->isChecked())
        cname = ui->classRadio1->text();
    else if(ui->classRadio2->isChecked())
        cname = ui->classRadio2->text();
    else if(ui->classRadio3->isChecked())
        cname = ui->classRadio3->text();
    else
        cname = ui->leClassName->text();

    lesson = lesson.trimmed();

    //获取当前时间
    QDateTime time;
    time = QDateTime::currentDateTime();
    QString currentdateTime = time.toString("yyyy-MM-dd hh:mm:ss");

    g.connectdb();

    if(isAlter)  //课堂界面修改
    {
        QString sqlstr;
        if(g.LoginType == Global::TYPE3)
        {
            useTime = groupnum;
            sqlstr = QString("update worklog set date = '%1',week = '%2',day = '%3',tname = '%4',lname = '%5',"
                             " cnum = '%6',cpresentNum = '%7',LaboratoryNum = '%8' , lastUpdateTime = '%9',useTime='%10',teacherID='%11' "
                             " where id = '%12';")
                    .arg(date,week,day,tname,lname)
                    .arg(cnum,rnum,labName,currentdateTime,useTime,g.getTeacherID())
                    .arg(g.getWorkLogID());
        }
        else
        {
            sqlstr = QString("update worklog set date = '%1',week = '%2',day = '%3',lesson = '%4',tname = '%5',lname = '%6',cmajor = '%7',cname = '%8',"
                             " cnum = '%9',cpresentNum = '%10',etime = '%11',etype = '%12',erequire = '%13',groupnum = '%14',"
                             " ecategory = '%15',LaboratoryNum = '%16' , lastUpdateTime = '%17',useTime='%18',teacherID='%19' "
                             " where id = '%20';")
                    .arg(date,week,day,lesson,tname,lname,major,cname)
                    .arg(cnum,rnum,etime,etype,erequire,groupnum)
                    .arg(ecategory,labName,currentdateTime,useTime,g.getTeacherID())
                    .arg(g.getWorkLogID());
        }
       // qDebug()<<sqlstr;
        bool state = g.sql.exec(sqlstr);

        if(!state)
        {
            Global::writeLog("error",QString("教师%1修改日志失败,错误:%1").arg(g.sql.lastError().text()));
            MyMessageBox box;
            box.isShowDialog(true);
            box.iniMsgBox(MyMessageBox::alert,QString("请检查你的输入是否正确，错误代码:%1").arg(g.sql.lastError().text()));
            box.exec();
            return;
        }
        Global::writeLog("work",QString("教师%1修改日志成功").arg(g.teacherNum));

        isAlter = false;
    }
    else//登录填写
    {
        QString checkExitSql = QString("select * from worklog where date = '%1' and lesson = '%2' and tname = '%3' and lname = '%4' ")
                .arg(date,lesson,tname,lname);
        // qDebug() << "checkExitSql" << checkExitSql;
        g.sql.exec(checkExitSql);


        if(g.sql.next())  //判断是否存在该日志
        {
            QString sqlstr ;
            if(g.LoginType == Global::TYPE3)
            {
                useTime = groupnum;
                sqlstr = QString("update worklog set date = '%1',week = '%2',day = '%3',tname = '%4',lname = '%5',"
                                 " cnum = '%6',cpresentNum = '%7',LaboratoryNum = '%8' , lastUpdateTime = '%9',useTime='%10',purpose='%11',teacherID='%12' "
                                 " where date = '%1' and tname = '%4' and lname = '%5';")
                        .arg(date,week,day,tname,lname)
                        .arg(cnum,rnum,labName,currentdateTime,useTime,purpose,g.getTeacherID());
            }
            else
            {
                sqlstr = QString("update worklog set date = '%1',week = '%2',day = '%3',lesson = '%4',tname = '%5',lname = '%6',cmajor = '%7',cname = '%8',"
                                 " cnum = '%9',cpresentNum = '%10',etime = '%11',etype = '%12',erequire = '%13',groupnum = '%14',"
                                 " ecategory = '%15',LaboratoryNum = '%16' , lastUpdateTime = '%17',useTime='%18',purpose='%19',teacherID='%20' "
                                 " where date = '%1' and lesson = '%4' and tname = '%5' and lname = '%6' and cname = '%8';")
                        .arg(date,week,day,lesson,tname,lname,major,cname)
                        .arg(cnum,rnum,etime,etype,erequire,groupnum)
                        .arg(ecategory,labName,currentdateTime,useTime,purpose,g.getTeacherID());
            }
            //qDebug() <<"存在"<< sqlstr;

            bool state = g.sql.exec(sqlstr);

            if(!state)
            {
                Global::writeLog("error",QString("教师%1修改日志失败,错误:%1").arg(g.sql.lastError().text()));
                MyMessageBox box;
                box.isShowDialog(true);
                box.iniMsgBox(MyMessageBox::alert,QString("请检查你的输入是否正确，错误代码:%1").arg(g.sql.lastError().text()));
                box.exec();
                return;
            }
            Global::writeLog("work",QString("教师%1修改日志成功").arg(g.teacherNum));
        }
        else //不存在，插入
        {
            QString sqlstr;
            if(g.LoginType == Global::TYPE3)
            {
                useTime = groupnum;
                sqlstr = QString("insert into worklog("
                                 "date,week,day,lesson,tname,lname,cnum,cpresentNum,"
                                 "LaboratoryNum,lastUpdateTime,createTime,useTime,purpose,teacherID) "
                                 "values('%1','%2','%3','%4','%5','%6','%7','%8',"
                                 "'%9','%10','%10','%11','%12','%13');")
                        .arg(date,week,day,lesson,tname,lname,cnum,rnum)
                        .arg(labName,currentdateTime,useTime,purpose,g.getTeacherID());
            }
            else
            {
                sqlstr = QString("insert into worklog("
                                 "date,week,day,lesson,tname,lname,cmajor,cname,"
                                 "cnum,cpresentNum,etime,etype,erequire,groupnum,ecategory,"
                                 "LaboratoryNum,lastUpdateTime,createTime,useTime,purpose,teacherID ) "
                                 "values('%1','%2','%3','%4','%5','%6','%7','%8',"
                                 "'%9','%10','%11','%12','%13','%14','%15',"
                                 "'%16','%17','%17','%18','%19','%20');")
                        .arg(date,week,day,lesson,tname,lname,major,cname)
                        .arg(cnum,rnum,etime,etype,erequire,groupnum,ecategory)
                        .arg(labName,currentdateTime,useTime,purpose,g.getTeacherID());
            }
            bool state = g.sql.exec(sqlstr);

            //qDebug() <<"不存在"<< sqlstr ;
            if(!state)
            {
                Global::writeLog("error",QString("教师%1写入日志失败,错误:%1").arg(g.sql.lastError().text()));
                MyMessageBox box;
                box.isShowDialog(true);
                box.iniMsgBox(MyMessageBox::alert,QString("请检查你的输入是否正确，错误代码:%1").arg(g.sql.lastError().text()));
                box.exec();
                return;
            }
            Global::writeLog("work",QString("教师%1写入日志成功").arg(g.teacherNum));
        }
        g.sql.clear();
        QString tempsql = QString("select id,AttendanceTime from worklog where date = '%1' and lesson = '%2' and tname = '%3' and lname = '%4';")
                .arg(date,lesson,tname,lname);
        g.sql.exec(tempsql);
        if(g.sql.next())
        {
            g.workLogId = g.sql.value(0).toInt();
            g.AttendanceTime = g.sql.value(1).toInt();
        }
    }
    //更新已考勤记录
    g.sql.clear();
    QString updateAttSql = QString("update attendance set lessondate = '%1',lessonweek = '%2',lname = '%3',classname = '%4'"
                                   " where worklogid = '%5'")
            .arg(date,week,lname,cname,QString::number(g.workLogId,10));
    g.sql.exec(updateAttSql);

    g.week = ui->leWeek->text().toInt();  //保存周
    this->hide();
    loop->quit();
}


/**
* @brief 初始化工作日志界面
* @date 2017/12/9 (更新)
* @author han
* @return void
*/
void WorkLog::onLogin()
{   
    ui->leEtime->setText("2");
    QDateTime time;
    time = QDateTime::currentDateTime();
    QString date = time.toString("yyyy-MM-dd");
    ui->textDate->setText(date);
    ui->leXingqi->setText(time.toString("ddd"));  //设置显示星期

    QString lesson;
    double hour = time.toString("h").toDouble();
    double min = time.toString("m").toDouble();
    double hm = hour +min/60;
    QString lesson1;
    QString lesson2;
    if(hm>=7 && hm<=9.6)
    {
        ui->classTimeStartCbx->setCurrentIndex(0);
        ui->classTimeEndCbx->setCurrentIndex(1);
        lesson = "1/2";
        lesson1 = "1/";
        lesson2 = "2";
    }
    else if(hm>9.6 && hm<=11.6)
    {
        ui->classTimeStartCbx->setCurrentIndex(2);
        ui->classTimeEndCbx->setCurrentIndex(3);
        lesson = "3/4";
        lesson1 = "3/";
        lesson2 = "4";
    }
    else if(hm>=12 && hm<=16.6)
    {
        ui->classTimeStartCbx->setCurrentIndex(4);
        ui->classTimeEndCbx->setCurrentIndex(5);
        lesson = "5/6";
        lesson1 = "5/";
        lesson2 = "6";
    }
    else if(hm>16.6 && hm<=18.4)
    {
        ui->classTimeStartCbx->setCurrentIndex(6);
        ui->classTimeEndCbx->setCurrentIndex(7);
        lesson = "7/8";
        lesson1 = "7/";
        lesson2 = "8";
    }
    else if(hm>=19 && hm<=23.6)
    {
        ui->classTimeStartCbx->setCurrentIndex(8);
        ui->classTimeEndCbx->setCurrentIndex(10);
        lesson = "9/11";
        lesson1 = "9/";
        lesson2 = "10";
    }

    QString tname;
    Global g;
    g.connectdb();
    g.sql.exec(QString("select name from teacher where id =%1").arg(g.teacherNum));
    if(g.sql.next())
        tname = g.sql.value(0).toString();
    //获取实验类型
    g.sql.exec("select etype from etype  order by id");
    ui->cbxEtype->clear();
    while(g.sql.next())
    {
        ui->cbxEtype->addItem(g.sql.value(0).toString());
    }
    //获取实验类别
    g.sql.clear();
    g.sql.exec("select category from Ecategory order by id");
    ui->cbxEcategory->clear();
    while(g.sql.next())
    {
        ui->cbxEcategory->addItem(g.sql.value(0).toString());
    }
    //获取课程类别
    g.sql.clear();
    g.sql.exec("select erequire from erequire order by id");
    ui->cbxRequire->clear();
    while(g.sql.next())
    {
        ui->cbxRequire->addItem(g.sql.value(0).toString());
    }


    //获取班别名
    QString sql = QString("select DISTINCT cname from lesson where tid = '%1' order by createTime desc limit 3;")
            .arg(g.getTeacherID());
    g.sql.exec(sql);
    if(g.sql.next())
    {
        //判断教师授课门数,并写到单选框
        if(g.sql.size() == 1)
        {
            ui->classRadio1->setText(g.sql.value(0).toString());
            ui->classRadio2->hide();
            ui->classRadio3->hide();
        }
        else  if(g.sql.size() == 2)
        {
            ui->classRadio1->setText(g.sql.value(0).toString());
            g.sql.next();
            ui->classRadio2->show();
            ui->classRadio2->setText(g.sql.value(0).toString());
            ui->classRadio3->hide();
        }
        else  if(g.sql.size() == 3)
        {
            ui->classRadio1->setText(g.sql.value(0).toString());
            g.sql.next();
            ui->classRadio2->show();
            ui->classRadio2->setText(g.sql.value(0).toString());
            g.sql.next();
            ui->classRadio3->show();
            ui->classRadio3->setText(g.sql.value(0).toString());
        }
    }

    g.disconnectdb();
    ui->leTeacherName->setText(tname);  //设置教师名字

    initCourseInfo(lesson,lesson1,lesson2,tname,time);  //初始化页面信息
}

void WorkLog::onSignEnd(const QString &str, const QString &str2, const QString &str3, const QString &str4)
{
    ui->leClassName->setText(str);
    ui->leLessonName->setText(str2);
    ui->leCnum->setText(str3);
    ui->leRepresentNum->setText(str4);
}

void WorkLog::closeEvent(QCloseEvent * )
{

}

WorkLog::~WorkLog()
{
    delete ui;
}


/**
* @brief 初始化教师匹配下拉列表
* @date 2017/12/9
* @author han
* @return void
*/
void WorkLog::initTeacherList()
{
    word_list.clear(); //清空原有的
    Global g;
    g.connectdb();
    QString sql = "select name from teacher";
    g.sql.exec(sql);

    //循环添加匹配的教师名字
    for (int i = 0; i <g.sql.size(); ++i)
    {
        g.sql.next();
        word_list << g.sql.value(0).toString();  //添加
    }
    completer = new QCompleter(word_list, this);
    //completer->setModel(new QStringListModel(word_list, this));

    g.disconnectdb();

}


/**
* @brief 重写事件过滤器,实现lineedit点击事件
* @date 2017/12/9
* @author han
* @return void
*/
bool WorkLog::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        if(obj == ui->leLessonName) //如果是点击课程名称
        {
            updateLessionDrop();
        }
        else if(obj == ui->leClassName) //如果是点击班级
        {
            updateClassDrop();
        }
    }

    return QWidget::eventFilter(obj,event);
}


/**
* @brief 更新课程下拉列表
* @date 2017/12/9
* @author han
* @return void
*/
void WorkLog::updateLessionDrop()
{
    word_listForCourseName.clear();//清空原有的
    Global g;
    g.connectdb();
    QString sql = QString("select lname from lesson  left join  teacher on lesson.tid = teacher.id  where teacher.name = '%1' ").arg(ui->leTeacherName->text());
    g.sql.exec(sql);
    //qDebug() << sql;
    //循环添加匹配的课程
    for (int i = 0; i <g.sql.size(); ++i)
    {
        g.sql.next();
        word_listForCourseName << g.sql.value(0).toString();  //添加
    }

    completerForCourseName->setModel(new QStringListModel(word_listForCourseName, this));
    completerForCourseName->popup();
    g.disconnectdb();
}


/**
* @brief 更新其他班别下拉
* @date 2017/12/9
* @author han
* @return void
*/
void WorkLog::updateClassDrop()
{
    word_listForClass.clear();//清空原有的
    Global g;
    g.connectdb();
    QString sql = QString("select DISTINCT cname from lesson left JOIN teacher on lesson.tid = teacher.id where teacher.name = '%1';")
            .arg(ui->leTeacherName->text());
    g.sql.exec(sql);
    // qDebug() << sql;
    //循环添加匹配的班别
    for (int i = 0; i <g.sql.size(); ++i)
    {
        g.sql.next();
        word_listForClass << g.sql.value(0).toString();  //添加
    }

    completerForClass->setModel(new QStringListModel(word_listForClass, this));

    g.disconnectdb();
}

/**
* @brief 获取课程信息
* @date 2017/12/9
* @author han
* @return void
*/
void WorkLog::getCourseInfo()
{
    Global g;
    g.connectdb();
    QString sql = QString("select * from worklog "
                          "where tname = '%1' and lname = '%2' and cmajor = '%3' and cname = '%4' "
                          " order by createTime desc LIMIT 1")
            .arg(ui->leTeacherName->text(),ui->leLessonName->text(),ui->leMajor->text(),ui->leClassName->text());
    g.sql.exec(sql);
    //qDebug () <<sql;

    if(g.sql.next())  //更新页面课程内容
    {
        ui->leCnum->setText(g.sql.value("cnum").toString());
        ui->textGroupNum->setText(g.sql.value("groupnum").toString());
        ui->leEtime->setText(g.sql.value("etime").toString());
        ui->cbxEtype->setCurrentText(g.sql.value("etype").toString());
        ui->cbxEcategory->setCurrentText(g.sql.value("ecategory").toString());
        ui->cbxRequire->setCurrentText(g.sql.value("erequire").toString());
    }
    g.disconnectdb();
}



/**
* @brief 页面打开时后自动填充
* @date 2017/12/9
* @author han
* @return void
*/
void WorkLog::initCourseInfo(QString lesson, QString lesson1,QString lesson2,  QString teacherName, QDateTime time)
{
    //处理日期,从前一个月算起
    int year = time.toString("yyyy").toInt();
    int Month = time.toString("MM").toInt() -1;
    if(Month == 0)  //如果是1月份
    {
        year = year-1;
        Month = 12;
    }
    QString startTime = QString("%1").arg(year) + "-" + QString("%1").arg(Month) +"-1" ;

    QString day = time.toString("ddd");  //星期

    Global g;
    g.connectdb();

    QString sql;
    if(!isAlter)
        sql = QString("select * from worklog "
                          "where date >= '%1' and day = '%2' and tname = '%3' and "
                          "(lesson = '%4' or lesson like '%%5%' or lesson like '%%6%') "
                          "order by createTime desc LIMIT 1")
            .arg(startTime,day,teacherName,lesson,lesson1,lesson2);
    else
        sql = QString("select * from worklog where id='%1'").arg(g.getWorkLogID());

    g.sql.exec(sql);
    //qDebug() << sql;
    if(g.sql.next())  //更新页面课程内容
    {
        int week = g.sql.value("week").toInt() + 1;
        ui->leWeek->setText(QString::number(week,10));
        QString lastDate = g.sql.value("date").toString();
        if(lastDate == time.toString("yyyy-MM-dd"))
            ui->leWeek->setText(g.sql.value("week").toString());
        ui->leMajor->setText(g.sql.value("cmajor").toString());
        ui->leCnum->setText(g.sql.value("cnum").toString());
        ui->leEtime->setText(g.sql.value("etime").toString());
        ui->cbxEtype->setCurrentText(g.sql.value("etype").toString());
        ui->cbxEcategory->setCurrentText(g.sql.value("ecategory").toString());
        ui->cbxRequire->setCurrentText(g.sql.value("erequire").toString());
        QString lessonName = g.sql.value("lname").toString().trimmed();
        ui->leLessonName->setText(lessonName);

        //判断登录类型，修改界面内容
        if(g.LoginType != g.TYPE3)
            ui->textGroupNum->setText(g.sql.value("groupnum").toString());
        else
            ui->textGroupNum->setText(g.sql.value("usetime").toString());
        //设置节次
        QString lesson = g.sql.value("lesson").toString().trimmed();

        if(lesson.contains("/"))
        {
            QStringList list = lesson.split("/");
            QString start = list.at(0);
            QString end = list.at(1);
            ui->classTimeStartCbx->setCurrentIndex(start.toInt()-1);
            ui->classTimeEndCbx->setCurrentIndex(end.toInt()-1);
        }

        //设置班别名
        QString classname = g.sql.value("cname").toString().trimmed();

        //判断班别名是否和单选框中的一致
        if(classname == ui->classRadio1->text().trimmed())
            ui->classRadio1->click();
        else if(classname == ui->classRadio2->text().trimmed())
            ui->classRadio2->click();
        else if(classname == ui->classRadio3->text().trimmed())
            ui->classRadio3->click();
        else
        {
            ui->leClassName->setText(classname);
            ui->classRadio4->click();
        }
    }
}

/**
* @brief 班级输入完成后自动填充
* @date 2017/12/9
* @author han
* @return void
*/
void WorkLog::on_leClassName_editingFinished()
{
    classSeletedChange(ui->leClassName->text());
}

/**
* @brief 班级输入完成后自动填充
* @date 2017/12/9
* @author han
* @return void
*/
void WorkLog::on_leClassName_returnPressed()
{
    getCourseInfo();
}


/**
* @brief 返回
* @date 2017/12/12
* @author han
* @return void
*/
void WorkLog::on_returnBtn_clicked()
{
    this->hide();
}

void WorkLog::on_leClassName_selectionChanged()
{
    updateClassDrop();
}


void WorkLog::on_leLessonName_selectionChanged()
{
    updateLessionDrop();
}

void WorkLog::on_pushButton_clicked()
{
    onClick();
}

void WorkLog::on_classTimeStartCbx_currentIndexChanged(int index)
{
    if(index > ui->classTimeEndCbx->currentIndex())
        ui->classTimeEndCbx->setCurrentIndex(index);
    //设置学时
    int time = ui->classTimeEndCbx->currentIndex() - ui->classTimeStartCbx->currentIndex() +1;
    ui->leEtime->setText(QString::number(time,10));
}

void WorkLog::on_classTimeEndCbx_currentIndexChanged(int index)
{
    if(index < ui->classTimeStartCbx->currentIndex())
        ui->classTimeStartCbx->setCurrentIndex(index);
    //设置学时
    int time = ui->classTimeEndCbx->currentIndex() - ui->classTimeStartCbx->currentIndex() +1;
    ui->leEtime->setText(QString::number(time,10));
}


/**
* @brief 班级选择其他
* @date 2017/12/30
* @author han
* @return void
*/
void WorkLog::on_classRadio4_toggled(bool checked)
{
    ui->leClassName->setEnabled(checked);
    if(checked)
        ui->leClassName->setFocus();
}


/**
* @brief 设置界面用于登录类型3，隐藏相关控件
* @date 2017/12/30
* @author han
* @return void
*/
void WorkLog::initWorkLogForType3Login(bool flag)
{
    ui->label_5->setVisible(flag);
    ui->jieciW->setVisible(flag);
    ui->label_9->setVisible(flag);
    ui->classW->setVisible(flag);
    ui->label_8->setVisible(flag);
    ui->leMajor->setVisible(flag);
    ui->label_20->setVisible(flag);
    ui->label_13->setVisible(flag);
    ui->label_14->setVisible(flag);
    ui->cbxEcategory->setVisible(flag);
    ui->cbxEtype->setVisible(flag);
    ui->cbxRequire->setVisible(flag);
    ui->label_12->setVisible(flag);
    ui->leEtime->setVisible(flag);
    ui->line_2->setVisible(flag);
    ui->line_4->setVisible(flag);
    ui->line_5->setVisible(flag);
    ui->line_8->setVisible(flag);

    if(!flag)
    {
        ui->label_7->setText("教学活动名称:");
        ui->label_17->setText("使用时长(小时):");
    }
    else
    {
        ui->label_7->setText("课程名称:");
        ui->label_17->setText("每组人数:");
    }

    resize(958,350);
}
