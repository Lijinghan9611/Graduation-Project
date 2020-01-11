#include "CLassManage.h"
#include "ui_CLassManage.h"
#include "CoolTitleBar.h"
#include <QDebug>
#include <QLineEdit>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QSettings>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QException>
#include <qt_windows.h>
#include <QMenu>
#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#endif

CLassManage::CLassManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CLassManage)
{
    ui->setupUi(this);
    ispress = false;
    noShowDetail = false;
    unShowInfo = false;

    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags( Qt::FramelessWindowHint| windowFlags());  //cause combobox2 view style can not set

    connectFtpTimer = new QTimer;
    connect(connectFtpTimer,SIGNAL(timeout()),this,SLOT(onTimeOut()));


    ui->minBtn->setToolTip("最小化");
    ui->closeBtn->setToolTip("关闭");
    comparePortraitWidget = new ComparePortrait;
    connect(this,SIGNAL(beginComparePortrait(int)),comparePortraitWidget,SLOT(onBegin(int)));
    connect(this,SIGNAL(comparePortrait(QString,QImage)),comparePortraitWidget,SLOT(onClientImgCome(QString,QImage)));

    saveAttendanceAutoTimer = new QTimer(this);
    connect(saveAttendanceAutoTimer,SIGNAL(timeout()),this,SLOT(onSaveAttendanceAutoTimerTimeOut()));
    row = 0;
    portraitIndex = 0;
    signOrEndSign = true;   //点名结束标志
    allStudentPresent = false;
    signOrQuestion = true;
    isFtpTest = false;
    g.connectdb();      //necessary before model or model can't find db

    signTimer = new QTimer(this);
    checkSignStatusTimer = new QTimer(this);
    signSuccess = false;

    connect(signTimer,SIGNAL(timeout()),this,SLOT(onSignTime()));
    connect(checkSignStatusTimer,SIGNAL(timeout()),this,SLOT(onCheckSign()));


    model = new MySqlTableModel();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    ui->tableView->setModel(model);
    connect(model,SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this,SLOT(modelDataChanged(QModelIndex,QModelIndex)));
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //设置表格等宽

    //考勤表
    AttendanceModel = new MySqlTableModel();
    AttendanceModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    ui->tbViewAttendance->setModel(AttendanceModel);
    ui->tbViewAttendance->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tbViewAttendance->setAlternatingRowColors(true);
    ui->tbViewAttendance->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //设置表格等宽
    connect(AttendanceModel,SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this,SLOT(attendanceModelDataChanged(QModelIndex,QModelIndex)));

    ui->tableWidget->setColumnCount(5);
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    int count = set->value("ui/x").toInt() * set->value("ui/y").toInt();
    ui->tableWidget->setRowCount(count);  //设置行数
    delete set;

    QStringList header;
    header<<"学号"<<"姓名"<<"状态"<<"机位号"<<"答案";
    ui->tableWidget->setHorizontalHeaderLabels(header);

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setColumnHidden(4,true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //设置表格等宽

    ui->pushButton->setShortcut(Qt::Key_F5);
    ui->pushButton->setToolTip("查询全部F5");
    ui->saveBtn->setToolTip("保存Ctrl+S");
    ui->CancelBtn->setToolTip("撤销Ctrl+Z");
    ui->addRowBtn->setToolTip("插入Insert");
    ui->deleteSeletedBtn->setToolTip("删除Delete");
    ui->textEdit->setObjectName("textedit");
    ui->tabWidget->setCurrentIndex(0);

    connect(ui->lineEdit_3,SIGNAL(cursorPositionChanged(int,int)),this,SLOT(lineEditTip(int,int)));
    connect(ui->lineEdit_3,SIGNAL(returnPressed()),ui->sendAnswerBtn,SIGNAL(clicked()));
    connect(ui->tableWidget->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(onTableWidgetSort(int)));
    connect(ui->tableView->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(onTableWidgetSort(int)));
    connect(ui->tbViewAttendance->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(onTableWidgetSort(int)));

    setEditButtonEnable(false);
    ui->uploadProgressBar->setVisible(false);


    QImage temp(":/images/images/else/unsolved.png");
    QImage temp1(":/images/images/else/solved.png");
    unSolved = QPixmap::fromImage(temp).scaled(QSize(16,16), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    solved = QPixmap::fromImage(temp1).scaled(QSize(16,16), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labADsolved->setPixmap(solved);
    ui->labUnsolved->setPixmap(unSolved);

    //设置菜单类型为自定义菜单
    ui->tbViewAttendance->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->AdviceList->setContextMenuPolicy(Qt::CustomContextMenu);

    popMenu = new QMenu;
    popMenu->setMinimumWidth(200);
    copyAction = new QAction("复制",this);
    deleteAction= new QAction("删除",this);
    refreshAction= new QAction("刷新",this);
    exportAction= new QAction("导出",this);
    pasteAction = new QAction("粘贴",this);
    addAction = new QAction("添加",this);

    // 添加图标
    QIcon icon(":/images/images/action/copy.png");
    copyAction->setIcon(icon);
    QIcon icon1(":/images/images/action/delete.png");
    deleteAction->setIcon(icon1);
    QIcon icon2(":/images/images/action/refresh.png");
    refreshAction->setIcon(icon2);
    QIcon icon3(":/images/images/action/export.png");
    exportAction->setIcon(icon3);

    // 设置快捷键
    copyAction->setShortcut(QKeySequence::Copy);
    pasteAction->setShortcut(QKeySequence::Paste);

    connect(copyAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(refreshAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(exportAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(addAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));

}


/**
* @brief 开启自动点名
* @date 2018/1/8
* @author han
* @return void
*/
void CLassManage::setSignTimerStart()
{
    timeOutTime = 0;
    int totalSec = globalAttendanceTime - timeOutTime;  //剩余秒数
    int sec = totalSec % 60;
    int mm = totalSec / 60;
    QString secStr = QString::number(sec,10);
    QString mmStr = QString::number(mm,10);
    if(sec < 10)
        secStr = "0" + secStr;
    if(mm < 10)
        mmStr = "0" + mmStr;
    QString time = mmStr + ":" + secStr;
    ui->labattendantime->setText(QString("倒计时：%1").arg(time));

    signTimer->start(1000);  //点名timer开始计时
    checkSignStatusTimer->stop();  //检查timer结束
}


/**
* @brief 停止自动点名计时器
* @date 2018/1/8
* @author han
* @return void
*/
void CLassManage::setSignTimerStop()
{
    ui->labattendantime->setText(QString("倒计时：00:00"));
    signTimer->stop();
}

/**
* @brief 初始化显示课程信息
* @date 2018/1/2
* @author han
* @return void
*/
void CLassManage::initLesson()
{
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    //获取考勤时间间隔
    QString value = QString("AttendanceTime/%1").arg(g.teacherNum);
    int selectTime = set->value(value,15).toInt();
    QString currentText = QString::number(selectTime) + "分钟";
    ui->cbxAttenddanceTime->setCurrentText(currentText);
    globalAttendanceTime = selectTime * 60;  //考勤间隔
    isLastAttendanceSave = true;

    //获取ftp设置
    QString ipValue = QString("FTPServer/IP");
    QString numberValue = QString("FTPServer/Account");
    QString pwdValue = QString("FTPServer/Password");
    QString sendValue = QString("FtpSet-%1/FtpSendType").arg(g.getTeacherID());
    QString v1 =set->value(ipValue,"").toString();
    QString v2 =set->value(numberValue,"").toString();
    QString v3 =set->value(pwdValue,"").toString();
    QString v4 =set->value(sendValue,"1").toString();

    SaveTogether = set->value("FTPServer/SaveTogether",true).toBool();
    saveTogetherPath = set->value("FTPServer/ServerPath","").toString();

    //获取本地保存路径
    localSavePath = set->value("LocalSavePath/Path","AttendanceRecord").toString().trimmed();
    if(localSavePath == "AttendanceRecord" || localSavePath == "")  //默认路径
        localSavePath = QDir::currentPath() + "/"+localSavePath;
    //检查路径最后的字符是否为'/'
    QString i = localSavePath.right(1);
    if(i != "/")
        localSavePath += "/";

    if(saveTogetherPath == "")
        SaveTogether = true;
    if(v1 != "" && v2 != "" && v3 != "")
    {
        FtpServerIP = Global::getXorEncryptDecrypt(v1,21);
        FtpNumber = Global::getXorEncryptDecrypt(v2,21);
        FtpPwd = Global::getXorEncryptDecrypt(v3,21);
    }
    else
    {
        FtpServerIP = "";
        FtpNumber = "";
        FtpPwd = "";
    }
    if(v4 == "1")
    {
        sendAuto = true;
        ui->radioSendAuto->setChecked(true);
    }
    else
    {
        sendAuto = false;
        ui->radioSendManual->setChecked(true);
    }
    delete set;

    Global g;
    g.connectdb();
    QString sql = QString("select cname,lname from worklog where id = %1")
            .arg(g.getWorkLogID());
    g.sql.exec(sql);
    if(g.sql.next())
    {
        // className , lessonName
        if(g.LoginType != g.TYPE3)
        {
            lessonName = g.sql.value("lname").toString();
            className = g.sql.value("cname").toString();
            ui->labLesson->setText(QString("班级：%1 <br>课程：%2").arg(className,lessonName));
        }
        else
        {
            lessonName = g.sql.value("lname").toString();
            className = "";
            ui->labLesson->setText(QString("教学活动：%1").arg(lessonName));
        }
    }

    //获取学生信息表id
    g.sql.clear();
    sql = QString("select cid from lesson where tid = '%1' and lname = '%2' ")
            .arg(QString::number(g.teacherNum),lessonName);
    g.sql.exec(sql);
    classId = -1;
    if(g.sql.next())
    {
        classId = g.sql.value(0).toInt();
    }

    //判断是否已经导入学生信息
    g.sql.exec("select COUNT(*) from class"+classId);
    if(g.sql.next())
    {
        if(g.sql.value(0).toInt() == 0)
        {
            ui->importBtn->setEnabled(false);
            ui->importBtn->setText("学生名单已导入");
        }
        else
        {
            ui->importBtn->setText("导入学生名单");
            ui->importBtn->setEnabled(true);
        }
    }
    else
    {
        ui->importBtn->setText("学生名单已导入");
        ui->importBtn->setEnabled(false);
    }

    initDrop();
    setSignTimerStart();  //开启点名倒计时
}


/**
* @brief 初始化考勤下拉
* @date 2018/1/4
* @author han
* @return void
*/
void CLassManage::initDrop()
{
    Global g;
    g.connectdb();

    noShowDetail = true;
    g.sql.exec(QString("select cname,lname from lesson where tid = %1").arg(QString::number(g.teacherNum)));

    ui->cbxLessonAndClass->clear();  //清空原有选项
    int i =0;
    int target = 0;
    initDropBool = true;
    bool haveLesson = false;
    while(g.sql.next())
    {
        QString cname = g.sql.value(0).toString();
        QString lname = g.sql.value(1).toString();
        if(lname == lessonName)  //当前课程
        {
            target = i;
            haveLesson = true;
        }
        ui->cbxLessonAndClass->addItem(lname + " " + cname);
        i++;
    }

    //如果没有该课程,则创建一次性课程
    if(!haveLesson)
    {
        QString cname = className;
        QString lname = lessonName;
        ui->cbxLessonAndClass->addItem(lname + " " + cname);
    }
    initDropBool = false;
    ui->cbxLessonAndClass->setCurrentIndex(target);
    ui->tab3searchAttendanceBtn->click();

    noShowDetail = false;
}

void CLassManage::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget * pw = this->window();
    if(pw->isTopLevel() && pw->isMaximized())
    {
        ui->maxBtn->setStyleSheet("QPushButton#maxBtn{background-image:url(:/images/images/TiTleBar/btn_restore_normal.png);background-color:transparent;background-repeat:no-repeat;background-position:center;}"
                                  "QPushButton#maxBtn:hover {background:url(:/images/images/TiTleBar/btn_restore_highlight.png),transparent;background-repeat:no-repeat;background-position:center;}"
                                  "QPushButton#maxBtn:pressed{background:url(:/images/images/TiTleBar/btn_restore_down.png),transparent;background-repeat:no-repeat;background-position:center;}");
        ui->maxBtn->setToolTip("恢复");

    }
    else
    {
        ui->maxBtn->setStyleSheet("QPushButton#maxBtn{background-image:url(:/images/images/TiTleBar/btn_max_normal.png);background-color:transparent;background-repeat:no-repeat;background-position:center;}"
                                  "QPushButton#maxBtn:hover {background:url(:/images/images/TiTleBar/btn_max_highlight.png),transparent;background-repeat:no-repeat;background-position:center;}"
                                  "QPushButton#maxBtn:pressed{background:url(:/images/images/TiTleBar/btn_max_down.png),transparent;background-repeat:no-repeat;background-position:center;}");
        ui->maxBtn->setToolTip("最大化");
    }

    QWidget::paintEvent(ev);

}

/**
* @brief 设置操作学生信息表格的Button是否可以操作
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::setEditButtonEnable(bool f)
{
    ui->CancelBtn->setEnabled(f);
    ui->saveBtn->setEnabled(f);
}

/**
* @brief 查询全部学生数据
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_pushButton_clicked()
{
    deleteInfoRow.clear();
    g.connectdb();
    QString table = "class" + QString::number(classId);
    //设置表头
    model->setTable(table);  //设置表名
    model->select();
    model->setHeaderData(0,Qt::Horizontal,"学号");
    model->setHeaderData(1,Qt::Horizontal,"姓名");
    model->setHeaderData(2,Qt::Horizontal,"性别");
    model->setHeaderData(3,Qt::Horizontal,"点名数");
    model->setHeaderData(4,Qt::Horizontal,"缺课数");
    model->setHeaderData(5,Qt::Horizontal,"班别");
    model->setHeaderData(6,Qt::Horizontal,"人像路径");
    sqlShow(model->rowCount());
}


/**
* @brief 数据被改变
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::modelDataChanged(QModelIndex , QModelIndex )
{
    setEditButtonEnable(true);
}

void CLassManage::lineEditTip(int a, int b)
{
    QLineEdit * pLineEdit = qobject_cast<QLineEdit*>(sender());
    if (a == -1 && b == 0)
    {
        if (pLineEdit == ui->lineEdit_3)
            ui->textEdit->append("请输入你的问题");
    }
}

void CLassManage::onTableWidgetSort(int i)
{
    ui->tableWidget->sortByColumn(i,Qt::AscendingOrder);
    ui->tableView->sortByColumn(i,Qt::AscendingOrder);
    ui->tbViewAttendance->sortByColumn(i,Qt::AscendingOrder);
}

void CLassManage::sqlShow(int i)
{
    if(noShowDetail)
        return;
    if(i==0)
    {
        ui->textEdit->append("查询成功,无匹配值.\n");
        return;
    }
    if(i==-1)
    {
        ui->textEdit->append("查询失败,请检查您输入的值.\n");
        return;
    }
    ui->textEdit->append(QString("查询成功，匹配到 %1 行记录.\n").arg(i));
}

/**
* @brief 点名结果
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::onSignMg(const QString& ip, const QString& id, const QString& name)
{
    emit signMgToolTip(ip,id,name);  //set tool tip

    if(signOrEndSign)  //如果没有开始点名,处理无教师上课情况
        return;

    int index = 0;  //m_class index
    QString position = ipToPosition(ip);
    bool exit = false;
    //更新考勤记录
    QDateTime time = QDateTime::currentDateTime();
    QString lessondate = time.toString("yyyy-MM-dd");
    //获取当前最新考勤次数
    int AttendanceTime = getCurrentAttendanceTime();
    QString lessonweek = QString::number(g.week,10);//周次
    QString Time1 = QString::number(AttendanceTime,10);  //考勤次数

    foreach (QStringList list, m_class)
    {
        //添加
        if(list.at(0) == id && list.at(1) == name)  //client input correct
        {
            int temp = AttendanceTime - 1;
            QString Time = QString::number(temp,10);
            Global g;

            QString sql = QString("select AttendanceResult from Attendance where lessondate = '%1' "
                                  "and lname = '%2' and studentid = '%3' and studentname = '%4'"
                                  " and AttendanceTime = '%5' and worklogid = '%6'")
                    .arg(lessondate,lessonName,id,name,Time,g.getWorkLogID());
            //查找上一次的考勤结果
            g.sql.exec(sql);
            QString AttendanceResult = "已签到";
            if(g.sql.next())
            {
                QString result = g.sql.value(0).toString().trimmed();
                if(result == "迟到")  //如果上一次点名为迟到的
                {
                    AttendanceResult = "迟到";
                }
            }

            QString table = "class" + QString::number(classId);
            g.sql.exec(QString("update %1 set absence = absence-1 where id = %2 and name = '%3'").arg(table,id,name));

            QString sqlstr = QString("update Attendance set AttendanceResult = '%6',position = '%8' "
                                     "where studentid = '%1' and studentname = '%2' and lname = '%3' "
                                     " and lessondate = '%4' and AttendanceTime = '%5' and worklogid = '%7' ")
                    .arg(id,name,lessonName,lessondate,Time1,AttendanceResult,g.getWorkLogID(),position);
            g.sql.exec(sqlstr);

            //添加到表格显示
            if(ui->tableWidget->rowCount() <= row)  //如果表格行数不够
                ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row,0,new QTableWidgetItem(id));
            ui->tableWidget->setItem(row,1,new QTableWidgetItem(name));
            ui->tableWidget->setItem(row,2,new QTableWidgetItem("已签到"));
            ui->tableWidget->item(row,2)->setForeground(Qt::green);
            ui->tableWidget->setItem(row,3,new QTableWidgetItem(position));
            //居中
            ui->tableWidget->item(row,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget->item(row,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget->item(row,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget->item(row,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);


            m_map.insert(ip,id);
            m_class.removeAt(index);  //移去已经签到的人
            row ++;
            exit = true;
            break;  //结束循环
        }
        index ++;
    }

    if(!exit)  //客户端出错或者没有匹配到
    {

        if(ui->tableWidget->rowCount() <= row)  //如果表格行数不够
            ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,0,new QTableWidgetItem(id));
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(name));
        ui->tableWidget->setItem(row,2,new QTableWidgetItem("不匹配"));
        ui->tableWidget->item(row,2)->setForeground(Qt::darkYellow);
        ui->tableWidget->setItem(row,3,new QTableWidgetItem(position));
        //居中
        ui->tableWidget->item(row,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(row,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(row,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(row,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        row ++;

        //把不匹配的添加到考勤记录
        QString sqlstr = QString("insert into Attendance(lessondate,lessonweek,lname,classname,studentid,studentname,attendancetime,attendanceResult,worklogid,position)"
                                 " value('%1','%2','%3','%4','%5','%6','%7','已签到','%8','%9')")
                .arg(lessondate,lessonweek,lessonName,className,id,name,Time1,g.getWorkLogID(),position);
        g.sql.exec(sqlstr);  //添加
    }


    if(row == Global::hostNum)        //点名结束
    {
        Global::writeLog("work",QString("教师%1,点名结束").arg(g.getTeacherID()));
        g.sql.clear();
        ui->tableWidget->sortByColumn(0,Qt::AscendingOrder);
        AttendanceTime--;
        for(int i =0; i<m_class.size();i++)   //缺席的人
        {
            QString id = m_class.at(i).at(0);
            QString name = m_class.at(i).at(1);
            QString result = "";
            if(AttendanceTime > 0)  //如果不是第一次考勤，则判断是不是早退
            {
                QString Time = QString::number(AttendanceTime,10);
                Global g;

                QString sql = QString("select AttendanceResult from Attendance where lessondate = '%1' and lname = '%2' "
                                      "and studentid = '%3' and studentname = '%4' and AttendanceTime = '%5' and worklogid = '%6' ")
                        .arg(lessondate,lessonName,id,name,Time,g.getWorkLogID());
                //查找上一次的考勤结果
                g.sql.exec(sql);
                if(g.sql.next())
                {
                    result = g.sql.value(0).toString().trimmed();
                    if(result == "已签到" || result == "早退")  //消失的人设置为早退
                    {
                        result = "早退";
                        QString sqlstr = QString("update Attendance set AttendanceResult = '早退' where studentid = '%1' and studentname = '%2' "
                                                 "and lname = '%3' and lessondate = '%4' and AttendanceTime = '%5' and worklogid = '%6'")
                                .arg(id,name,lessonName,lessondate,Time1,g.getWorkLogID());
                        g.sql.exec(sqlstr);
                    }
                }
            }

            if(result.trimmed().isEmpty())
            {
                result = "缺勤";
            }
            if(ui->tableWidget->rowCount() <= row)  //如果表格行数不够
                ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row,0,new QTableWidgetItem(id));
            ui->tableWidget->setItem(row,1,new QTableWidgetItem(name));
            ui->tableWidget->setItem(row,2,new QTableWidgetItem(result));
            if(result == "早退")
                ui->tableWidget->item(row,2)->setForeground(Qt::blue);
            else
                ui->tableWidget->item(row,2)->setForeground(Qt::red);
            //居中
            ui->tableWidget->item(row,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget->item(row,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget->item(row,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            row ++;

        }
        ui->textEdit->append(QString("班级人数:%1  上线主机数:%2   缺勤数:%3   到课率:%4%\n")
                             .arg(number).arg(Global::hostNum).arg(m_class.size()).arg((((double)number-(double)m_class.size())/(double)number)*100));
        signSuccess = true;  //点名成功
        if(m_class.size() == 0)
            allStudentPresent = true;

        //登录界面处理
        emit signEnd(className,lessonName,QString::number(number),QString::number(number-m_class.size()));
        m_class.clear();
        ui->callNameBtn->setText("点名");
        signOrEndSign = true;
        ui->sendAnswerBtn->setEnabled(true);
        ui->sendAnswerBtn->setStyleSheet("QPushButton{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(46,46,46),stop:1 rgb(66,66,66));color:white;padding:4px;border-radius:6px;}"
                                         "QPushButton:hover{background-color:rgb(80,80,80);}"
                                         "QPushButton:pressed{background-color:rgb(30,30,30)}");
        //开启自动点名
        setSignTimerStart();
    }
}


/**
* @brief 自动保存计时器超时
* @date 2018/1/22
* @author han
* @param
* @return void
*/
void CLassManage::onSaveAttendanceAutoTimerTimeOut()
{
    saveAttendanceAuto(true);
}

/**
* @brief 考勤结束后15分钟自动保存考勤记录，本地+在线Ftp
* @date 2017/12/16
* @author han
* @param unShowInfo 为真时不显示弹窗。
*  @param isTimeOut 标记是否为计时器超时触发
* @return void
*/
void CLassManage::saveAttendanceAuto(bool isTimeOut)
{
    MyMessageBox box;
    if(!signOrEndSign)
    {
        box.iniMsgBox(MyMessageBox::alert,QString("正在点名，请先结束."));
        box.exec();
        return;
    }

    isLastAttendanceSave = true;  //标记上一次考勤记录以保存
    saveAttendanceAutoTimer->stop();

    Global g;
    g.connectdb();

    QString lessonDate = QDateTime::currentDateTime().toString("yyyy年MM月dd日");

    //获取节次
    QString sqlt = QString("select lesson from worklog where id = '%1'")
            .arg(g.getWorkLogID());
    g.sql.exec(sqlt);
    QString lesson = "";
    if(g.sql.next())
    {
        lesson = g.sql.value(0).toString().replace("/","至");
    }

    //本地存储目录处理
    QString path = localSavePath;
    QDir dir;
    dir.setPath(path);
    if(!dir.exists())  //目录不存在,创建新目录
    {
        if(!dir.mkpath(path))
        {
            box.iniMsgBox(MyMessageBox::error,"创建目录失败，请手动创建目录" + path);
            box.isShowDialog(true);
            box.exec();
            return;
        }
    }

    //考勤次数
    QString tempTime = QString::number(getCurrentAttendanceTime(),10);
    QString name = QString("%1%2%3第%4节第%5次考勤记录.xlsx")
            .arg(lessonDate,lessonName,className,lesson,tempTime).trimmed();
    QString fileName =path + name;

    ExcelManager ex;  //AttendanceModel
    ex.ExportToExcel(AttendanceModel,fileName);

    ui->textEdit->append(QString("自动保存考勤记录成功,文件路径 %1<br>").arg(fileName));

    if(isTimeOut && !sendAuto) //如果设置了手动发送到Ftp,则在计时器timeout时不发送考勤文件.
        return;

    //Ftp
    FtpManager *ftp = new FtpManager(FtpNumber,FtpPwd,FtpServerIP);
    qDebug() << FtpNumber << FtpPwd<< FtpServerIP;
    QString serverPath;
    QString serverName = QDateTime::currentDateTime().toString("考勤yyyy_MM_dd_hh_mm_sss") + ".xlsx";

    if(SaveTogether)  //存在一起
    {
        serverPath = saveTogetherPath.trimmed();
        //处理格式，以防出错
        serverPath = serverPath.replace(":","");
        QStringList list;
        if(serverPath.contains("/"))
            list = serverPath.split("/");
        else
            list.append(serverPath);
        serverPath = "";
        foreach(QString s ,list)
        {
            if(!s.trimmed().isEmpty())
                serverPath +=  s + "/";
        }
        serverPath += serverName;

        ftp->uploadFile(fileName,serverPath);
    }   
    else  //各自保存
    {
        g.connectdb();
        QString sql = QString("select name from teacher where id = '%1'").arg(g.getTeacherID());
        g.sql.exec(sql);
        QString tname;
        if(g.sql.next())
            tname = g.sql.value(0).toString();
        serverPath = tname + "/" + serverName;
        ftp->uploadFile(fileName,serverPath);
    }
    qDebug() << fileName << serverPath;
    reply = ftp->getRelay();  //获取
    connect(reply, SIGNAL(finished()), this, SLOT(OnFinishRelay()));
 }

/**
* @brief 收到答案
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::onAnswerMg(const QString& ip , const QString& id, const QString& name , const QString& answer)
{
    QString position = ipToPosition(ip);
    ui->tableWidget->setItem(row,0,new QTableWidgetItem(id));
    ui->tableWidget->setItem(row,1,new QTableWidgetItem(name));
    if(answer == rightAnswer&&rightAnswer!="")
    {
        ui->tableWidget->setItem(row,2,new QTableWidgetItem("正确"));
        ui->tableWidget->item(row,2)->setForeground(Qt::green);
        rightCount ++;
    }
    else
    {
        ui->tableWidget->setItem(row,2,new QTableWidgetItem("未匹配"));
        ui->tableWidget->item(row,2)->setForeground(Qt::darkYellow);
    }
    ui->tableWidget->setItem(row,3,new QTableWidgetItem(position));
    ui->tableWidget->setItem(row,4,new QTableWidgetItem(answer));
    row ++;
    if(row == Global::hostNum)
        cheakAnswer();
}


/**
* @brief 检查答案
* @date 2017/12/13
* @author han
* @return void
*/
void CLassManage::cheakAnswer()
{
    for(int i = 0;i < row ; i++)
    {
        if(ui->tableWidget->item(i,4)->text()==rightAnswer &&
                rightAnswer!="" &&
                ui->tableWidget->item(i,2)->text() !="正确")
        {
            ui->tableWidget->item(i,2)->setText("正确");
            ui->tableWidget->item(i,2)->setForeground(Qt::green);
            rightCount ++;
        }
        else
        {
            if(ui->tableWidget->item(i,4)->text()!=rightAnswer &&rightAnswer!="")
            {
                ui->tableWidget->item(i,2)->setText("错误");
                ui->tableWidget->item(i,2)->setForeground(Qt::red);
            }

        }
    }

    if(row == Global::hostNum)
    {
        ui->textEdit->append(QString("上线主机数 %1  答对人数%2  正确率%3%\n")
                             .arg(Global::hostNum).arg(rightCount).arg(double(rightCount)/(double)Global::hostNum*100));
        ui->callNameBtn->setEnabled(true);
        ui->callNameBtn->setStyleSheet("QPushButton{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(46,46,46),stop:1 rgb(66,66,66));color:white;padding:4px;border-radius:6px;}"
                                       "QPushButton:hover{background-color:rgb(80,80,80);}"
                                       "QPushButton:pressed{background-color:rgb(30,30,30)}");
        ui->sendAnswerBtn->setEnabled(true);
        ui->sendAnswerBtn->setStyleSheet("QPushButton{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(46,46,46),stop:1 rgb(66,66,66));color:white;padding:4px;border-radius:6px;}"
                                         "QPushButton:hover{background-color:rgb(80,80,80);}"
                                         "QPushButton:pressed{background-color:rgb(30,30,30)}");
    }
}

QString CLassManage::ipToPosition(const QString &ip)
{
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    QString positon = set->value("client/" +ip).toString();
    delete set;

    return positon;
}

void CLassManage::closeEvent(QCloseEvent*)
{


    ui->tabWidget->setCurrentIndex(0);
    ui->lineEdit_3->clear();
    ui->callNameBtn->setEnabled(true);
    ui->callNameBtn->setStyleSheet("QPushButton{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(46,46,46),stop:1 rgb(66,66,66));color:white;padding:4px;border-radius:6px;}"
                                   "QPushButton:hover{background-color:rgb(80,80,80);}"
                                   "QPushButton:pressed{background-color:rgb(30,30,30)}");
    ui->sendAnswerBtn->setEnabled(true);
    ui->sendAnswerBtn->setStyleSheet("QPushButton{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(46,46,46),stop:1 rgb(66,66,66));color:white;padding:4px;border-radius:6px;}"
                                     "QPushButton:hover{background-color:rgb(80,80,80);}"
                                     "QPushButton:pressed{background-color:rgb(30,30,30)}");

}


/**
* @brief 重新登录
* @date 2017/12/17
* @author han
* @return void
*/
void CLassManage::onLogOut()
{
    ui->textEdit->clear();
    model->setTable("");
    model->select();
    ui->tableWidget->clearContents();
    ui->lineEdit_3->clear();
    lessonName = "";
    className = "";
}


/**
* @brief 自动点名
* @date 2017/12/13
* @author han
* @return void
*/
void CLassManage::onSignTime()
{

    QString table = "class" + QString::number(classId); //班别表
    Global g;
    g.connectdb();

    //检查是否已经导入名单
    QString checkStudentCoutSql = QString("select * from %1").arg(table);
    g.sql.exec(checkStudentCoutSql);
    if(!g.sql.next())
    {
        return;
    }
    timeOutTime++;
    if(timeOutTime != globalAttendanceTime)  //倒计时未结束
    {
        int totalSec = globalAttendanceTime - timeOutTime;  //剩余秒数
        int sec = totalSec % 60;
        int mm = totalSec / 60;
        QString secStr = QString::number(sec,10);
        QString mmStr = QString::number(mm,10);
        if(sec < 10)
            secStr = "0" + secStr;
        if(mm < 10)
            mmStr = "0" + mmStr;
        QString time = mmStr + ":" + secStr;
        ui->labattendantime->setText(QString("倒计时：%1").arg(time));
        return;
    }

    timeOutTime = 0;
    setSignTimerStop();  //停止自动点名计时器
    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::information,"即将进行点名,15分钟后将自动执行点名.<br>15:00");
    box.startHideTimer(15*60);
    if(box.exec())
    {
        ui->callNameBtn->click();  //点名
        checkSignStatusTimer->start(60000);  //1分钟后检查是否点名结束
    }
    else
    {
        setSignTimerStart();
    }

}

/**
* @brief 检查是否已经结束点名
* @date 2017/12/13
* @author han
* @return void
*/
void CLassManage::onCheckSign()
{  
    if(signSuccess)  //点名结束
    {
        setSignTimerStart();
    }

}


/**
* @brief 导入人像
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::onIniPortrait(const QString&ip,const QImage &img)
{
    QString studentId = m_map.value(ip);
    QString path = "studentPortrait/" + studentId +".jpg";
    img.save(path);
    QString table = "class" + QString::number(classId);
    Global g;
    g.connectdb();
    bool state = g.sql.exec(QString("update %1 set portrait='%2' where id =%3").arg(table,path,studentId));
    if(state)
        portraitIndex ++;
    if(portraitIndex == Global::hostNum)
    {
        ui->textEdit->append("导入人像成功!");
        portraitIndex = 0;
        Global::writeLog("work",QString("教师%1导入人像成功").arg(g.getTeacherID()));
    }
}

void CLassManage::onComparePortrait(const QString &ip, const QImage &img)
{
    QString id = m_map.value(ip);
    emit comparePortrait(id,img);
}

CLassManage::~CLassManage()
{
    if(!isLastAttendanceSave)  //上一次考勤记录未保存
    {
        qDebug() << "~CLassManage()isLastAttendanceSave";
        unShowInfo = true;
        saveAttendanceAuto(true);  //自动保存考勤记录
    }

    delete comparePortraitWidget;
    delete ui;
}



/**
* @brief 提问
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_sendAnswerBtn_clicked()
{
    QString questionDetail = ui->lineEdit_3->text().trimmed();
    if(Global::hostNum == 0)
    {
        SingleTonSToast::getInstance().setMessageVDuration("无主机上线", 6000,1);
        return;
    }
    if(questionDetail.isEmpty())
    {
        SingleTonSToast::getInstance().setMessageVDuration("请输入问题", 6000,1);
        return;
    }
    ui->tableWidget->setColumnHidden(4,false);
    ui->tableWidget->clearContents();
    ui->tabWidget->setCurrentIndex(2);
    row = 0;
    rightCount = 0;
    rightAnswer="";
    emit question(questionDetail);
    ui->sendAnswerBtn->setEnabled(false);
    ui->sendAnswerBtn->setStyleSheet("QPushButton{background:grey;}");
    ui->callNameBtn->setEnabled(false);
    ui->callNameBtn->setStyleSheet("QPushButton{background:grey;}");
    signOrQuestion = false;
    Global::writeLog("work",QString("教师%1开始提问").arg(g.getTeacherID()));
}


/**
* @brief 开始、停止点名
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_callNameBtn_clicked()
{
    MyMessageBox box;

    QString table = "class" + QString::number(classId); //班别表
    g.connectdb();

    //检查是否已经导入名单
    QString checkStudentCoutSql = QString("select * from %1").arg(table);
    g.sql.exec(checkStudentCoutSql);
    if(!g.sql.next())
    {
        SingleTonSToast::getInstance().setMessageVDuration("请先导入名单", 6000,1);
        return;
    }

    if(signOrEndSign)        //begin sign
    {
        if(Global::hostNum == 0)
        {
            Global::writeLog("work",QString("教师%1点名失败，无主机上线.").arg(g.getTeacherID()));
            SingleTonSToast::getInstance().setMessageVDuration("无主机上线", 6000,1);
            return;
        }
        if(signSuccess)
        {
            box.iniMsgBox(MyMessageBox::information,QString("是否再考勤一次？"));
            if(!box.exec())
                return;
        }

        if(!isLastAttendanceSave)  //上一次考勤记录未保存
        {
            unShowInfo = true;
            saveAttendanceAuto(true);  //自动保存考勤记录
        }
        isLastAttendanceSave = false;  //标记本次考勤记录未保存
        insertAllCallNameInfo();  //插入全部点名记录
        m_class.clear();
        m_map.clear();
        ui->callNameBtn->setText("点名结束");
        signOrEndSign = false;
        if(!g.sql.exec(QString("update %1 set absence = absence+1").arg(table)))
        {
            Global::writeLog("error",QString("教师%1点名失败,错误代码:%2").arg(g.getTeacherID(),g.sql.lastError().text()));
            box.iniMsgBox(MyMessageBox::error,QString("点名失败,错误代码:%1").arg(g.sql.lastError().text()));
            box.isShowDialog(true);
            box.exec();
            return;
        }
        g.sql.clear();
        if(!g.sql.exec(QString("update %1 set time = time+1").arg(table)))
        {
            Global::writeLog("work",QString("教师%1点名失败,错误代码:%2").arg(g.getTeacherID(),g.sql.lastError().text()));
            box.iniMsgBox(MyMessageBox::error,QString("点名失败,错误代码:%1").arg(g.sql.lastError().text()));
            box.isShowDialog(true);
            box.exec();
            return;
        }
        g.sql.clear();

        if(!g.sql.exec(QString("select id,name from %1").arg(table)))
        {
            Global::writeLog("work",QString("教师%1点名失败,错误代码:%2").arg(g.getTeacherID(),g.sql.lastError().text()));
            box.iniMsgBox(MyMessageBox::error,QString("点名失败,错误代码:%1").arg(g.sql.lastError().text()));
            box.isShowDialog(true);
            box.exec();
            return;
        }

        ui->sendAnswerBtn->setEnabled(false);
        ui->sendAnswerBtn->setStyleSheet("QPushButton{background:grey;}");
        ui->tableWidget->setColumnHidden(4,true);

        while(g.sql.next())
        {
            QStringList list;
            list<<g.sql.value(0).toString()<<g.sql.value(1).toString();
            m_class.append(list);
        }
        number = m_class.size();
        row = 0;
        ui->tableWidget->clearContents();
        ui->tabWidget->setCurrentIndex(2);
        emit beginSign();
        Global::writeLog("work",QString("教师%1开始点名").arg(g.getTeacherID()));
        setSignTimerStop();

        ui->textEdit->append("点名已开始");
        signOrQuestion = true;
        ui->sendToFtpBtn->setEnabled(true);  //发送到Ftp按键设为true
        saveAttendanceAutoTimer->start(15*60*1000);

        initDrop();  //重新初始化下拉列表
    }
    else  //retrieve sign
    {
        box.iniMsgBox(MyMessageBox::information,"正在点名，是否提前结束?");
        if(!box.exec())
            return;
        ui->callNameBtn->setText("点名");
        signOrEndSign = true;
        emit retrieveSign();
        ui->tableWidget->setColumnHidden(4,true);
        Global::writeLog("work",QString("教师%1手动结束点名").arg(g.getTeacherID()));

        //开启自动点名
        setSignTimerStart();
    }
}


/**
* @brief 开始点名时,插入全部的点名记录,默认全部为缺勤
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::insertAllCallNameInfo()
{
    Global g;
    QDateTime t = QDateTime::currentDateTime();
    QString date = t.toString("yyyy-MM-dd");
    g.connectdb();

    //获取当前周次
    QString week = QString::number(g.week, 10);

    //获取当前考勤次数
    g.AttendanceTime++;
    int AttendanceTime = getCurrentAttendanceTime();
    QString Time = QString::number(AttendanceTime,10);

    g.sql.exec(QString("update worklog set AttendanceTime = AttendanceTime + 1 where id = '%1'").arg(g.getWorkLogID()));

    //查询学生的信息,防止后面信息获取失败
    noShowDetail = true;
    ui->pushButton->click();
    noShowDetail = false;

    QString worklogID = g.getWorkLogID();
    for(int i = 0;i < model->rowCount();i++)
    {
        QModelIndex idIndex=model->index(i,0,QModelIndex());//获取学号
        QString studentID= idIndex.data().toString();//
        QModelIndex nameIndex=model->index(i,1,QModelIndex());//获取姓名
        QString studentName= nameIndex.data().toString();//
        QModelIndex cnameIndex=model->index(i,5,QModelIndex());//获取班别
        QString cName= cnameIndex.data().toString();//

        QString sql = QString("insert into Attendance(lessondate,lessonweek,lname,classname,studentid,studentname,AttendanceTime,AttendanceResult,worklogid) "
                              "value('%1','%2','%3','%4','%5','%6',%7,'缺勤','%8')")
                .arg(date,week,lessonName,cName,studentID,studentName,Time,worklogID);
        g.sql.exec(sql);
    }
    Global::writeLog("work",QString("教师%1点名，插入初始考勤记录").arg(g.getTeacherID()));
}
/**
* @brief 导出学生数据
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_exportStudentInfoBtn_clicked()
{
    MyMessageBox box;
    QString table = "class" + QString::number(classId);
    g.connectdb();

    QString fileName;
    QString name = "考勤统计" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
#ifdef Q_OS_WIN
    fileName = QFileDialog::getSaveFileName(this,"导出数据","c:/" + name,"Excel(*.xlsx);;学生数据(*.csv);;文本文件(*.txt)");
#else
    fileName = QFileDialog::getSaveFileName(this,"导出数据","home/" + name,"Excel(*.xlsx);;学生数据(*.csv);;文本文件(*.txt)");
#endif
    if(fileName == "")
        return;
    QFileInfo fileinfo = QFileInfo(fileName);
    //文件后缀
    QString file_suffix = fileinfo.suffix();
    if(file_suffix != "xlsx")  //导出csv，txt格式
    {
        QFile csv(fileName);
        if(!csv.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            Global::writeLog("error",QString("教师%1导出学生信息失败，文件打开失败").arg(g.getTeacherID()));
            box.iniMsgBox(MyMessageBox::error,"数据导出失败");
            box.isShowDialog(true);
            box.exec();
            return;
        }
        QTextStream out(&csv);
        QString line = "学号,姓名,性别,点名次数,缺勤次数,班别";
        out<<line<<"\n";
        g.sql.exec(QString("select * from %1").arg(table));
        while(g.sql.next())
        {
            line = g.sql.value(0).toString() + ',' +g.sql.value(1).toString()
                    + ',' + g.sql.value(2).toString() + ',' + g.sql.value(3).toString()
                    + ',' +g.sql.value(4).toString()+ ',' +g.sql.value(5).toString();
            out<<line<<"\n";
        }
    }
    else
    {
        QStringList list;
        list.append("学号");
        list.append("姓名");
        list.append("性别");
        list.append("点名次数");
        list.append("缺勤次数");
        list.append("班别");

        ExcelManager ex;
        ex.ExportToExcel(list,model,fileName);
    }
    SingleTonSToast::getInstance().setMessageVDuration("导出成功", 5000);

    Global::writeLog("work",QString("教师%1导出学生信息成功").arg(g.getTeacherID()));
    ui->textEdit->append(QString("数据导出成功,文件路径 %1").arg(fileName));
}


/**
* @brief 导入学生数据
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_importBtn_clicked()
{
    MyMessageBox box;

    QString table = "class" + QString::number(classId);
    g.connectdb();
    ui->tabWidget->setCurrentIndex(1);
    int dataRow =0;
    int successRow=0;
    QString time,absence;
    QString fileName = QFileDialog::getOpenFileName(this,"打开文件",qApp->applicationDirPath()
                                                    ,"excel(*.xlsx *.xls);;flies(*.csv)");
    if(fileName =="")
        return;
    QFileInfo fileinfo = QFileInfo(fileName);
    //文件后缀
    QString file_suffix = fileinfo.suffix();
    QString result = "";
    if(file_suffix != "xls" && file_suffix != "xlsx")  //导入csv，txt格式
    {
        QFile csv(fileName);
        if(!csv.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            Global::writeLog("error",QString("教师%1导入学生信息失败：文件打开失败").arg(g.getTeacherID()));
            box.iniMsgBox(MyMessageBox::error,"文件打开失败");
            box.isShowDialog(true);
            box.exec();
            return;
        }
        QStringList list;
        QTextStream stream(&csv);
        ui->textEdit->append("正在导入数据...");
        while(!stream.atEnd())  //按行读取数据
        {
            QString line = stream.readLine();
            if(line == "学号,姓名,性别,班别") //排除数据行
                continue;
            dataRow ++;
            list.clear();
            if(!line.contains(",")) //错误数据行
                continue;
            list = line.split(",",QString::KeepEmptyParts);
            if(list.size() == 1)
            {
                if(g.sql.exec(QString("insert into %1(id) values(%2)") .arg(table,list.at(0))))
                    successRow ++;
            }
            else if(list.size() == 2)
            {
                if(g.sql.exec(QString("insert into %1(id,name) values(%2,'%3')")
                              .arg(table,list.at(0),list.at(1))))
                    successRow ++;
            }
            else if(list.size() == 3)
            {
                if(g.sql.exec(QString("insert into %1(id,name,sex) values(%2,'%3','%4')")
                              .arg(table,list.at(0),list.at(1),list.at(2))))
                    successRow ++;
            }
            else if(list.size() == 4)  //标准，模板
            {
                if(g.sql.exec(QString("insert into %1(id,name,sex,classname) values(%2,'%3','%4','%5')")
                              .arg(table,list.at(0),list.at(1),list.at(2),list.at(3))))
                    successRow ++;
            }
            else  if(list.size() == 5)
            {
                if(list.at(4)!="")
                    time = list.at(4);
                else time = "0";
                if(g.sql.exec(QString("insert into %1(id,name,sex,classname,time) values(%2,'%3','%4','%5',%6)")
                              .arg(table,list.at(0),list.at(1),list.at(2),list.at(3),time)))
                    successRow ++;
            }
            else if(list.size() == 6)
            {
                if(list.at(4)!="")
                    time = list.at(4);
                else time = "0";
                if(list.at(5)!="")
                    absence = list.at(5);
                else absence = "0";
                if(g.sql.exec(QString("insert into %1(id,name,sex,classname,time,absence) values(%2,'%3','%4','%5',%6,%7)")
                              .arg(table,list.at(0),list.at(1),list.at(2),list.at(3),time,absence)))
                    successRow ++;
            }
            else
                continue;
        }
    }
    else
    {
        ExcelManager ex;
        result = ex.ReadStuInfoExcelFile(fileName,table,&dataRow,&successRow);
    }

    int failRow = successRow-dataRow;
    if(failRow < 0)
        failRow = 0;

    SingleTonSToast::getInstance().setMessageVDuration("导入成功", 5000);

    Global::writeLog("work",QString("教师%1导入学生信息成功").arg(g.getTeacherID()));
    ui->textEdit->append(QString("成功导入 %1 条数据,").arg(successRow)+QString("失败:%1\n").arg(failRow));
    if(result != "")
    {
        ui->textEdit->append(result);
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,result);
        box.isShowDialog(true);
        box.exec();
    }
    ui->pushButton->click();
}

/**
* @brief 删除表格所选内容
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_deleteSeletedBtn_clicked()
{

    setEditButtonEnable(true);
    QModelIndexList indexList =  ui->tableView->selectionModel()->selectedRows();
    foreach (QModelIndex index, indexList)
    {
        model->removeRow(index.row());
        deleteInfoRow.append(index.row());
        ui->tableView->hideRow(index.row());
    }
    ui->textEdit->append("请保存修改或撤销~");
}


/**
* @brief 撤销修改
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_CancelBtn_clicked()
{
    model->revertAll();
    foreach(int index ,deleteRow)
    {
        ui->tableView->showRow(index);
    }
    deleteRow.clear(); // 清空列表
}


/**
* @brief 保存
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_saveBtn_clicked()
{
    MyMessageBox box;

    box.iniMsgBox(MyMessageBox::alert,"你确定要保存所有修改吗?");
    if(box.exec()==QDialog::Accepted)
    {
        ui->textEdit->append("请稍等...");
        model->database().transaction();
        if(model->submitAll())
        {
            deleteInfoRow.clear();
            Global::writeLog("work",QString("教师%1修改学生信息成功").arg(g.getTeacherID()));
            model->database().commit();
            ui->textEdit->append("保存成功.\n");
            SingleTonSToast::getInstance().setMessageVDuration("保存成功", 5000);
            setEditButtonEnable(false);
        }
        else
        {
            model->database().rollback();
            box.iniMsgBox(MyMessageBox::error,QString("保存失败！错误代码:%1")
                          .arg(model->lastError().text()));
            box.exec();
            ui->textEdit->append("保存失败！请查询后重新尝试.\n");
            Global::writeLog("work",QString("教师%1修改学生信息失败, 错误：%1").arg(g.getTeacherID(),model->lastError().text()));
            Global::writeLog("error",QString("教师%1修改学生信息失败, 错误：%1").arg(g.getTeacherID(),model->lastError().text()));

        }
    }
}

/**
* @brief 下载模板
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_downloadBtn_clicked()
{
    QString fileName;

#ifdef Q_OS_WIN
    fileName = QFileDialog::getSaveFileName(this,"下载模板","c:/学生信息模板","excel(*.xlsx);;文本(*.csv)");
#else
    fileName = QFileDialog::getSaveFileName(this,"下载模板","home/学生信息模板","excel(*.xlsx);;文本(*.csv)");
#endif
    if(fileName == "")
        return;

    QFileInfo fileinfo = QFileInfo(fileName);
    //文件后缀
    QString file_suffix = fileinfo.suffix();
    bool state = false;
    if(file_suffix == "csv")  //导出csv，txt格式
    {
        state = QFile::copy(":/detail/files/学生信息模板.csv",fileName);
    }
    else
    {
        state = QFile::copy(":/detail/files/学生信息模板.xlsx",fileName);
    }
    if(!state)
        ui->textEdit->append(QString("模板下载失败,请检查模板文件是否存在,路径为%1/files/学生信息模板.xlsx(csv)").arg(qApp->applicationDirPath()));
    else
        ui->textEdit->append(QString("学生信息模板下载成功 所在文件夹 %1").arg(fileName));
    Global::writeLog("work",QString("教师%1下载模板").arg(g.getTeacherID()));
}


/**
* @brief 添加新行
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_addRowBtn_clicked()
{
    int rowCount = model->rowCount( );
    model->insertRow(rowCount);
    ui->tableView->scrollToBottom();  //滚动到底部
}


/**
* @brief 查询考勤记录
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::on_tab3searchAttendanceBtn_clicked()
{
    searchSeletedAttendance();
}



/**
* @brief tabwidget切换
* @date 2017/12/10
* @author han
* @return void
*/
void CLassManage::on_tabWidget_currentChanged(int index)
{
    if (index == 0) //切换到考勤记录面板
    {
        initDrop();
    }
    else if(index == 1)  //查询考勤统计数据
    {
        noShowDetail = true;
        ui->pushButton->click();
        noShowDetail = false;
    }
    else if(index == 3)  //查询建议
    {
        getAdviceHistory();
    }
    else if(index == 4)
    {
        Global g;
        g.connectdb();

        g.sql.exec(QString("select Detail,lastUpdateTime from erepair where worklogid = '%1';").arg(g.getWorkLogID()));

        if(g.sql.next())
        {
            QString time = g.sql.value(1).toString();
            if(time.contains("T"))
                time = time.replace("T"," ");

            QString detail = g.sql.value(0).toString();
            ui->labRepairUpdateTime->setText("上次更新时间:"+time);
            ui->textRepair->setText(detail);
        }
    }
}



/**
* @brief 获取教师的历史建议
* @date 2018/1/6
* @author han
* @return void
*/
void CLassManage::getAdviceHistory()
{
    Global g;
    g.connectdb();

    g.sql.exec(QString(" select AdviceDetail,solved,t.lastUpdateTime,t.worklogid "
                       " from teacherAdvice t LEFT JOIN worklog w on t.worklogid = w.id "
                       " where w.teacherID = '%1'")
               .arg(g.getTeacherID()));

    ui->AdviceList->clear();
    int i = 0;
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        int isSolved = g.sql.value(1).toInt();
        QString time = g.sql.value(2).toString();
        int workLogID = g.sql.value(3).toInt();

        if(workLogID == g.workLogId)  //为当前的建议
        {
            if(time.contains("T"))
                time = time.replace("T"," ");
            ui->labLastAdUpdateTime->setText("上次更新时间:"+time);
            ui->leAdvice->setText(detail);
        }

        detail = convertToString(detail); //转换字符串

        QListWidgetItem *item = new QListWidgetItem(ui->AdviceList);
        if(isSolved == g.ADVICESOLVED)  //已受理
        {
            item->setData(Qt::DecorationRole, solved);
            item->setData(Qt::DisplayRole,detail);
        }
        else
        {
            item->setData(Qt::DecorationRole, unSolved);
            item->setData(Qt::DisplayRole,detail);
        }
        item->setToolTip(detail);
        //        if(i % 2 == 0 || i == 0)
        //            item->setBackgroundColor(QColor(117, 249, 255));
        i++;
    }
}


/**
* @brief 转换字符串,适当位置加上换行,让其在listwidget上显示正常
* @date 2018/1/6
* @author han
* @return void
*/
QString CLassManage::convertToString(QString sourceStr)
{
    if(sourceStr.length() < 15)  //长度太短，无需转换
        return sourceStr;

    QStringList list;
    list.clear();
    if(sourceStr.contains("\n"))  //把原文按换行符分割
    {
        list = sourceStr.split("\n");
    }
    else
        list.append(sourceStr);

    QString result = "";
    int i = 0;
    foreach(QString str,list) //每段处理
    {\
        if(sourceStr.length() > 15)
        {
            int rowCount = str.length() / 15;  //计算显示的行数

            if(str.length() % 15 != 0)  //最后一行
                rowCount++;
            for(int i = 1;i < rowCount;i++)
            {
                if(i*15 < str.length())
                    str = str.insert(i * 15, "\n");  //插入换行符
            }
        }
        i++;
        if(i != list.size())  //如果不是最后一行，加回原来的换行符
            str = str + "\n";
        result += str;
    }
    return result;
}
/**
* @brief 班别课程下拉列表改变
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::on_cbxLessonAndClass_currentIndexChanged(const QString &arg1)
{
    ui->cbxWeek->clear();  //清空原有选项
    if(arg1.trimmed().isEmpty())
        return;
    QStringList list = arg1.split(" ");
    QString lname = list.at(0);
    QString classname = list.at(1);
    Global g;
    g.connectdb();
    g.sql.clear();


    QString sql = QString("select distinct lessonweek from attendance "
                          " where classname ='%1' "
                          " and lname = '%2' ORDER BY lessonweek DESC ")
            .arg(classname,lname);
    //qDebug() << sql;
    g.sql.clear();
    g.sql.exec(sql);
    while(g.sql.next())
    {
        QString week ="第" + g.sql.value(0).toString() + "周";
        ui->cbxWeek->addItem(week);
    }

}



/**
* @brief 周次下拉列表改变
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::on_cbxWeek_currentIndexChanged(const QString &arg1)
{
    Global g;
    g.connectdb();
    ui->cbxZhou->clear();  //清空原有选项
    if(arg1.trimmed().isEmpty())
        return;
    if(ui->cbxLessonAndClass->currentText().trimmed().isEmpty())
    {
        return;
    }
    QStringList list = ui->cbxLessonAndClass->currentText().split(" ");
    QString lname = list.at(0);
    QString classname = list.at(1);
    QString temp = arg1;
    QString week = temp.replace(QString("第"),QString(""));
    week = week.replace(QString("周"),QString(""));


    QString sql = QString("select distinct lessondate from attendance where classname ='%1' and lname = '%2' and lessonweek = '%3' ORDER BY lessondate DESC")
            .arg(classname,lname,week);

    //qDebug() << sql;
    g.sql.clear();
    g.sql.exec(sql);

    while(g.sql.next())
    {
        QString lessonDate =g.sql.value(0).toString();

        QDateTime t = QDateTime::fromString(lessonDate, "yyyy-MM-dd");

        QString time = lessonDate + " " + t.toString("ddd");

        ui->cbxZhou->addItem(time);
    }
}


/**
* @brief 日期下拉改变
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::on_cbxZhou_currentIndexChanged(const QString &arg1)
{
    Global g;
    g.connectdb();
    ui->cbxTime->clear();  //清空原有选项
    if(arg1.trimmed().isEmpty())
        return;
    if(ui->cbxLessonAndClass->currentText().trimmed().isEmpty() || ui->cbxZhou->currentText().trimmed().isEmpty())
    {
        return;
    }
    //获取班别
    QStringList list = ui->cbxLessonAndClass->currentText().split(" ");
    QString lname = list.at(0);
    QString classname = list.at(1);


    //获取周次
    QString week = ui->cbxWeek->currentText().replace(QString("第"),QString("")).replace(QString("周"),QString(""));

    //获取星期
    QStringList list1 = ui->cbxZhou->currentText().split(" ");
    QString lessonDate = list1.at(0);

    QString sql = QString("select distinct attendanceTime from attendance where classname ='%1' and lname = '%2' and lessonweek = '%3' and lessondate = '%4' order by attendanceTime desc")
            .arg(classname,lname,week,lessonDate);


    g.sql.exec(sql);

    while(g.sql.next())
    {
        QString attendanTime ="第" + g.sql.value(0).toString() + "次点名";
        ui->cbxTime->addItem(attendanTime);
    }

}


/**
* @brief 次数下拉
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::on_cbxTime_currentIndexChanged(const QString)
{
}

/**
* @brief 查询所有符合条件的考勤记录
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::searchSeletedAttendance()
{
    deleteRow.clear();
    Global g;
    g.connectdb();
    QString table = "Attendance";

    QString lname;
    QString classname ;
    if(ui->cbxLessonAndClass->currentText().trimmed().isEmpty())
    {
        sqlShow(0);
        return;
    }

    QString filter = "";
    //获取班别，课程
    QStringList list = ui->cbxLessonAndClass->currentText().split(" ");
    lname = list.at(0);
    classname = list.at(1);

    g.sql.exec(QString("select cid from lesson where tid = '%1' and lname = '%2' and cname = '%3'")
               .arg(g.getTeacherID(),lname,classname));
    if(!g.sql.next())
        return;
    QString studentTable = "class" + g.sql.value(0).toString();

    filter = QString(" lname='%1' and (classname in (select distinct classname from %2) or className = '%3' or className = '')")
            .arg(lname,studentTable,classname);

    //获取周次
    QString temp = ui->cbxWeek->currentText();
    if(!temp.trimmed().isEmpty())
    {
        QString week = temp.replace(QString("第"),QString("")).replace(QString("周"),QString(""));
        filter += QString(" and lessonweek = '%1'").arg(week);
    }

    //获取星期
    temp = ui->cbxZhou->currentText();
    QString lessonDate = "";
    if(!temp.trimmed().isEmpty())
    {
        QStringList list1 = temp.split(" ");
        lessonDate = list1.at(0);
        filter += QString(" and lessonDate = '%1'").arg(lessonDate);
    }
    //获取点名次数
    temp = ui->cbxTime->currentText();
    QString time;
    if(!temp.trimmed().isEmpty())
    {
        time = ui->cbxTime->currentText().replace(QString("第"),QString("")).replace(QString("次点名"),QString(""));
        filter += QString(" and AttendanceTime = '%1'").arg(time);
    }

    //设置表头
    AttendanceModel->setTable(table);  //设置表名
    AttendanceModel->setFilter(filter);  //设置条件
    AttendanceModel->select();
    AttendanceModel->setHeaderData(1,Qt::Horizontal,"日期");
    AttendanceModel->setHeaderData(2,Qt::Horizontal,"周次");
    AttendanceModel->setHeaderData(3,Qt::Horizontal,"课程");
    AttendanceModel->setHeaderData(4,Qt::Horizontal,"班别");
    AttendanceModel->setHeaderData(5,Qt::Horizontal,"学号");
    AttendanceModel->setHeaderData(6,Qt::Horizontal,"姓名");
    AttendanceModel->setHeaderData(7,Qt::Horizontal,"第i次考勤");
    AttendanceModel->setHeaderData(8,Qt::Horizontal,"考勤情况");
    AttendanceModel->setHeaderData(10,Qt::Horizontal,"机位号");
    sqlShow(AttendanceModel->rowCount());
    ui->tab3addAttendanceBtn->setEnabled(true);
    ui->tab3deleteAttendanceBtn->setEnabled(true);
    ui->tab3downloadAttendanceBtn->setEnabled(true);

    ui->tbViewAttendance->hideColumn(0);
    ui->tbViewAttendance->hideColumn(7);
    ui->tbViewAttendance->hideColumn(9);

    Global::writeLog("work",QString("教师%1查询考勤记录").arg(g.getTeacherID()));

    //获取日志id
    QModelIndex idIndex=AttendanceModel->index(0,9,QModelIndex());
    int worklogid = idIndex.data().toInt();

    if(g.workLogId == worklogid && time.toInt() == g.AttendanceTime)
    {
        ui->labRecordInfo->setText("所显示的为当前上课考勤记录");
        ui->labRecordInfo->setStyleSheet("QLabel{color:green}");
        ui->sendToFtpBtn->setEnabled(true);
    }
    else
    {
        ui->labRecordInfo->setText("所显示的为历史考勤记录");
        ui->labRecordInfo->setStyleSheet("QLabel{color:blue}");
    }

}


/**
* @brief 增加考勤记录
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::on_tab3addAttendanceBtn_clicked()
{
    MyMessageBox box;

    QString name = ui->cbxLessonAndClass->currentText();

    box.iniMsgBox(MyMessageBox::information,"添加考勤记录的班别为：<br>" + name);

    if(box.exec())
    {
        QModelIndex idIndex=AttendanceModel->index(0,1,QModelIndex());//获取日期
        QString lessonDate = idIndex.data().toString();
        if(lessonDate.contains("-"))
            lessonDate = lessonDate.replace("-","/");

        idIndex=AttendanceModel->index(0,2,QModelIndex());//获取周次
        QString week = idIndex.data().toString();

        idIndex=AttendanceModel->index(0,3,QModelIndex());//获取课程
        QString lname = idIndex.data().toString();

        idIndex=AttendanceModel->index(0,4,QModelIndex());//获取班别
        QString classname = idIndex.data().toString();

        idIndex=AttendanceModel->index(0,7,QModelIndex());//获取点名次数
        QString time = idIndex.data().toString();

        idIndex=AttendanceModel->index(0,9,QModelIndex());//获取日志ID
        QString worklogID = idIndex.data().toString();

        int rowCount = AttendanceModel->rowCount();
        AttendanceModel->insertRow(rowCount);
        ui->tbViewAttendance->scrollToBottom();  //滚动到底部
        AttendanceModel->setData(AttendanceModel->index(rowCount,1),lessonDate);  //设置日期
        AttendanceModel->setData(AttendanceModel->index(rowCount,2),week);  //设置周次
        AttendanceModel->setData(AttendanceModel->index(rowCount,3),lname);  //设置课程
        AttendanceModel->setData(AttendanceModel->index(rowCount,4),classname);  //设置班别
        AttendanceModel->setData(AttendanceModel->index(rowCount,7),time);  //设置考勤次数
        AttendanceModel->setData(AttendanceModel->index(rowCount,8),"迟到");  //考勤情况
        AttendanceModel->setData(AttendanceModel->index(rowCount,9),worklogID);  //日志ID

        ui->textEdit->append(QString("修改后点击'保存'生效.\n"));
        ui->cancelEditAtBtn->setEnabled(true);
        ui->tab3saveAttendanceBtn->setEnabled(true);
    }
}



/**
* @brief 保存考勤记录修改
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::on_tab3saveAttendanceBtn_clicked()
{
    MyMessageBox box;

    box.iniMsgBox(MyMessageBox::alert,"你确定要保存所有修改吗?");
    if(box.exec()==QDialog::Accepted)
    {
        ui->textEdit->append("请稍等...");
        if(!g.db.isOpen())
        {
            Global::writeLog("error",QString("教师%1删除考勤记录失败，数据库未开启").arg(g.teacherNum));
            return;
        }
        AttendanceModel->database().transaction();
        if(AttendanceModel->submitAll())
        {
            deleteRow.clear(); // 清空列表
            while(AttendanceModel->canFetchMore())
                AttendanceModel->fetchMore();
            AttendanceModel->database().commit();
            ui->textEdit->append("保存成功.\n");
            SingleTonSToast::getInstance().setMessageVDuration("保存成功", 5000);
            ui->cancelEditAtBtn->setEnabled(false);
            ui->tab3saveAttendanceBtn->setEnabled(false);
            Global::writeLog("work",QString("教师%1修改考勤记录成功").arg(g.teacherNum));
        }
        else
        {
            AttendanceModel->database().rollback();
            Global::writeLog("work",QString("教师%1修改考勤记录失败,错误原因:%2").arg(g.getTeacherID(),AttendanceModel->lastError().text()));
            Global::writeLog("error",QString("教师%1修改考勤记录失败,错误原因:%2").arg(g.getTeacherID(),AttendanceModel->lastError().text()));
            box.iniMsgBox(MyMessageBox::error,QString("保存失败！错误代码:%1")
                          .arg(AttendanceModel->lastError().text()));
            box.exec();
            // qDebug() << AttendanceModel->lastError().text();
            ui->textEdit->append("保存失败！请重新尝试.\n");
        }
    }

}


/**
* @brief 删除选中的考勤记录
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::on_tab3deleteAttendanceBtn_clicked()
{
    QModelIndexList indexList =  ui->tbViewAttendance->selectionModel()->selectedRows();
    foreach (QModelIndex index, indexList)
    {
        AttendanceModel->removeRow(index.row());
        deleteRow.append(index.row());
        ui->tbViewAttendance->hideRow(index.row());
    }
    ui->textEdit->append("请保存修改或撤销.");
    ui->cancelEditAtBtn->setEnabled(true);
    ui->tab3saveAttendanceBtn->setEnabled(true);


}


/**
* @brief 考勤表有修改
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::attendanceModelDataChanged(QModelIndex,QModelIndex)
{
    ui->cancelEditAtBtn->setEnabled(true);
    ui->tab3saveAttendanceBtn->setEnabled(true);
}

/**
* @brief 撤销修改考勤记录
* @date 2017/12/11
* @author han
* @return void
*/
void CLassManage::on_cancelEditAtBtn_clicked()
{
    AttendanceModel->revertAll();

    ui->cancelEditAtBtn->setEnabled(false);
    ui->tab3saveAttendanceBtn->setEnabled(false);

    foreach(int index ,deleteRow)
    {
        ui->tbViewAttendance->showRow(index);
    }
    deleteRow.clear(); // 清空列表
}


/**
* @brief 导出考勤记录
* @date 2017/12/12
* @author han
* @return void
*/
void CLassManage::on_tab3downloadAttendanceBtn_clicked()
{

    if(ui->cbxLessonAndClass->currentText().trimmed().isEmpty())
    {
        return;
    }

    QString name = "";

    //获取班别，课程
    name = ui->cbxLessonAndClass->currentText();

    //获取周次
    QString temp = ui->cbxWeek->currentText();
    if(!temp.trimmed().isEmpty())
    {
        name += " "+temp;
    }

    //获取日期
    temp = ui->cbxZhou->currentText();
    if(!temp.trimmed().isEmpty())
    {
        name = QString("%1 ").arg(temp) + name;
    }
    //获取点名次数
    temp = ui->cbxTime->currentText();
    if(!temp.trimmed().isEmpty())
    {
        QString time = ui->cbxTime->currentText().replace(QString("第"),QString("")).replace(QString("次点名"),QString(""));
        name += QString(" 第%1次").arg(time);
    }
    name += "考勤记录";

    MyMessageBox box;

    QString fileName;
#ifdef Q_OS_WIN
    fileName = QFileDialog::getSaveFileName(this,"导出数据","c:/" + name,"考勤记录(*.xlsx);;考勤记录(*.csv);;文本文件(*.txt)");
#else  //linux
    fileName = QFileDialog::getSaveFileName(this,"导出数据","home/" + name,"考勤记录(*.xlsx);;考勤记录(*.csv);;文本文件(*.txt)");
#endif
    if(fileName == "")
        return;
    QFileInfo fileinfo = QFileInfo(fileName);
    //文件后缀
    QString file_suffix = fileinfo.suffix();
    if(file_suffix != "xlsx")  //导出csv，txt格式
    {
        QFile csv(fileName);
        if(!csv.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            box.iniMsgBox(MyMessageBox::error,"数据导出失败,打开文件失败.");
            box.exec();
            return;
        }
        QTextStream out(&csv);
        QString line = "日期,周次,课程,班别,学号,姓名,第i次考勤,考勤结果,机位号";
        out<<line<<"\n";  //写入表头

        for(int i = 0;i < AttendanceModel->rowCount();i++)
        {
            QString lessonDate = AttendanceModel->record(i).value(1).toString();
            QString week = AttendanceModel->record(i).value(2).toString();
            QString lname = AttendanceModel->record(i).value(3).toString();
            QString cname = AttendanceModel->record(i).value(4).toString();
            QString id = AttendanceModel->record(i).value(5).toString();
            QString sname = AttendanceModel->record(i).value(6).toString();
            QString aTime = AttendanceModel->record(i).value(7).toString();
            QString result = AttendanceModel->record(i).value(8).toString();
            QString position = AttendanceModel->record(i).value(10).toString();

            line = lessonDate + ',' +week + ',' + lname + ',' + cname + ',' +id+ ',' +sname + ',' +aTime+ ',' +result + ','+position;
            out<<line<<"\n";
        }
    }
    else
    {
        ExcelManager ex;
        ex.ExportToExcel(AttendanceModel,fileName);
    }
    SingleTonSToast::getInstance().setMessageVDuration("导出成功", 5000);

    ui->textEdit->append(QString("数据导出成功 文件路径 %1").arg(fileName));
    Global::writeLog("work",QString("教师%1导出考勤数据成功 路径:%2").arg(g.getTeacherID(),fileName));
}


/**
* @brief 打开工作日志界面
* @date 2017/12/12
* @author han
* @return void
*/
void CLassManage::on_showWorkLogBtn_clicked()
{
    emit editWorklog();
}


/**
* @brief 设置自动点名时间间隔
* @date 2017/12/13
* @author han
* @return void
*/
void CLassManage::on_pushButton_2_clicked()
{
    Global::writeLog("work",QString("教师%1修改自动点名时间间隔").arg(g.teacherNum));
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    int selectTime = ui->cbxAttenddanceTime->currentText().replace("分钟","").toInt();
    QString value = QString("AttendanceTime/%1").arg(g.teacherNum);
    set->setValue(value,QString::number(selectTime,10));
    globalAttendanceTime = selectTime * 60;  //更改考勤间隔
    ui->labattendantime->setText(QString("倒计时：%1:00").arg(QString::number(selectTime,10)));
    timeOutTime = 0;
    signTimer->stop();
    signTimer->start(1000);
    delete set;
    SingleTonSToast::getInstance().setMessageVDuration("设置成功", 5000);
}


/**
* @brief 最小化
* @date 2017/12/14
* @author han
* @return void
*/
void CLassManage::on_minBtn_clicked()
{
    this->showMinimized();
}


/**
* @brief 最大化
* @date 2017/12/14
* @author han
* @return void
*/
void CLassManage::on_maxBtn_clicked()
{
    if(this->isMaximized()) //如果已经是最大化了，就显示正常的
    {
        this->showNormal();
    }
    else
    {
        this->showMaximized();
    }
}


/**
* @brief 关闭
* @date 2017/12/14
* @author han
* @return void
*/
void CLassManage::on_closeBtn_clicked()
{
    this->close();
}

void CLassManage::mousePressEvent(QMouseEvent *event)
{
#ifdef Q_OS_WIN
    if (ReleaseCapture())
    {
        QWidget *pWindow = this->window();
        if (pWindow->isTopLevel())
        {
            SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
        }
    }
    event->ignore();
#else
    if(event->button() == Qt::LeftButton)
    {
        QWidget * p = this->window();
        if(p->isTopLevel())
        {
            m_DragPosition = event->globalPos() - p->pos();
            ispress = true;
            event->accept();
        }
    }
#endif
}

void CLassManage::mouseMoveEvent(QMouseEvent *event)
{
#ifdef Q_OS_WIN
    Q_UNUSED(event);
#else
    if (ispress && event->buttons() && Qt::LeftButton)
    {
        QWidget * p = this->window();
        if(p->isTopLevel())
        {
            if(p->isMaximized())
                p->showNormal();
            p->move(event->globalPos() - m_DragPosition);
            event->accept();
        }
    }
#endif
}

void CLassManage::mouseReleaseEvent(QMouseEvent *)
{
    ispress =false;
    update();
}

void CLassManage::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
        emit ui->maxBtn->clicked();
}

/**
* @brief 测试Ftp连接
* @date 2017/12/15
* @author han
* @return void
*/
void CLassManage::on_pushButton_3_clicked()
{
    if(FtpNumber == ""||FtpPwd == "" || FtpServerIP == "")
    {
        SingleTonSToast::getInstance().setMessageVDuration("管理员未设置服务器IP及账号", 6000,1);
        return;
    }

    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path = appDataPath + "/ftpTest.txt";
    QFile f(path);
    if(!f.exists())  //测试文件不存在,新建一个
    {
        if(!f.open(QIODevice::WriteOnly | QIODevice::Append|QIODevice::Text))
        {
            qDebug() << "Open failed." ;
            MyMessageBox box;
            box.iniMsgBox(MyMessageBox::information,"测试文件创建失败,请手动在软件根目录创建文件名为ftpTest.txt文件,再进行测试.");
            box.isShowDialog(true);
            box.exec();
            return;
        }
        QTextStream txtOutput(&f);
        txtOutput << "Ftp Test" << endl;
        f.close();
    }

    FtpManager *ftp = new FtpManager(FtpNumber,FtpPwd,FtpServerIP);

    QString fileName = "FtpTest" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".txt";

    if(SaveTogether)  //存在一起
    {
        QString serverPath = saveTogetherPath.trimmed();
        //处理格式，以防出错
        serverPath = serverPath.replace(":","");
        QStringList list;
        if(serverPath.contains("/"))
            list = serverPath.split("/");
        else
            list.append(serverPath);
        serverPath = "";
        foreach(QString s ,list)
        {
            if(!s.trimmed().isEmpty())
                serverPath +=  s + "/";
        }
        serverPath += fileName;
        ftp->uploadFile(path,serverPath);  //上传测试文件
        //qDebug() << FtpNumber << FtpPwd <<  FtpServerIP << serverPath;
    }
    else  //各自保存
    {
        g.connectdb();
        QString sql = QString("select name from teacher where id = '%1'").arg(g.getTeacherID());
        g.sql.exec(sql);
        QString tname;
        if(g.sql.next())
            tname = g.sql.value(0).toString();
        QString serverpath = tname + "/"+ fileName;
        ftp->uploadFile(path,serverpath);  //上传测试文件

        // qDebug() << FtpNumber << FtpPwd <<  FtpServerIP << serverpath << sql;
    }

    reply = ftp->getRelay();  //获取
    connect(reply, SIGNAL(finished()), this, SLOT(OnFinishRelay()));
    isFtpTest = true;

    connectFtpTimer->start(1000);
    ui->labFtpTestResult->setText("正在连接:10");
    timeOutTimes = 10;
}

/**
* @brief 网络超时
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void CLassManage::onTimeOut()
{
    timeOutTimes--;
    ui->labFtpTestResult->setText("正在连接:"+ QString::number(timeOutTimes));
    if(timeOutTimes == 0)
    {
        connectFtpTimer->stop();
        if (reply->isRunning())
        {
            reply->abort();
            reply->deleteLater();
        }
    }
}


/**
* @brief 发送结果回调函数
* @date 2017/12/15
* @author han
* @return void
*/
void CLassManage::OnFinishRelay()
{    
    connectFtpTimer->stop();
    ui->labFtpTestResult->clear();
    MyMessageBox box;
    //无错误返回
    if(reply->error() == QNetworkReply::NoError)
    {
        if(isFtpTest)
        {
            SingleTonSToast::getInstance().setMessageVDuration("连接成功", 6000);
            isFtpTest = false;
        }
        else
        {
            if(!unShowInfo)
            {
                unShowInfo = false;
                ui->textEdit->append("文件发送到Ftp成功.");
                SingleTonSToast::getInstance().setMessageVDuration("文件发送成功", 6000);
            }
        }
    }
    else
    {
        //处理错误
        if(isFtpTest)
        {
            isFtpTest = false;
            box.iniMsgBox(MyMessageBox::error,QString("连接失败，错误原因：%1<br>可能原因：服务器不存在以教师姓名命名的文件夹.").arg(reply->errorString()));
            box.isShowDialog(true);
            box.exec();
        }
        else
        {
            if(!unShowInfo)
            {
                unShowInfo = false;
                box.iniMsgBox(MyMessageBox::error,QString("文件发送失败，错误原因：%1<br>").arg(reply->errorString()));
                box.isShowDialog(true);
                box.exec();
            }
        }
    }

    reply->deleteLater();
}

/**
* @brief 选择ftp文件
* @date 2017/12/15
* @author han
* @return void
*/
void CLassManage::on_selectFtpFileBtn_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"选择文件",qApp->applicationDirPath()
                                                    ,"files(*.*)");
    if(filename =="")
        return;
    ui->leFileName->setText(filename);
    ui->uploadProgressBar->setVisible(false);
}


/**
* @brief 上传选择的文件
* @date 2017/12/15
* @author han
* @return void
*/
void CLassManage::on_uploadFtpFileBtn_clicked()
{
    MyMessageBox box;
    QString path =  ui->leFileName->text();
    if(path.trimmed().isEmpty())
    {
        box.iniMsgBox(MyMessageBox::information,"请先选择文件.");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    QFileInfo f(path);
    if(!f.exists())  //测试文件不存在,新建一个
    {
        box.iniMsgBox(MyMessageBox::information,"文件不存在.");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    QString fileName = f.fileName(); //获取文件名

    FtpManager *ftp = new FtpManager(FtpNumber,FtpPwd,FtpServerIP);
    if(SaveTogether)  //存在一起
    {
        QString serverPath = saveTogetherPath.trimmed();
        //处理格式，以防出错
        serverPath = serverPath.replace(":","");
        QStringList list;
        if(serverPath.contains("/"))
            list = serverPath.split("/");
        else
            list.append(serverPath);
        serverPath = "";
        foreach(QString s ,list)
        {
            if(!s.trimmed().isEmpty())
                serverPath +=  s + "/";
        }
        serverPath += fileName;
        ftp->uploadFile(path,serverPath);  //上传测试文件
        //qDebug() << FtpNumber << FtpPwd <<  FtpServerIP << serverPath;
    }
    else  //各自保存
    {
        g.connectdb();
        QString sql = QString("select name from teacher where id = '%1'").arg(g.getTeacherID());
        g.sql.exec(sql);
        QString tname;
        if(g.sql.next())
            tname = g.sql.value(0).toString();
        QString serverpath = tname + "/"+ fileName;
        ftp->uploadFile(path,serverpath);  //上传测试文件

        // qDebug() << FtpNumber << FtpPwd <<  FtpServerIP << serverpath << sql;
    }

    reply = ftp->getRelay();  //获取
    connect(reply, SIGNAL(finished()), this, SLOT(OnFinishRelay()));
    connect(reply, SIGNAL(uploadProgress(qint64 ,qint64)),this,SLOT(OnUploadProgress(qint64,qint64)));
    ui->uploadProgressBar->setVisible(true);
}


/**
* @brief 显示上传进度条
* @date 2017/12/15
* @author han
* @return void
*/
void CLassManage::OnUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    ui->uploadProgressBar->setValue(bytesSent*1.0/bytesTotal*100);
}


/**
* @brief 获取当前考勤次数
* @date 2017/12/16
* @author han
* @return void
*/
int CLassManage::getCurrentAttendanceTime()
{
    return g.AttendanceTime;
}


/**
* @brief 手动发送考勤记录到Ftp服务器
* @date 2017/12/16
* @author han
* @return void
*/
void CLassManage::on_sendToFtpBtn_clicked()
{
    saveAttendanceAuto(false);
}


/**
* @brief 返回
* @date 2017/12/16
* @author han
* @return void
*/
void CLassManage::on_pushButton_5_clicked()
{
    this->hide();
}

void CLassManage::on_resetReBtn_clicked()
{
    ui->textRepair->clear();
}

void CLassManage::on_resetAdBtn_clicked()
{
    ui->leAdvice->clear();
}

/**
* @brief 填写建议
* @date 2018/1/5
* @author han
* @return void
*/
void CLassManage::on_confirmAdBtn_clicked()
{
    Global g;
    g.connectdb();
    MyMessageBox box;
    QString time  = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString ad = ui->leAdvice->toPlainText();
    ui->labLastAdUpdateTime->setText("上次更新时间:"+time);

    //不存在插入，否则更新
    QString sql = QString("INSERT INTO teacheradvice(advicedetail,worklogid,createTime,lastUpdateTime) value('%1','%2','%3','%3') "
                          " ON DUPLICATE KEY UPDATE advicedetail='%1',lastUpdateTime='%3';").arg(ad,g.getWorkLogID(),time);
    if(g.sql.exec(sql))
    {
        SingleTonSToast::getInstance().setMessageVDuration("保存成功", 5000);
    }
    else
    {
        box.iniMsgBox(MyMessageBox::error,"保存失败,原因：" + g.sql.lastError().text());
        box.isShowDialog(true);
        box.exec();
    }
    getAdviceHistory();//更新
}


/**
* @brief 插入维修信息
* @date 2018/1/6
* @author han
* @return void
*/
void CLassManage::on_confirmReBtn_clicked()
{
    Global g;
    g.connectdb();
    MyMessageBox box;
    QString time  = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString repair = ui->textRepair->toPlainText();
    ui->labRepairUpdateTime->setText("上次更新时间:"+time);

    //不存在插入，否则更新
    QString sql = QString("INSERT INTO erepair(detail,worklogid,createTime,lastUpdateTime) value('%1','%2','%3','%3') "
                          " ON DUPLICATE KEY UPDATE detail='%1',lastUpdateTime='%3';")
            .arg(repair,g.getWorkLogID(),time);
    if(g.sql.exec(sql))
    {
        SingleTonSToast::getInstance().setMessageVDuration("保存成功", 5000);
    }
    else
    {
        box.iniMsgBox(MyMessageBox::info_no_beep,"保存失败,原因：" + g.sql.lastError().text());
        box.isShowDialog(true);
        box.exec();
    }

}


/**
* @brief 右键菜单点击
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void CLassManage::rightClickedOperation()
{
    QAction * action = qobject_cast<QAction*>(sender());

    if(copyAction == action)  //复制
    {
        QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
        QString originalText;
        QModelIndex index;
        QString otherData = "";

        if(ui->tabWidget->currentIndex() == 0)
        {
            index =  ui->tbViewAttendance->currentIndex();
        }
        else if(ui->tabWidget->currentIndex() == 1)
        {
            index =  ui->tableView->currentIndex();
        }
        else if(ui->tabWidget->currentIndex() == 2)
        {
            index =  ui->tableWidget->currentIndex();
        }
        else if(ui->tabWidget->currentIndex() == 3)
        {
            index =  ui->AdviceList->currentIndex();
        }

        if(otherData == "")
            originalText = index.data(Qt::DisplayRole).toString();
        else
            originalText = otherData;

        clipboard->setText(originalText);                  //设置剪贴板内容
    }
    else if(pasteAction == action)  //粘贴
    {
        QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
        QString originalText = clipboard->text();      //获取剪切板内容

        if(ui->tabWidget->currentIndex() == 0)
        {
            QModelIndex index =  ui->tbViewAttendance->currentIndex();
            AttendanceModel->setData(index,originalText);
        }
        else if(ui->tabWidget->currentIndex() == 1)
        {
            QModelIndex index =  ui->tableView->currentIndex();
            model->setData(index,originalText);
        }

    }
    else if(refreshAction == action)  //刷新
    {
        if(ui->tabWidget->currentIndex() == 0)
        {
            ui->tab3searchAttendanceBtn->click();
        }
        else if(ui->tabWidget->currentIndex() == 1)
        {
            ui->pushButton->click();
        }
        else if(ui->tabWidget->currentIndex() == 3)
        {
            getAdviceHistory();
        }

    }
    else if(deleteAction == action)  //删除
    {
        if(ui->tabWidget->currentIndex() == 0)
        {
            ui->tab3deleteAttendanceBtn->click();
        }
        else if(ui->tabWidget->currentIndex() == 1)
        {
            ui->deleteSeletedBtn->click();
        }
    }
    else if(exportAction == action) //导出
    {
        if(ui->tabWidget->currentIndex() == 0)
        {
            ui->tab3downloadAttendanceBtn->click();
        }
        else if(ui->tabWidget->currentIndex() == 1)
        {
            ui->exportStudentInfoBtn->click();
        }

    }
    else if(addAction == action)  //添加
    {
        if(ui->tabWidget->currentIndex() == 0)
        {
            ui->tab3addAttendanceBtn->click();
        }
        else if(ui->tabWidget->currentIndex() == 1)
        {
            ui->addRowBtn->click();
        }
    }

}


/**
* @brief 考勤管理表右键菜单
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void CLassManage::on_tbViewAttendance_customContextMenuRequested(const QPoint)
{
    popMenu->clear();
    popMenu->addAction(refreshAction);
    popMenu->addAction(copyAction);
    popMenu->addAction(pasteAction);
    popMenu->addAction(deleteAction);
    popMenu->addAction(addAction);
    popMenu->addSeparator();
    popMenu->addAction(exportAction);

    QModelIndexList list = ui->tbViewAttendance->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        ui->tab3deleteAttendanceBtn->setEnabled(false);
        deleteAction->setEnabled(false);
        copyAction->setEnabled(false);
        pasteAction->setEnabled(false);
    }
    else
    {
        ui->tab3deleteAttendanceBtn->setEnabled(true);
        deleteAction->setEnabled(true);
        copyAction->setEnabled(true);
        pasteAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());
}



/**
* @brief 考勤统计表右键菜单
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void CLassManage::on_tableView_customContextMenuRequested(const QPoint )
{
    popMenu->clear();
    popMenu->addAction(refreshAction);
    popMenu->addAction(copyAction);
    popMenu->addAction(pasteAction);
    popMenu->addAction(deleteAction);
    popMenu->addAction(addAction);
    popMenu->addSeparator();
    popMenu->addAction(exportAction);

    QModelIndexList list = ui->tableView->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        ui->deleteSeletedBtn->setEnabled(false);
        deleteAction->setEnabled(false);
        copyAction->setEnabled(false);
        pasteAction->setEnabled(false);
    }
    else
    {
        ui->deleteSeletedBtn->setEnabled(true);
        deleteAction->setEnabled(true);
        copyAction->setEnabled(true);
        pasteAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());
}


/**
* @brief 建议右侧列表右键菜单
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void CLassManage::on_AdviceList_customContextMenuRequested(const QPoint)
{
    popMenu->clear();
    popMenu->addAction(refreshAction);
    popMenu->addAction(copyAction);

    QModelIndexList list = ui->AdviceList->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        copyAction->setEnabled(false);
    }
    else
    {
        copyAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());
}


/**
* @brief 点名表右键菜单
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void CLassManage::on_tableWidget_customContextMenuRequested(const QPoint)
{
    popMenu->clear();
    popMenu->addAction(copyAction);
    popMenu->exec(QCursor::pos());
}

void CLassManage::on_radioSendAuto_clicked(bool checked)
{
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    QString sendValue = QString("FtpSet-%1/FtpSendType").arg(g.getTeacherID());

    if(checked)
    {
        SingleTonSToast::getInstance().setMessageVDuration("设置成功", 5000);
        sendAuto = true;
        set->setValue(sendValue,"1");
    }
}

void CLassManage::on_radioSendManual_toggled(bool checked)
{
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    QString sendValue = QString("FtpSet-%1/FtpSendType").arg(g.getTeacherID());

    if(checked)
    {
        SingleTonSToast::getInstance().setMessageVDuration("已关闭自动发送，请注意保存考勤记录", 6000);
        sendAuto = false;
        set->setValue(sendValue,"0");
    }
}

