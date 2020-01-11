#include "AdminOption.h"
#include "ui_AdminOption.h"
#include "CoolTitleBar.h"
#include "MyMessageBox.h"
#include "Global.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include <QSettings>
#include <QFileDialog>
#include <QSqlRecord>
#include <QRegExp>
#include <QValidator>

AdminOption::AdminOption(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdminOption)
{
    ui->setupUi(this);
    setWindowFlags( Qt::FramelessWindowHint| windowFlags());
    setWindowTitle("管理员菜单");

    ui->minBtn->setObjectName("minimizeButton");
    ui->closeBtn->setObjectName("closeButton");
    ui->AdminTitleW->setObjectName("TranTitle");
    ui->cbxClientValue->setObjectName("combobox_class2");
    ui->startDate->setCalendarPopup(true);
    ui->startDate->setDisplayFormat("yyyy-MM-dd");
    ui->startDate->setDate(QDate::currentDate());
    ispress = false;

    ui->endDate->setCalendarPopup(true);
    ui->endDate->setDisplayFormat("yyyy-MM-dd");
    ui->endDate->setDate(QDate::currentDate());

    ui->endDateClient->setCalendarPopup(true);
    ui->endDateClient->setDisplayFormat("yyyy-MM-dd");
    ui->endDateClient->setDate(QDate::currentDate());

    ui->startDateClient->setCalendarPopup(true);
    ui->startDateClient->setDisplayFormat("yyyy-MM-dd");
    ui->startDateClient->setDate(QDate::currentDate());


    ui->lineEdit->setToolTip("为空时查询全部专业使用实验室的情况");
    ui->labResult->setObjectName("labResult2");

    //实验室表
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //设置表格等宽

    ui->tableWidget->setColumnCount(14);
    header<<"日期"<<"周数"<<"星期"<<"节数"<<"指导老师"<<"课程名称"<<"专业"<<"班级"<<"应到人数"
         <<"实到人数"<<"学时"<<"实验类别"<<"课程信息"<<"实验类型";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setColumnWidth(5,200);
    ui->tableWidget->setColumnWidth(6,200);
    ui->tableWidget->setColumnWidth(1,100);
    ui->tableWidget->setColumnWidth(4,100);

    //客户机表
    ui->clientTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->clientTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->clientTable->setAlternatingRowColors(true);
    ui->clientTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //设置表格等宽

    //课程表
    ui->tableWidgetLesson->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetLesson->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetLesson->setAlternatingRowColors(true);
    ui->tableWidgetLesson->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //设置表格等宽


    g.connectdb();
    model = new MySqlTableModel();
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    connect(model,SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this,SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
    //教师表
    ui->tableView->setModel(model);
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  //设置表格等宽

    //获取管理员账号,密码由于加密原因从登陆界面穿过来
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    ui->leAdmin->setText(set->value("admin/id","admin").toString());

    //获取本地存储路径
    QString path = set->value("LocalSavePath/Path","AttendanceRecord").toString();
    if(path == "AttendanceRecord")
        ui->leShowLocalPath->setText(path);
    else
        ui->labLocalPath->setText(path);
    delete set;

    connect(ui->lineEdit,SIGNAL(returnPressed()),ui->searchMajorBtn,SIGNAL(clicked()));
    connect(ui->tableWidget->horizontalHeader(),SIGNAL(sectionClicked(int)),this,SLOT(onTableWidgetSort(int)));

    signDialog = new SignIn(this,"zhuce");
    connect(signDialog,SIGNAL(newItem(QString,QString)),this,SLOT(on_AddTeacher(QString,QString)));

    newLessonDialog = new SignIn(this,"newclass");

    ui->tabWidget->setCurrentIndex(0);
    deleteRow.clear();

    //限制只能输入数字
    QRegExp regx("[0-9]+$");
    ui->leClientPage->setValidator(new QRegExpValidator(regx, ui->leClientPage));
    ui->leTeacherPage->setValidator(new QRegExpValidator(regx, ui->leTeacherPage));
    ui->lelLabPage->setValidator(new QRegExpValidator(regx, ui->leTeacherPage));

    initLesson();  //初始化课程信息

    ui->resultProgressBar->hide();

    connectFtpTimer = new QTimer;
    connect(connectFtpTimer,SIGNAL(timeout()),this,SLOT(onTimeOut()));


    QImage temp(":/images/images/else/unsolved.png");
    QImage temp1(":/images/images/else/solved.png");
    unSolved = QPixmap::fromImage(temp).scaled(QSize(16,16), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    solved = QPixmap::fromImage(temp1).scaled(QSize(16,16), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    ui->labSolve->setPixmap(solved);
    ui->labUnsolve->setPixmap(unSolved);
    ui->labRepairSolved->setPixmap(solved);
    ui->labRepairUnsolve->setPixmap(unSolved);

    //设置菜单类型为自定义菜单
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->clientTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->AdminTitleW->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->lessonListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidgetLesson->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidgetAdvice->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidgetRepair->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidgetOrder->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidgetCategory->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidgetRequire->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listWidgetType->setContextMenuPolicy(Qt::CustomContextMenu);

    popMenu = new QMenu;
    popMenu->setMinimumWidth(200);
    copyAction = new QAction("复制",this);
    deleteAction= new QAction("删除",this);
    refreshAction= new QAction("刷新",this);
    exportAction= new QAction("导出",this);
    importAction= new QAction("导入",this);
    logoutAction= new QAction("切换账号",this);
    quitAction= new QAction("退出",this);
    pasteAction = new QAction("粘贴",this);
    addClassAction = new QAction("新建课程",this);
    addSetAction = new QAction("添加",this);

    // 添加图标
    QIcon icon(":/images/images/action/copy.png");
    copyAction->setIcon(icon);
    QIcon icon1(":/images/images/action/delete.png");
    deleteAction->setIcon(icon1);
    QIcon icon2(":/images/images/action/refresh.png");
    refreshAction->setIcon(icon2);
    QIcon icon3(":/images/images/action/export.png");
    exportAction->setIcon(icon3);
    QIcon icon4(":/images/images/action/import.png");
    importAction->setIcon(icon4);
    QIcon icon5(":/images/images/button/logout.ico");
    logoutAction->setIcon(icon5);
    QIcon icon6(":/images/images/button/close.ico");
    quitAction->setIcon(icon6);

    // 设置快捷键
    copyAction->setShortcut(QKeySequence::Copy);
    pasteAction->setShortcut(QKeySequence::Paste);

    titlemenu = new QMenu;
    ui->setButton->setMenu(titlemenu);
    ui->setButton->setObjectName("settingTitle");
    titlemenu->setMinimumWidth(150);
    titlemenu->addAction(logoutAction);
    titlemenu->addAction(quitAction);

    connect(copyAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(refreshAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(exportAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(importAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(logoutAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(pasteAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(addClassAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));
    connect(addSetAction, SIGNAL(triggered()), this, SLOT(rightClickedOperation()));

}

/**
* @brief 初始化课程信息
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::initLesson()
{
    ui->lessonListWidget->clear();
    lessonID.clear();
    g.connectdb();

    //获取实验类型
    g.sql.clear();
    g.sql.exec("select etype from etype  order by id");
    ui->cbxEtype->clear();
    while(g.sql.next())
    {
        ui->cbxEtype->addItem(g.sql.value(0).toString());
    }
    //获取实验类别
    g.sql.clear();
    g.sql.exec("select category from Ecategory order by id");
    ui->cbxCetagory->clear();
    while(g.sql.next())
    {
        ui->cbxCetagory->addItem(g.sql.value(0).toString());
    }
    //获取课程类别
    g.sql.clear();
    g.sql.exec("select erequire from erequire order by id");
    ui->cbxRequire->clear();
    while(g.sql.next())
    {
        ui->cbxRequire->addItem(g.sql.value(0).toString());
    }

    g.sql.clear();
    g.sql.exec("select id,cname,lname from lesson");
    while(g.sql.next())
    {
        QString detail = g.sql.value(1).toString()+ "-"+g.sql.value(2).toString();
        lessonID.append(g.sql.value(0).toString());  //对应保存课程id
        QListWidgetItem *item = new QListWidgetItem(ui->lessonListWidget);
        item->setData(Qt::DisplayRole,detail);
    }

    currentLessonIndex = -1;
    ui->lessonListWidget->setCurrentRow(0);
    initLessonInfo(0);
}

/**
* @brief 获取教师的历史建议
* @date 2018/1/6
* @author han
* @return void
*/
void AdminOption::getAdviceHistory()
{
    Global g;
    g.connectdb();

    QString sql ;
    if(ui->cbxAdviceType->currentText().trimmed() == "已查看")
        sql = QString(" select AdviceDetail,solved,createTime,id "
                      " from teacherAdvice where solved = 1 ");
    else if(ui->cbxAdviceType->currentText().trimmed() == "待查看")
        sql = QString(" select AdviceDetail,solved,createTime,id "
                      " from teacherAdvice where solved = 0 ");
    else
        sql = QString(" select AdviceDetail,solved,createTime,id "
                      " from teacherAdvice ");
    g.sql.exec(sql);
    if(g.sql.size() == 0)
        ui->pushButton_44->setEnabled(false);
    else
        ui->pushButton_44->setEnabled(true);

    ui->listWidgetAdvice->clear();
    int i = 0;
    int solveCount = 0;
    int unsolveCount = 0;
    AdInfoList.clear();
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        int isSolved = g.sql.value(1).toInt();
        QString createTime = g.sql.value(2).toString();
        int id = g.sql.value(3).toInt();
        AdInfo *adi = new AdInfo();
        adi->id = id;
        adi->index = i;
        adi->isSolved = isSolved;
        AdInfoList.append(adi);

        if(createTime.contains("T"))
            createTime.replace("T"," ");
        detail = createTime + "\n" + detail;

        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetAdvice);
        if(isSolved == g.ADVICESOLVED)  //已受理
        {
            solveCount++;
            item->setData(Qt::DecorationRole, solved);
            item->setData(Qt::DisplayRole,detail);
        }
        else
        {
            unsolveCount++;
            item->setData(Qt::DecorationRole, unSolved);
            item->setData(Qt::DisplayRole,detail);
        }

        item->setToolTip(detail);
        i++;
    }
    QString ad = QString("共%1条记录，已处理%2条，待处理%3条")
            .arg(QString::number(i),QString::number(solveCount),QString::number(unsolveCount));
    ui->labAdviceCount->setText(ad);
}

void AdminOption::paintEvent(QPaintEvent *ev)
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


void AdminOption::onTableWidgetSort(int i)
{
    ui->tableWidget->sortByColumn(i,Qt::AscendingOrder);
}



void AdminOption::closeEvent(QCloseEvent *ev)
{
    ev->ignore();
    exit(0);
}

AdminOption::~AdminOption()
{
    delete ui;
}


/**
* @brief 查询实验室使用情况
* @date 2018/1/9
* @author han
* @return void
*/
void AdminOption::on_labUsedBtn_clicked()
{

    ui->tableWidget->clearContents();
    //获取日期
    QString startTime = ui->startDate->text();
    QString endTime = ui->endDate->text();
    if(startTime.trimmed().isEmpty())
        startTime = "2010-1-1";
    if(endTime.trimmed().isEmpty())
    {
        endTime = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    }

    QString sql = QString("select date,week,day,lesson,tname,lname,cmajor,cname,cnum,cpresentnum,etime,etype,erequire,ecategory"
                          "  from worklog where date >= '%1' and date <= '%2'")
            .arg(startTime,endTime);
    Global g ;
    g.connectdb();
    g.sql.exec(sql);
    int count = g.sql.size();
    ui->tableWidget->setRowCount(count);
    int i = 0;
    //qDebug() << sql;
    while(g.sql.next())
    {
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(g.sql.value(0).toString()));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(g.sql.value(1).toString()));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(g.sql.value(2).toString()));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(g.sql.value(3).toString()));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(g.sql.value(4).toString()));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(g.sql.value(5).toString()));
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(g.sql.value(6).toString()));
        ui->tableWidget->setItem(i,7,new QTableWidgetItem(g.sql.value(7).toString()));
        ui->tableWidget->setItem(i,8,new QTableWidgetItem(g.sql.value(8).toString()));
        ui->tableWidget->setItem(i,9,new QTableWidgetItem(g.sql.value(9).toString()));
        ui->tableWidget->setItem(i,10,new QTableWidgetItem(g.sql.value(10).toString()));
        ui->tableWidget->setItem(i,11,new QTableWidgetItem(g.sql.value(11).toString()));
        ui->tableWidget->setItem(i,12,new QTableWidgetItem(g.sql.value(12).toString()));
        ui->tableWidget->setItem(i,13,new QTableWidgetItem(g.sql.value(13).toString()));

        //居中
        ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,8)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,9)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,10)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,11)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,12)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,13)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        i++;
    }

    QString temp = "/"+QString::number(g.pageCount(ui->tableWidget),10);
    ui->labLabPageCount->setText(temp);
    ui->labLRecord->setText(QString("共%1条记录").arg(QString::number(count)));

    ui->labResult->setText(QString("实验室使用次数：%1").arg(i));

    LaboratoryShowType = 1;

}

/**
* @brief 查询各专业使用实验室的情况
* @date 2018/1/9
* @author han
* @return void
*/
void AdminOption::on_searchMajorBtn_clicked()
{
    ui->tableWidget->clearContents();

    g.connectdb();
    if(ui->lineEdit->text().trimmed().isEmpty())
        g.sql.exec(QString("select date,week,day,lesson,tname,lname,cmajor,cname,cnum,cpresentnum,etime,etype,erequire,ecategory from worklog"));
    else
        g.sql.exec(QString("select date,week,day,lesson,tname,lname,cmajor,cname,cnum,cpresentnum,etime,etype,erequire,ecategory "
                           " from worklog where cmajor = '%1'")
                   .arg(ui->lineEdit->text()));
    int count = g.sql.size();
    ui->tableWidget->setRowCount(count);
    int i = 0;
    while(g.sql.next())
    {
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(g.sql.value(0).toString()));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(g.sql.value(1).toString()));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(g.sql.value(2).toString()));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(g.sql.value(3).toString()));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(g.sql.value(4).toString()));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(g.sql.value(5).toString()));
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(g.sql.value(6).toString()));
        ui->tableWidget->setItem(i,7,new QTableWidgetItem(g.sql.value(7).toString()));
        ui->tableWidget->setItem(i,8,new QTableWidgetItem(g.sql.value(8).toString()));
        ui->tableWidget->setItem(i,9,new QTableWidgetItem(g.sql.value(9).toString()));
        ui->tableWidget->setItem(i,10,new QTableWidgetItem(g.sql.value(10).toString()));
        ui->tableWidget->setItem(i,11,new QTableWidgetItem(g.sql.value(11).toString()));
        ui->tableWidget->setItem(i,12,new QTableWidgetItem(g.sql.value(12).toString()));
        ui->tableWidget->setItem(i,13,new QTableWidgetItem(g.sql.value(13).toString()));

        //居中
        ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,8)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,9)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,10)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,11)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,12)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,13)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        i++;
    }
    QString temp = "/"+QString::number(g.pageCount(ui->tableWidget),10);
    ui->labLabPageCount->setText(temp);
    ui->labLRecord->setText(QString("共%1条记录").arg(QString::number(count,10)));

    if(ui->lineEdit->text().trimmed().isEmpty())
        ui->labResult->setText(QString("专业使用实验室总次数:%1").arg(QString::number(count)));
    else
        ui->labResult->setText(QString("专业 %1 使用实验室总次数:%2").arg(ui->lineEdit->text(),QString::number(count)));

    LaboratoryShowType = 3;
}


/**
* @brief 清空所有日志记录
* @date 2018/1/8
* @author han
* @return void
*/
void AdminOption::on_clearAllBtn_clicked()
{
    if(!checkIsAdmin())
        return;
    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"确定要清空所有日志记录?");
    if(box.exec() == QDialog::Accepted)
    {
        box.iniMsgBox(MyMessageBox::alert,"此操作不可恢复,确定吗?");
        if(box.exec() == QDialog::Accepted)
        {
            g.connectdb();
            bool state = g.sql.exec("truncate table worklog");
            if(state)
            {
                ui->labResult->setText("操作成功!");
                SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
            }
        }
    }
}


/**
* @brief 检查是否为管理员在操作
* @date 2018/1/14
* @author han
* @param
* @return void
*/
bool AdminOption::checkIsAdmin()
{
    MyMessageBox enterBox;
    enterBox.setEnterDialog(true);
    enterBox.setEchoModePwd();
    enterBox.iniMsgBox(MyMessageBox::info_no_beep,"请输入管理员密码：");
    enterBox.setTitle("请输入管理员密码进行身份验证.");
    if(enterBox.exec()) //验证管理员密码
    {
        QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

        //密码加密
        QString pwd = g.EncriptPasswordByMD5(enterBox.Msg);

        QString AdminPwd = set->value("admin/passwd","").toString();
        if(pwd != AdminPwd)
        {
            MyMessageBox box;
            box.iniMsgBox(MyMessageBox::alert,"密码错误!");
            box.isShowDialog(true);
            box.exec();
            return false;
        }

        return true;  //密码正确
    }
    else
        return false;
}
void AdminOption::on_searchTeacherBtn_clicked()
{
    g.connectdb();

    model->setTable("teacher");
    model->select();
    model->setHeaderData(0,Qt::Horizontal,"教师编号");
    model->setHeaderData(1,Qt::Horizontal,"姓名");
    model->setHeaderData(2,Qt::Horizontal,"密码");
    model->setHeaderData(3,Qt::Horizontal,"性别");
    model->setHeaderData(4,Qt::Horizontal,"邮箱");
    model->setHeaderData(5,Qt::Horizontal,"最近一次登录");
    model->setHeaderData(6,Qt::Horizontal,"创建时间");
    model->setHeaderData(7,Qt::Horizontal,"最后修改时间");

    ui->labTeacherPageCount->setText("/"+QString::number(g.pageCount(ui->tableView)));
    ui->labTeacherRecordCount->setText(QString("共%1条记录").arg(model->rowCount()));
    ui->pushButton->setEnabled(true);
    ui->deleteTeaBtn->setEnabled(false);
    ui->cancelTeaBtn->setEnabled(false);
    deleteRow.clear();  //清空
}


/**
* @brief 删除教师信息
* @date 2018/1/12
* @author han
* @param
* @return void
*/
void AdminOption::on_deleteTeaBtn_clicked()
{
    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"确定删除教师信息？");
    if(!box.exec())
        return;

    g.connectdb();
    QModelIndexList indexList =  ui->tableView->selectionModel()->selectedRows();

    QString teacherIDList = "";
    foreach (QModelIndex index, indexList)  //检查有授课信息的教师
    {
        QString teacherID = model->record(index.row()).value(0).toString();

        g.sql.exec(QString("select * from lesson where tid = '%1'")
                   .arg(teacherID));
        if(g.sql.next())
        {
            teacherIDList+= teacherID + ",";
        }
    }

    if(teacherIDList != "")
    {
        box.iniMsgBox(MyMessageBox::alert,QString("教师编号为：%1的教师有授课信息，确定删除吗？").arg(teacherIDList));
        if(!box.exec())
            return;
    }

    foreach (QModelIndex index, indexList)
    {
        model->removeRow(index.row());
        deleteRow.append(index.row());
        ui->tableView->hideRow(index.row());
    }
    SingleTonSToast::getInstance().setMessageVDuration("请保存修改或撤销~", 5000);
    ui->labResult->setText("请保存修改或撤销~");
    ui->saveTeaBtn->setEnabled(true);
    ui->cancelTeaBtn->setEnabled(true);
}

void AdminOption::on_cancelTeaBtn_clicked()
{
    model->revertAll();
    ui->saveTeaBtn->setEnabled(true);
    foreach(int index ,deleteRow)
    {
        ui->tableView->showRow(index);
    }
    deleteRow.clear(); // 清空列表
}


/**
* @brief 保存教师信息修改
* @date 2018/1/8
* @author han
* @param
* @return void
*/
void AdminOption::on_saveTeaBtn_clicked()
{
    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"你确定要保存所有修改吗?");
    if(box.exec()==QDialog::Accepted)
    {
        ui->labResult->setText("请稍等");
        model->database().transaction();
        if(model->submitAll())
        {
            model->database().commit();
            SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
            ui->labResult->setText("保存成功...");
            ui->saveTeaBtn->setEnabled(false);
            ui->cancelTeaBtn->setEnabled(false);
        }
        else
        {
            model->database().rollback();
            box.iniMsgBox(MyMessageBox::error,QString("保存失败！错误代码:%1").arg(model
                                                              ->lastError().text()));
            box.exec();
            ui->labResult->setText("保存失败！请查询后重新尝试...");
        }
    }
}


/**
* @brief 显示客户机使用次数
* @date 2018/1/10
* @author han
* @return void
*/
void AdminOption::on_searchClientBtn_clicked()
{
    ui->labResult->clear();
    ui->clientTable->raise();
    ui->clientTable->clear();
    ui->clientTable->setColumnCount(2);

    //重新设置表头
    headerList.clear();
    headerList <<"机位号"<<"使用次数";
    ui->clientTable->setHorizontalHeaderLabels(headerList);

    g.connectdb();
    g.sql.clear();
    //获取总机数
    QString sql = "select position,COUNT(*) as count from usestatus GROUP BY position";
    g.sql.exec(sql);
    if(!g.sql.next())
    {
        MyMessageBox box;
        box.isShowDialog(true);
        box.iniMsgBox(MyMessageBox::info_no_beep,"未查询到数据");
        ui->labResult->setText("未查询到数据!");
        box.exec();
        return;
    }
    ui->clientTable->setRowCount(g.sql.size());
    int i = 0;
    do
    {
        QString position = g.sql.value(0).toString();
        QString count = g.sql.value(1).toString();
        ui->clientTable->setItem(i,0,new QTableWidgetItem(position));
        ui->clientTable->setItem(i,1,new QTableWidgetItem(count));
        //居中
        ui->clientTable->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->clientTable->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        i++;
    }while(g.sql.next());

    ui->labClientRecordCount->setText(QString("共%1条记录").arg(g.sql.size()));
    int pageCount = g.pageCount(ui->clientTable);
    ui->labClientPageCount->setText("/"+QString::number(pageCount));

    ui->labResult->setText("查询成功!");

    clientShowType = 1;

}

/**
* @brief 查询客户机详细使用情况
* @date 2018/1/8
* @author han
* @return void
*/
void AdminOption::on_searchClientDetailBtn_clicked()
{
    ui->labResult->clear();
    ui->clientTable->clear();
    ui->clientTable->setColumnCount(5);

    headerList.clear();
    headerList<<"机位号"<<"用户账号"<<"用户名"<<"上线时间"<<"下线时间";

    ui->clientTable->setHorizontalHeaderLabels(headerList);
    ui->clientTable->setColumnWidth(1,150);
    ui->clientTable->setColumnWidth(4,150);
    g.connectdb();
    g.sql.exec("select * from usestatus");
    ui->clientTable->setRowCount(g.sql.size());
    int i = 0;
    while(g.sql.next())
    {
        ui->clientTable->setItem(i,0,new QTableWidgetItem(g.sql.value(1).toString()));
        ui->clientTable->setItem(i,1,new QTableWidgetItem(g.sql.value(2).toString()));
        ui->clientTable->setItem(i,2,new QTableWidgetItem(g.sql.value(3).toString()));
        ui->clientTable->setItem(i,3,new QTableWidgetItem(g.sql.value(4).toString()));
        ui->clientTable->setItem(i,4,new QTableWidgetItem(g.sql.value(5).toString()));
        //居中
        ui->clientTable->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->clientTable->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->clientTable->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->clientTable->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->clientTable->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        i++;
    }

    ui->labClientRecordCount->setText(QString("共%1条记录").arg(g.sql.size()));
    int pageCount = g.pageCount(ui->clientTable);
    ui->labClientPageCount->setText("/"+QString::number(pageCount));

    ui->labResult->setText("查询成功!");

    clientShowType = 2;
}

/**
* @brief 清除所有学生机使用数据
* @date 2018/1/9
* @author han
* @return void
*/
void AdminOption::on_removeAllClientDataBtn_clicked()
{
    if(!checkIsAdmin())
        return;

    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"确定要清空所有数据?");
    if(box.exec() == QDialog::Accepted)
    {
        box.iniMsgBox(MyMessageBox::alert,"此操作不可恢复,确定吗?");
        if(box.exec() == QDialog::Accepted)
        {
            g.connectdb();
            bool state = g.sql.exec("truncate table usestatus");
            if(state)
            {
                ui->labResult->setText("操作成功!");
                SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
            }
        }
    }
}


/**
* @brief 保存管理员账号密码
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::on_saveSysIniBtn_clicked()
{
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    //密码加密
    QString pwd = g.EncriptPasswordByMD5(ui->leAdminPwd->text().trimmed());

    set->setValue("admin/id",ui->leAdmin->text());
    set->setValue("admin/passwd",pwd);

    delete set;
    SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
    ui->labResult->setText("保存成功!");
    Global::writeLog("","管理员修改登录账号密码.");
}


/**
* @brief 设置管理员密码
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::setAdminPwd(QString pwd)
{
    ui->leAdminPwd->setText(pwd);
}
void AdminOption::on_minBtn_clicked()
{
    this->showMinimized();
}

void AdminOption::on_maxBtn_clicked()
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

void AdminOption::on_closeBtn_clicked()
{
    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::info_no_beep,"确定退出吗？");
    if(box.exec())
        exit(0);
}

void AdminOption::mousePressEvent(QMouseEvent *event)
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

void AdminOption::mouseMoveEvent(QMouseEvent *event)
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

void AdminOption::mouseReleaseEvent(QMouseEvent *)
{
    ispress =false;
    update();
}

void AdminOption::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
        emit ui->maxBtn->clicked();
}


/**
* @brief 添加教师
* @date 2018/1/9
* @author han
* @return void
*/
void AdminOption::on_pushButton_clicked()
{
    signDialog->show();
    signDialog->activateWindow();
}

void AdminOption::on_AddTeacher(QString id, QString name)
{
    ui->labResult->setText("成功添加教师账号:"+id+",姓名:"+name);
    ui->searchTeacherBtn->click();
}


/**
* @brief 修改教师信息
* @date 2018/1/9
* @author han
* @return void
*/
void AdminOption::onModelDataChanged(QModelIndex index,QModelIndex )
{
    ui->saveTeaBtn->setEnabled(true);
    ui->cancelTeaBtn->setEnabled(true);

    if(index.column() == 2)  //修改的为密码
    {
        ui->cancelTeaBtn->click();
        SingleTonSToast::getInstance().setMessageVDuration("教师密码在此处不可修改.", 5000,1);
        return;
    }
}


/**
* @brief 导出教师信息
* @date 2018/1/10
* @author han
* @param
* @return void
*/
void AdminOption::on_exportTeacherBtn_clicked()
{
    MyMessageBox box;
    QString fileName;
#ifdef Q_OS_WIN
    fileName = QFileDialog::getSaveFileName(this,"导出数据","c:/教师账号"  ,"考勤记录(*.xlsx);;考勤记录(*.csv);;文本文件(*.txt)");
#else  //linux
    fileName = QFileDialog::getSaveFileName(this,"导出数据","home/教师账号" ,"考勤记录(*.xlsx);;考勤记录(*.csv);;文本文件(*.txt)");
#endif
    if(fileName == "")
        return;
    ui->resultProgressBar->show();
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
        QString line = "编号,姓名,密码,性别,邮件";
        out<<line<<"\n";  //写入表头

        ui->resultProgressBar->setMaximum(model->rowCount());
        for(int i = 0;i < model->rowCount();i++)
        {
            QString id = model->record(i).value(0).toString();
            QString name = model->record(i).value(1).toString();
            QString pwd = model->record(i).value(2).toString();
            QString sex = model->record(i).value(3).toString();
            QString mail = model->record(i).value(4).toString();

            line = id + ',' +name + ',' + pwd + ',' + sex + ',' +mail;
            out<<line<<"\n";
            ui->resultProgressBar->setValue(i);
        }
        ui->resultProgressBar->setValue(ui->resultProgressBar->maximum());
    }
    else
    {
        QStringList list;
        list.append("编号");
        list.append("姓名");
        list.append("密码(加密显示)");
        list.append("性别");
        list.append("邮件");

        ExcelManager ex;
        ex.ExportToExcel(list,model,fileName,ui->resultProgressBar);
    }
    ui->resultProgressBar->hide();
    ui->labResult->setText(QString("导出成功 文件路径 %1").arg(fileName));
}


/**
* @brief tab切换
* @date 2018/1/14
* @author han
* @param
* @return void
*/
void AdminOption::on_tabWidget_currentChanged(int index)
{
    if(index == 1)  //切换到教师
        ui->searchTeacherBtn->click();
    else if(index == 4)
        getAdviceHistory();
    else if(index == 5)
        getRepairHistory();
    else if(index == 6)  //切换到系统设置
    {
        initSystemSetInfo();
    }
    else if(index == 7)
    {
        QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
        QString IP = set->value("FTPServer/IP").toString();
        QString Account = set->value("FTPServer/Account").toString();
        QString Password = set->value("FTPServer/Password").toString();
        bool saveTogether = set->value("FTPServer/SaveTogether").toBool();
        QString serverPath = set->value("FTPServer/ServerPath").toString();

        if(IP == "")
            return;
        //异或加密
        QString DecryptIp = Global::getXorEncryptDecrypt(IP,21);
        QString DecryptNum = Global::getXorEncryptDecrypt(Account,21);
        QString DecryptPwd = Global::getXorEncryptDecrypt(Password,21);

        ui->leFtpIp->setText(DecryptIp);
        ui->leFtpName->setText(DecryptNum);
        ui->leFtpPwd->setText(DecryptPwd);
        ui->leServerPath->setText(serverPath);

        if(saveTogether)
            ui->radioSaveTogeder->setChecked(true);
        else
            ui->radioButton->setChecked(true);

    }
}


/**
* @brief 导入教师信息
* @date 2018/1/10
* @author han
* @param
* @return void
*/
void AdminOption::on_importTeacherBtn_clicked()
{
    MyMessageBox box;

    g.connectdb();
    int dataRow =0;
    int successRow=0;

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
            Global::writeLog("error",QString("管理员导入教师信息失败：文件打开失败"));
            box.iniMsgBox(MyMessageBox::error,"文件打开失败");
            box.isShowDialog(true);
            box.exec();
            return;
        }
        QStringList list;
        QTextStream stream(&csv);
        ui->labResult->setText("正在导入数据...");

        //默认密码加密
        QString strPwdMd5 = g.EncriptPasswordByMD5("123456");
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss");

        while(!stream.atEnd())  //按行读取数据
        {
            QString line = stream.readLine();
            if(line.contains("姓名")) //排除数据行
                continue;
            dataRow ++;
            list.clear();
            if(!line.contains(",")) //错误数据行
                continue;
            list = line.split(",",QString::KeepEmptyParts);
            if(list.size() == 2)
            {
                if(g.sql.exec(QString("insert into teacher(id,name,password,mail,createTime) "
                                      "values('%1','%2','%3','sample@sample.com',%4)")
                              .arg(list.at(0),list.at(1),strPwdMd5,time)))
                    successRow ++;
            }
            else
                continue;
        }
    }
    else  //excel类型导入
    {
        ui->resultProgressBar->show();
        ExcelManager ex;
        result = ex.ImportTeacherInfo(fileName,&dataRow,&successRow);
    }

    int failRow = successRow-dataRow;
    if(failRow < 0)
        failRow = 0;

    ui->labResult->setText(QString("成功导入%1条信息,失败%2条").arg(QString::number(successRow),QString::number(failRow)));
    Global::writeLog("work",QString("管理员导入教师信息成功"));
    if(result != "")
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,result);
        box.isShowDialog(true);
        box.exec();
    }
    else
        SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
}


/**
* @brief 上一页
* @date 2018/1/10
* @author han
* @return void
*/
void AdminOption::on_pushButton_3_clicked()
{
    g.pageUp(ui->tableView,true);
}


/**
* @brief 下一页
* @date 2018/1/10
* @author han
* @return void
*/
void AdminOption::on_pushButton_4_clicked()
{
    g.pageDown(ui->tableView,true);
}


/**
* @brief 首页
* @date 2018/1/10
* @author han
* @return void
*/
void AdminOption::on_pushButton_5_clicked()
{
    g.pageHome(ui->tableView);
}


/**
* @brief 最后一页
* @date 2018/1/10
* @author han
* @return void
*/
void AdminOption::on_pushButton_6_clicked()
{
    g.pageEnd(ui->tableView);
}

void AdminOption::on_pushButton_7_clicked()
{
    g.pageHome(ui->clientTable);
}

void AdminOption::on_pushButton_8_clicked()
{
    g.pageUp(ui->clientTable,true);
}

void AdminOption::on_pushButton_9_clicked()
{
    g.pageDown(ui->clientTable,true);
}

void AdminOption::on_pushButton_10_clicked()
{
    g.pageEnd(ui->clientTable);
}

void AdminOption::on_pushButton_11_clicked()
{
    int page = ui->leClientPage->text().toInt();
    g.pageTo(ui->clientTable,page);
}

void AdminOption::on_pushButton_12_clicked()
{
    int page = ui->leTeacherPage->text().toInt();
    g.pageTo(ui->tableView,page);
}

void AdminOption::on_pushButton_13_clicked()
{
    g.pageHome(ui->tableWidget);
}

void AdminOption::on_pushButton_14_clicked()
{
    g.pageUp(ui->tableWidget,true);
}

void AdminOption::on_pushButton_15_clicked()
{
    g.pageDown(ui->tableWidget,true);
}

void AdminOption::on_pushButton_16_clicked()
{
    g.pageEnd(ui->tableWidget);
}

void AdminOption::on_pushButton_17_clicked()
{
    int page = ui->lelLabPage->text().toInt();
    g.pageTo(ui->tableWidget,page);
}

void AdminOption::on_lelLabPage_returnPressed()
{
    ui->pushButton_17->click();
}


void AdminOption::on_leTeacherPage_returnPressed()
{
    ui->pushButton_12->click();
}

void AdminOption::on_leClientPage_returnPressed()
{
    ui->pushButton_11->click();
}


/**
* @brief 按条件查询学生机使用情况
* @date 2018/1/10
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_2_clicked()
{
    ui->labResult->clear();
    ui->clientTable->clear();
    ui->clientTable->setColumnCount(5);

    headerList.clear();
    headerList<<"机位号"<<"用户账号"<<"用户名"<<"上线时间"<<"下线时间";
    ui->clientTable->setHorizontalHeaderLabels(headerList);

    QString sql = "select * from usestatus where 1=1 ";
    QString where = "";

    //组建sql
    QString startTime = ui->startDateClient->text().trimmed();
    if(startTime.isEmpty())
        startTime = "2010-1-1";
    QString endTime = ui->endDateClient->text().trimmed();
    if(endTime.isEmpty())
        endTime = QDate::currentDate().toString("yyyy-MM-dd");
    where += QString(" and online >= '%1' and online <= '%2' ").arg(startTime,endTime);

    QString selectValue = ui->cbxClientValue->currentText().trimmed();
    QString value = ui->leClientValue->text().trimmed();
    if(!value.isEmpty())
    {
        if(selectValue == "机位号")
        {
            where += QString(" and position = '%1' ").arg(value);
        }
        else if(selectValue == "学号")
        {
            where += QString(" and userID = '%1' ").arg(value);
        }
        else if(selectValue == "姓名")
        {
            where += QString(" and username = '%1' ").arg(value);
        }
    }

    sql += where;

    Global g;
    g.connectdb();
    g.sql.exec(sql);

    ui->clientTable->setRowCount(g.sql.size());
    int i = 0;
    while(g.sql.next())
    {
        ui->clientTable->setItem(i,0,new QTableWidgetItem(g.sql.value(1).toString()));
        ui->clientTable->setItem(i,1,new QTableWidgetItem(g.sql.value(2).toString()));
        ui->clientTable->setItem(i,2,new QTableWidgetItem(g.sql.value(3).toString()));
        ui->clientTable->setItem(i,3,new QTableWidgetItem(g.sql.value(4).toString()));
        ui->clientTable->setItem(i,4,new QTableWidgetItem(g.sql.value(5).toString()));
        //居中
        ui->clientTable->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->clientTable->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->clientTable->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->clientTable->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->clientTable->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        i++;
    }
    ui->labClientRecordCount->setText(QString("共%1条记录").arg(g.sql.size()));
    int pageCount = g.pageCount(ui->clientTable);
    ui->labClientPageCount->setText("/"+QString::number(pageCount));

    ui->labResult->setText("查询成功!");

    clientShowType = 3;
}


/**
* @brief 导出当前显示的信息
* @date 2018/1/10
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_18_clicked()
{
    exportPartData(ui->clientTable);
}


/**
* @brief 导出tableWidget显示的数据
* @date 2018/1/10
* @author han
* @param
* @return void
*/
void AdminOption::exportPartData(QTableWidget *table)
{
    if(table->rowCount() == 0)
        return;

    //获取表头
    int columnCount = table->columnCount();
    QStringList list;
    for(int i = 0;i < columnCount;i++)
    {
        QTableWidgetItem *title = table->horizontalHeaderItem(i);
        list << title->text();
    }

    MyMessageBox box;

    QString fileName;
    QString time = QDateTime::currentDateTime().toString("_yyyyMMddhhmmss");
#ifdef Q_OS_WIN
    fileName = QFileDialog::getSaveFileName(this,"导出数据","c:/data"+time,"Excel(*.xlsx);;文本(*.csv);;文本(*.txt)");
#else
    fileName = QFileDialog::getSaveFileName(this,"导出数据","home/data"+time,"Excel(*.xlsx);;文本(*.csv);;文本(*.txt)");
#endif
    if(fileName == "")
        return;

    ui->resultProgressBar->show();
    QFileInfo fileinfo = QFileInfo(fileName);
    //文件后缀
    QString file_suffix = fileinfo.suffix();
    if(file_suffix != "xlsx")  //导出csv，txt格式
    {
        QFile csv(fileName);
        if(!csv.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            box.iniMsgBox(MyMessageBox::error,"数据导出失败");
            box.exec();
            return;
        }

        QTextStream out(&csv);
        QString line = "";

        for(int i = 0;i < list.size();i++)
        {
            QString s = list.at(i);
            if(i != list.size() - 1)
                line += s + ",";
            else
                line += s;

        }

        out << line << "\n";  //写入表头

        ui->resultProgressBar->setMaximum(table->model()->rowCount());
        ui->resultProgressBar->setValue(0);
        //导出
        for(int i = 0;i < table->model()->rowCount();i++)
        {
            QString line1 = "";
            for(int j = 0;j < list.size();j++)
            {
                QString s = table->item(i,j)->text();
                if(j != list.size() - 1)
                    line1 += s + ",";
                else
                    line1 += s;
            }
            out << line1 << "\n";  //写入内容
            ui->resultProgressBar->setValue(i);
        }
        ui->resultProgressBar->setValue(ui->resultProgressBar->maximum());
        csv.close();
    }
    else  //导出excel文件
    {
        int size = list.size();
        QXlsx::Document xlsx(fileName);
        QXlsx::Worksheet *sheet = xlsx.currentWorksheet();  //获取当前工作表
        //设置宽度
        xlsx.setColumnWidth(1, size, 20);

        QXlsx::Format title;
        title.setFontBold(true); //黑体
        title.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

        //写入标题
        for(int i = 0;i < size;i++)
        {
            QString s = list.at(i);
            sheet->write(1, i+1, s,title);
        }

        QXlsx::Format cHAlign;
        cHAlign.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
        ui->resultProgressBar->setMaximum(table->rowCount());
        ui->resultProgressBar->setValue(0);

        //导出
        for(int i = 0;i < table->rowCount();i++)
        {
            for(int j = 0;j < size;j++)
            {
                QString s = table->item(i,j)->text();
                sheet->write(i+2, j+1, s,cHAlign);
            }
            ui->resultProgressBar->setValue(i);
        }
        ui->resultProgressBar->setValue(ui->resultProgressBar->maximum());
        xlsx.save();  //保存
    }

    ui->resultProgressBar->hide();
    ui->resultProgressBar->setValue(0);
    ui->labResult->setText(QString("数据导出成功,文件路径 %1").arg(fileName));
}

void AdminOption::on_pushButton_19_clicked()
{

    exportPartData(ui->tableWidget);
}


/**
* @brief 查询全部实验室使用数据
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_20_clicked()
{
    ui->tableWidget->clearContents();
    QString sql = QString("select date,week,day,lesson,tname,lname,cmajor,cname,cnum,cpresentnum,etime,etype,erequire,ecategory"
                          " from worklog ");
    Global g ;
    g.connectdb();
    g.sql.exec(sql);
    int count = g.sql.size();
    ui->tableWidget->setRowCount(count);
    int i = 0;
    while(g.sql.next())
    {
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(g.sql.value(0).toString()));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(g.sql.value(1).toString()));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(g.sql.value(2).toString()));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(g.sql.value(3).toString()));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(g.sql.value(4).toString()));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(g.sql.value(5).toString()));
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(g.sql.value(6).toString()));
        ui->tableWidget->setItem(i,7,new QTableWidgetItem(g.sql.value(7).toString()));
        ui->tableWidget->setItem(i,8,new QTableWidgetItem(g.sql.value(8).toString()));
        ui->tableWidget->setItem(i,9,new QTableWidgetItem(g.sql.value(9).toString()));
        ui->tableWidget->setItem(i,10,new QTableWidgetItem(g.sql.value(10).toString()));
        ui->tableWidget->setItem(i,11,new QTableWidgetItem(g.sql.value(11).toString()));
        ui->tableWidget->setItem(i,12,new QTableWidgetItem(g.sql.value(12).toString()));
        ui->tableWidget->setItem(i,13,new QTableWidgetItem(g.sql.value(13).toString()));

        //居中
        ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,7)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,8)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,9)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,10)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,11)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,12)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,13)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        i++;
    }

    QString temp = "/"+QString::number(g.pageCount(ui->tableWidget),10);
    ui->labLabPageCount->setText(temp);
    ui->labLRecord->setText(QString("共%1条记录").arg(QString::number(count)));

    ui->labResult->setText(QString("实验室使用次数：%1").arg(i));

    LaboratoryShowType = 2;
}

/**
* @brief 改变课程
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::on_lessonListWidget_clicked(const QModelIndex &index)
{
    initLessonInfo(index.row());
}


/**
* @brief 获取课程信息
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::initLessonInfo(int index)
{
    if(lessonID.size() == 0)
    {
        ui->leLessonName->clear();
        ui->leTeacherId->clear();
        ui->leCname->clear();
        ui->leCmajor->clear();
        ui->labLessonCreateTime->clear();
        ui->cbxCetagory->clear();
        ui->cbxEtype->clear();
        ui->cbxRequire->clear();
        ui->leTeacherName->clear();
        ui->tableWidgetLesson->clearContents();

        currentLessonIndex = -1;
        return;
    }

    currentLessonIndex = index;
    g.connectdb();
    QString sql = QString(" select lesson.*,classinfo.major,teacher.name as teachername "
                          " from lesson LEFT JOIN classinfo ON lesson.cname = classinfo.classname "
                          " LEFT JOIN teacher on lesson.tid = teacher.id "
                          " where lesson.id = '%1' ").arg(lessonID.at(index));

    g.sql.exec(sql);
    classId = "";
    if(g.sql.next())  //获取课程信息
    {
        classId = g.sql.value(1).toString();
        QString cname = g.sql.value(2).toString();
        QString teacherID = g.sql.value(3).toString();
        QString lname = g.sql.value(4).toString();
        QString createTime = g.sql.value(5).toString();
        QString erequire = g.sql.value(6).toString();
        QString etype = g.sql.value(7).toString();
        QString ecategory = g.sql.value(8).toString();
        QString major = g.sql.value(9).toString();
        QString teachername = g.sql.value(10).toString();

        ui->leLessonName->setText(lname);
        ui->leTeacherId->setText(teacherID);
        ui->leCname->setText(cname);
        ui->leCmajor->setText(major);
        ui->labLessonCreateTime->setText(createTime);
        ui->cbxCetagory->setCurrentText(ecategory);
        ui->cbxEtype->setCurrentText(etype);
        ui->cbxRequire->setCurrentText(erequire);
        ui->leTeacherName->setText(teachername);
    }

    ui->tableWidgetLesson->clear();
    //设置表头
    lessonHeader.clear();
    lessonHeader <<"学号" << "姓名" << "性别" << "班别";
    ui->tableWidgetLesson->setColumnCount(4);
    ui->tableWidgetLesson->setHorizontalHeaderLabels(lessonHeader);

    classId = "class"+classId;
    sql = "select id,name,sex,classname from " + classId;

    g.sql.clear();
    g.sql.exec(sql);

    ui->tableWidgetLesson->setRowCount(g.sql.size());
    int i = 0;
    while(g.sql.next())  //获取该班学生信息
    {
        QString id = g.sql.value(0).toString();
        QString name = g.sql.value(1).toString();
        QString sex = g.sql.value(2).toString();
        QString classname = g.sql.value(3).toString();

        ui->tableWidgetLesson->setItem(i,0,new QTableWidgetItem(id));
        ui->tableWidgetLesson->setItem(i,1,new QTableWidgetItem(name));
        ui->tableWidgetLesson->setItem(i,2,new QTableWidgetItem(sex));
        ui->tableWidgetLesson->setItem(i,3,new QTableWidgetItem(classname));

        //居中
        ui->tableWidgetLesson->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetLesson->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetLesson->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetLesson->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        i++;
    }
    if(g.sql.size() == 0) //判断是否已经导入学生名单
    {
        ui->labLessonMsg->setText("名单未导入");
        ui->importStudBtn->setText("导入学生名单");
    }
    else
    {
        ui->labLessonMsg->setText("名单已导入");
        ui->importStudBtn->setText("重新导入学生名单");
    }
    ui->labStuCount->setText(QString::number(g.sql.size(),10) + " 人");
}


/**
* @brief 清空所有课程信息
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_27_clicked()
{
    if(!checkIsAdmin())
        return;

    MyMessageBox box;

    box.iniMsgBox(MyMessageBox::alert,"确定要清空所有课程数据?此操作仅清空课程表中的数据");
    if(box.exec() == QDialog::Accepted)
    {
        box.iniMsgBox(MyMessageBox::alert,"此操作不可恢复,确定吗?");
        if(box.exec() == QDialog::Accepted)
        {
            g.connectdb();
            bool state = g.sql.exec("truncate table lesson");
            if(state)
            {
                SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
                ui->labResult->setText("操作成功!");
                ui->pushButton_23->click();
            }
        }
    }
}


/**
* @brief 导入学生信息
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::on_importStudBtn_clicked()
{
    MyMessageBox box;
    if(currentLessonIndex == -1)
        return;

    g.connectdb();//连接数据库

    QString table = classId;
    if(ui->importStudBtn->text().trimmed() == "重新导入学生名单")
    {
        box.iniMsgBox(MyMessageBox::info_no_beep,"是否重新导入学生名单？此操作会清空现有的数据.");
        if(!box.exec())
            return;

        //删除表中原有数据
        bool state = g.sql.exec("truncate table " + table);
        if(!state)  //删除失败
        {
            g.writeLog("error",g.sql.lastError().text());   //写入日志
            box.iniMsgBox(MyMessageBox::info_no_beep,"导入信息出错,请重试。错误代码：" + g.sql.lastError().text());
            box.isShowDialog(true);
            box.exec();
            return;
        }
    }

    int dataRow =0;
    int successRow=0;
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
            Global::writeLog("error",QString("导入学生信息失败：文件打开失败"));
            box.iniMsgBox(MyMessageBox::error,"文件打开失败");
            box.isShowDialog(true);
            box.exec();
            return;
        }
        QStringList list;
        QTextStream stream(&csv);

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

            if(list.size() == 4)  //标准，模板
            {
                if(g.sql.exec(QString("insert into %1(id,name,sex,classname) values(%2,'%3','%4','%5')")
                              .arg(table,list.at(0),list.at(1),list.at(2),list.at(3))))
                    successRow ++;
            }
            else
                continue;
        }
    }
    else  //导入excel
    {
        ui->resultProgressBar->show();
        ExcelManager ex;
        result = ex.ReadStuInfoExcelFile(fileName,table,&dataRow,&successRow,ui->resultProgressBar);
    }

    int failRow = successRow-dataRow;
    if(failRow < 0)
        failRow = 0;

    Global::writeLog("work",QString("导入学生信息成功"));
    SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
    ui->labResult->setText(QString("成功导入 %1 条数据,").arg(successRow)+QString("失败:%1\n").arg(failRow));
    if(result != "")
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,result);
        box.isShowDialog(true);
        box.exec();
    }

    initLessonInfo(currentLessonIndex);  //更新数据
}


/**
* @brief 显示所有课程信息
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_25_clicked()
{
    ui->tableWidgetLesson->clear();
    //设置表头
    lessonHeader.clear();
    lessonHeader <<"课程" << "班别" << "教师" << "课程性质" << "实验类别" << "实验类型" << "创建时间";
    ui->tableWidgetLesson->setColumnCount(7);
    ui->tableWidgetLesson->setHorizontalHeaderLabels(lessonHeader);


    QString sql = "select lesson.*,teacher.name as teachername from lesson left join teacher on lesson.tid = teacher.id";

    g.sql.clear();
    g.sql.exec(sql);

    ui->tableWidgetLesson->setRowCount(g.sql.size());
    int i = 0;
    while(g.sql.next())  //获取全部课程信息
    {
        QString lname = g.sql.value("lname").toString();
        QString cname = g.sql.value("cname").toString();
        QString tname = g.sql.value("teachername").toString();
        QString erequire = g.sql.value("erequire").toString();
        QString etype = g.sql.value("etype").toString();
        QString ecategory = g.sql.value("ecategory").toString();
        QString createTime = g.sql.value("createTime").toString();

        ui->tableWidgetLesson->setItem(i,0,new QTableWidgetItem(lname));
        ui->tableWidgetLesson->setItem(i,1,new QTableWidgetItem(cname));
        ui->tableWidgetLesson->setItem(i,2,new QTableWidgetItem(tname));
        ui->tableWidgetLesson->setItem(i,3,new QTableWidgetItem(erequire));
        ui->tableWidgetLesson->setItem(i,4,new QTableWidgetItem(etype));
        ui->tableWidgetLesson->setItem(i,5,new QTableWidgetItem(ecategory));
        ui->tableWidgetLesson->setItem(i,6,new QTableWidgetItem(createTime));

        //居中
        ui->tableWidgetLesson->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetLesson->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetLesson->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetLesson->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetLesson->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetLesson->item(i,5)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidgetLesson->item(i,6)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        i++;
    }
}


/**
* @brief 导出课程界面的信息
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_26_clicked()
{

    exportPartData(ui->tableWidgetLesson);
}


/**
* @brief 保存课程信息
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::on_saveLessonEditBtn_clicked()
{
    if(currentLessonIndex == -1 || lessonID.size() == 0)
        return;
    QString id = lessonID.at(currentLessonIndex);
    QString cname = ui->leCname->text();
    QString teacherID = ui->leTeacherId->text();
    QString lname = ui->leLessonName->text();
    QString erequire = ui->cbxRequire->currentText();
    QString etype = ui->cbxEtype->currentText();
    QString ecategory = ui->cbxCetagory->currentText();
    QString cmajor = ui->leCmajor->text();

    if(!isTeacherExit(teacherID))
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,"该教师不存在，请检查教师编号是否正确");
        box.isShowDialog(true);
        box.exec();
        return;
    }

    QString sql = QString("update lesson set cname='%1',lname='%2',erequire='%3'"
                          ",etype='%4',ecategory='%5',tid='%7' where id='%6'")
            .arg(cname,lname,erequire,etype,ecategory,id,teacherID);
    QString sql1 = QString("update classinfo set major='%1' where classname='%2'")
            .arg(cmajor,cname);
    //qDebug() <<sql1;
    g.sql.exec(sql1);
    bool result = g.sql.exec(sql);
    if(result)
    {
        SingleTonSToast::getInstance().setMessageVDuration("保存成功", 5000);
    }
    else
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::error,QString("保存失败，请重试,错误代码：%1.").arg(g.sql.lastError().text()));
        box.isShowDialog(true);
        box.exec();
        return;
    }
}


/**
* @brief 检查教师是否存在
* @date 2018/1/11
* @author han
* @param
* @return void
*/
bool AdminOption::isTeacherExit(QString id)
{
    g.connectdb();
    g.sql.clear();
    g.sql.exec(QString("select * from teacher where id='%1'").arg(id));

    if(g.sql.next())
        return true;
    else
        return false;
}

void AdminOption::on_pushButton_23_clicked()
{
    initLesson();
}


/**
* @brief 新建课程
* @date 2018/1/11
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_21_clicked()
{
    newLessonDialog->show();
}


/**
* @brief 导入课程信息,只支持excel文件
* @date 2018/1/12
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_24_clicked()
{
    if(currentLessonIndex == -1)
        return;

    Global g;
    g.connectdb();//连接数据库

    int dataRow =0;
    int successRow=0;
    QString fileName = QFileDialog::getOpenFileName(this,"打开文件",qApp->applicationDirPath()
                                                    ,"excel(*.xlsx *.xls);;flies(*.csv)");
    if(fileName =="")
        return;

    ui->resultProgressBar->show();
    //导入
    ExcelManager ex;
    QString result = ex.ImportLessonInfo(fileName,&dataRow,&successRow,ui->resultProgressBar);

    int failRow = successRow-dataRow;
    if(failRow < 0)
        failRow = 0;

    Global::writeLog("work",QString("导入课程信息成功"));
    ui->labResult->setText(QString("成功导入 %1 条数据,").arg(successRow)+QString("失败:%1").arg(failRow));

    SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
    ui->pushButton_23->click();  //刷新

    if(result != "")
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,result);
        box.isShowDialog(true);
        box.exec();
    }
}


/**
* @brief 删除选定数据
* @date 2018/1/12
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_22_clicked()
{
    QList<QListWidgetItem*> list = ui->lessonListWidget->selectedItems();
    if(list.size() == 0)
        return;

    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"确定删除该课程吗？此操作只删除选定的课程信息以及对应学生信息，不会删除工作日志.");
    if(!box.exec())
        return;
    g.connectdb();
    g.sql.clear();
    bool hadFail = false;
    QString error = "";
    foreach(QListWidgetItem *sel ,list)  //删除选定课程
    {
        if (sel)
        {
            int r = ui->lessonListWidget->row(sel);
            g.sql.exec(QString("select cid from lesson where id = '%1'")
                       .arg(lessonID.at(r)));
            bool state = false;
            if(g.sql.next())
            {
                QString table = "class"+g.sql.value(0).toString().trimmed();
                g.sql.clear();
                state = g.sql.exec("drop table "+table);//删除表
            }
            if(!state)
            {
                hadFail = true;
                error += g.sql.lastError().text() + "<br>";
                g.writeLog("error",g.sql.lastError().text());
                continue;
            }
            //删除记录
            state = g.sql.exec(QString("delete from lesson where id = '%1'")
                               .arg(lessonID.at(r)));
            if(!state)
            {
                hadFail = true;
                error += g.sql.lastError().text() + "<br>";
                g.writeLog("error",g.sql.lastError().text());
                continue;
            }

            //移除表项

            ui->lessonListWidget->takeItem(r);
            lessonID.removeAt(r);
            if(r == currentLessonIndex)
            {
                initLessonInfo(0);  //更新数据
                if(ui->lessonListWidget->count() > 0)
                    ui->lessonListWidget->setCurrentRow(0);  //设置该项为选中
            }
        }
    }

    if(hadFail)
    {
        box.iniMsgBox(MyMessageBox::success,"删除成功,但是有删除失败项,错误信息:"+error);
        box.isShowDialog(true);
        box.exec();
    }
    else
        SingleTonSToast::getInstance().setMessageVDuration("删除成功", 5000);
    ui->labResult->setText("删除成功");
}


/**
* @brief 课程列表选定状态改变
* @date 2018/1/12
* @author han
* @param
* @return void
*/
void AdminOption::on_lessonListWidget_itemSelectionChanged()
{
    QList<QListWidgetItem*> list = ui->lessonListWidget->selectedItems();

    if(list.size() == 0)
        ui->pushButton_22->setEnabled(false);
    else
        ui->pushButton_22->setEnabled(true);
}


/**
* @brief 下载学生信息模板
* @date 2018/1/12
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_28_clicked()
{
    QString fileName;

#ifdef Q_OS_WIN
    fileName = QFileDialog::getSaveFileName(this,"下载模板","c:/学生信息模板","excel(*.xlsx);;模板(*.csv)");
#else
    fileName = QFileDialog::getSaveFileName(this,"下载模板","home/学生信息模板","excel(*.xlsx);;模板(*.csv)");
#endif
    if(fileName == "")
        return;

    QFileInfo fileinfo = QFileInfo(fileName);
    //文件后缀
    QString file_suffix = fileinfo.suffix();
    bool state = false;
    if(file_suffix == "csv")  //导出csv，txt格式 :/detail/files/学生信息模板.csv
    {
        state = QFile::copy(":/detail/files/学生信息模板.csv",fileName);
    }
    else
    {
        state = QFile::copy(":/detail/files/学生信息模板.xlsx",fileName);
    }
    if(!state)
        ui->labResult->setText(QString("模板下载失败,请检查模板文件是否存在,路径为%1/files/学生信息模板.xlsx(csv)").arg(qApp->applicationDirPath()));
    else
        ui->labResult->setText(QString("学生信息模板下载成功,所在文件夹 %1").arg(fileName));
    Global::writeLog("work",QString("管理员下载学生信息模板"));
}


/**
* @brief 下载课程信息模板
* @date 2018/1/12
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_29_clicked()
{
    QString fileName;

#ifdef Q_OS_WIN
    fileName = QFileDialog::getSaveFileName(this,"下载模板","c:/课程信息模板","excel(*.xlsx)");
#else
    fileName = QFileDialog::getSaveFileName(this,"下载模板","home/课程信息模板","excel(*.xlsx)");
#endif
    if(fileName == "")
        return;
    bool state = false;

    //复制文件:/detail/files/课程信息模板.xlsx
    state = QFile::copy(":/detail/files/课程信息模板.xlsx",fileName);
    if(!state)
        ui->labResult->setText(QString("模板下载失败,请检查模板文件是否存在,路径为%1/files/课程信息模板.xlsx").arg(qApp->applicationDirPath()));
    else
        ui->labResult->setText(QString("课程信息模板下载成功,所在文件夹 %1").arg(fileName));
    Global::writeLog("work",QString("管理员下载课程信息模板"));
}


/**
* @brief 点击表格时，把删除键设置为可用状态
* @date 2018/1/10
* @author han
* @param
* @return void
*/
void AdminOption::on_tableView_clicked(const QModelIndex &)
{
    QModelIndexList indexList =  ui->tableView->selectionModel()->selectedRows();

    if(indexList.size() != 0)
        ui->deleteTeaBtn->setEnabled(true);
    else
        ui->deleteTeaBtn->setEnabled(false);
}

/**
* @brief 查看错误日志
* @date 2018/1/14
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_30_clicked()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QString strPath = path + "/Log/error.log";//这里填写你文件的路径，有中文的话可能需要编码转换

    QFile file(strPath);
    if(!file.exists())  //新建文件
    {
        Global::writeLog("error","创建文件");
    }

    bool is_open = QDesktopServices::openUrl(QUrl::fromLocalFile(strPath));

    if(!is_open)  //打开失败
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::error,"打开错误，请检查日志文件是否存在，路径为：" + strPath);
        box.isShowDialog(true);
        box.exec();
    }
}


/**
* @brief 打开运行日志
* @date 2018/1/14
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_31_clicked()
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QString strPath = path + "/Log/work.log";//这里填写你文件的路径，有中文的话可能需要编码转换

    QFile file(strPath);
    if(!file.exists())  //新建文件
    {
        Global::writeLog("work","创建文件");
    }

    bool is_open = QDesktopServices::openUrl(QUrl::fromLocalFile(strPath));

    if(!is_open)  //打开失败
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::error,"打开错误，请检查日志文件是否存在，路径为：" + strPath);
        box.isShowDialog(true);
        box.exec();
    }
}


/**
* @brief 获取系统设置
* @date 2018/1/14
* @author han
* @param
* @return void
*/
void AdminOption::initSystemSetInfo()
{
    g.connectdb();

    ui->listWidgetOrder->clear();

    //读取配置文件获取以保存的命令
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    set->beginGroup("CmdOrder");
    QStringList temp = set->childKeys();
    for(int i = 0;i < temp.size();i++)
    {
        QString key = QString(temp.at(i));
        QString ord = set->value(key).toString();

        QString detail = key+ " : "+ord;
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetOrder);
        item->setData(Qt::DisplayRole,detail);
    }

    //获取实验类别
    ui->listWidgetType->clear();
    g.sql.exec("select eType from eType");
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetType);
        item->setData(Qt::DisplayRole,detail);
    }

    //获取实验类型
    ui->listWidgetCategory->clear();
    g.sql.exec("select Category from eCategory");
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetCategory);
        item->setData(Qt::DisplayRole,detail);
    }

    //获取课程性质
    ui->listWidgetRequire->clear();
    g.sql.exec("select erequire from erequire");
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetRequire);
        item->setData(Qt::DisplayRole,detail);
    }

}


/**
* @brief 添加远程命令
* @date 2018/1/14
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_32_clicked()
{
    QString orderName = ui->leOrderName->text().trimmed();
    QString orderDetail = ui->leOrderDetail->text().trimmed();

    MyMessageBox box;
    if(orderName.isEmpty() || orderDetail.isEmpty())
    {
        box.iniMsgBox(MyMessageBox::info_no_beep,"请补充完成名称和命令.");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    //读取配置文件
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    QString value = QString("CmdOrder/") + orderName;
    if(set->contains(value))  //判断是否存在
    {
        set->setValue(value,orderDetail);
        ui->labResult->setText("修改成功");
        Global::writeLog("",QString("管理员修改远程命令%1.").arg(orderDetail));
        SingleTonSToast::getInstance().setMessageVDuration("修改成功", 5000);
    }
    else
    {
        set->setValue(value,orderDetail);
        ui->labResult->setText("添加成功");
        Global::writeLog("",QString("管理员添加远程命令%1.").arg(orderDetail));
        SingleTonSToast::getInstance().setMessageVDuration("添加成功", 5000);
    }

    ui->listWidgetOrder->clear();
    //读取配置文件获取以保存的命令
    set->beginGroup("CmdOrder");
    QStringList temp = set->childKeys();
    for(int i = 0;i < temp.size();i++)
    {
        QString key = QString(temp.at(i));
        QString ord = set->value(key).toString();

        QString detail = key+ " : "+ord;
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetOrder);
        item->setData(Qt::DisplayRole,detail);
    }

}


/**
* @brief 命令列表点击
* @date 2018/1/14
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetOrder_clicked(const QModelIndex &index)
{
    QString order = index.data().toString();
    if(order.contains(":")) //分割
    {
        QStringList list = order.split(" : ");
        ui->leOrderDetail->setText(list.at(1));
        ui->leOrderName->setText(list.at(0));
    }
}


/**
* @brief 判断列表有没有选中项
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetOrder_itemSelectionChanged()
{
    QList<QListWidgetItem*> list = ui->listWidgetOrder->selectedItems();

    if(list.size() == 0)
    {
        ui->pushButton_40->setEnabled(false);

    }
    else
    {
        ui->pushButton_40->setEnabled(true);

    }
}

/**
* @brief 判断列表有没有选中项
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetType_itemSelectionChanged()
{
    QList<QListWidgetItem*> list = ui->listWidgetType->selectedItems();

    if(list.size() == 0)
    {
        ui->pushButton_41->setEnabled(false);

    }
    else
    {
        ui->pushButton_41->setEnabled(true);

    }
}

/**
* @brief 判断列表有没有选中项
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetRequire_itemSelectionChanged()
{
    QList<QListWidgetItem*> list = ui->listWidgetRequire->selectedItems();

    if(list.size() == 0)
    {
        ui->pushButton_42->setEnabled(false);

    }
    else
    {
        ui->pushButton_42->setEnabled(true);

    }
}
/**
* @brief 判断列表有没有选中项
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetCategory_itemSelectionChanged()
{
    QList<QListWidgetItem*> list = ui->listWidgetCategory->selectedItems();

    if(list.size() == 0)
    {
        ui->pushButton_43->setEnabled(false);

    }
    else
    {
        ui->pushButton_43->setEnabled(true);

    }
}


/**
* @brief 实验类别点击
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetType_clicked(const QModelIndex &index)
{
    QString Type = index.data().toString();
    ui->leEditType->setText(Type);
}

/**
* @brief 课程性质点击
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetRequire_clicked(const QModelIndex &index)
{
    QString Require = index.data().toString();
    ui->leEditRequire->setText(Require);
}

/**
* @brief 课程类别点击
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetCategory_clicked(const QModelIndex &index)
{
    QString Category = index.data().toString();
    ui->leEditCategory->setText(Category);
}


/**
* @brief 添加实验类型
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_34_clicked()
{
    QString type = ui->leEditType->text().trimmed();
    if(type.isEmpty())
        return;
    bool find = false;
    for(int i = 0; i < ui->listWidgetType->count();i++)  //判断是否存在
    {
        QListWidgetItem *temp =  ui->listWidgetType->item(i);
        QString currentType = temp->data(Qt::DisplayRole).toString();

        if(currentType == type)
        {
            find = true;
            break;
        }
    }
    if(find == true) //已存在
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,"该项已存在");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    else
    {
        QString sql = QString("insert into etype(etype) value('%1');").arg(type);
        g.connectdb();
        if(g.sql.exec(sql))
        {\
            SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
            ui->labResult->setText("类别添加成功");
        }
        else
        {
            MyMessageBox box;
            box.iniMsgBox(MyMessageBox::info_no_beep,"类别添加失败,原因：" + g.sql.lastError().text());
            box.isShowDialog(true);
            box.exec();
            return;
        }
    }

    //获取实验类别
    ui->listWidgetType->clear();
    g.sql.clear();
    g.sql.exec("select eType from eType");
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetType);
        item->setData(Qt::DisplayRole,detail);
    }

}


/**
* @brief 添加课程性质
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_36_clicked()
{
    QString Require = ui->leEditRequire->text().trimmed();
    if(Require.isEmpty())
        return;
    bool find = false;
    for(int i = 0; i < ui->listWidgetRequire->count();i++)  //判断是否存在
    {
        QListWidgetItem *temp =  ui->listWidgetRequire->item(i);
        QString currentRequire = temp->data(Qt::DisplayRole).toString();

        if(currentRequire == Require)
        {
            find = true;
            break;
        }
    }
    if(find == true) //已存在
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,"该项已存在");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    else
    {
        QString sql = QString("insert into eRequire(eRequire) value('%1');").arg(Require);
        g.connectdb();
        if(g.sql.exec(sql))
        {\
            SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
            ui->labResult->setText("性质添加成功");
        }
        else
        {
            MyMessageBox box;
            box.iniMsgBox(MyMessageBox::info_no_beep,"性质添加失败,原因：" + g.sql.lastError().text());
            box.isShowDialog(true);
            box.exec();
            return;
        }
    }

    //获取课程性质
    g.sql.clear();
    ui->listWidgetRequire->clear();
    g.sql.exec("select erequire from erequire");
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetRequire);
        item->setData(Qt::DisplayRole,detail);
    }
}


/**
* @brief 添加实验类型
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_38_clicked()
{
    QString Category = ui->leEditCategory->text().trimmed();
    if(Category.isEmpty())
        return;
    bool find = false;
    for(int i = 0; i < ui->listWidgetCategory->count();i++)  //判断是否存在
    {
        QListWidgetItem *temp =  ui->listWidgetCategory->item(i);
        QString currentCategory = temp->data(Qt::DisplayRole).toString();

        if(currentCategory == Category)
        {
            find = true;
            break;
        }
    }
    if(find == true) //已存在
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,"该项已存在");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    else
    {
        QString sql = QString("insert into eCategory(Category) value('%1');").arg(Category);
        g.connectdb();
        if(g.sql.exec(sql))
        {\
            SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
            ui->labResult->setText("类型添加成功");
        }
        else
        {
            MyMessageBox box;
            box.iniMsgBox(MyMessageBox::info_no_beep,"类型添加失败,原因：" + g.sql.lastError().text());
            box.isShowDialog(true);
            box.exec();
            return;
        }
    }

    //获取实验类型
    g.sql.clear();
    ui->listWidgetCategory->clear();
    g.sql.exec("select Category from eCategory");
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetCategory);
        item->setData(Qt::DisplayRole,detail);
    }
}


/**
* @brief 删除远程命令
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_40_clicked()
{
    QList<QListWidgetItem*> list = ui->listWidgetOrder->selectedItems();
    if(list.size() == 0)
        return;

    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"确定删除选定项?.");
    if(!box.exec())
        return;

    bool success = false;
    //删除
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    foreach(QListWidgetItem *temp, list)
    {
        QString order = temp->data(Qt::DisplayRole).toString();
        QString value;
        if(order.contains(":")) //分割
        {
            QStringList list = order.split(" : ");
            value = "CmdOrder/" + list.at(0);
            set->remove(value);
            success = true;
        }
    }

    if(success)  //判断是否存在
    {
        Global::writeLog("",QString("管理员删除远程命令成功."));
        SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
        ui->labResult->setText("删除成功");
    }
    else
    {
        Global::writeLog("error",QString("管理员删除远程命令失败"));
        ui->labResult->setText("删除失败");
    }
    ui->leOrderDetail->clear();
    ui->leOrderName->clear();
    ui->listWidgetOrder->clear();

    //读取配置文件获取以保存的命令
    set->beginGroup("CmdOrder");
    QStringList temp = set->childKeys();
    for(int i = 0;i < temp.size();i++)
    {
        QString key = QString(temp.at(i));
        QString ord = set->value(key).toString();

        QString detail = key+ " : "+ord;
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetOrder);
        item->setData(Qt::DisplayRole,detail);
    }
}


/**
* @brief 删除实验类别
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_41_clicked()
{
    QList<QListWidgetItem*> list = ui->listWidgetType->selectedItems();
    if(list.size() == 0)
        return;

    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"确定删除选定项?.");
    if(!box.exec())
        return;

    bool success = false;
    //删除
    g.connectdb();
    foreach(QListWidgetItem *temp, list)
    {
        QString type = temp->data(Qt::DisplayRole).toString();
        QString sql = QString("delete from etype where etype = '%1'")
                .arg(type);
        success = g.sql.exec(sql);
    }

    if(success)  //判断是否存在
    {
        Global::writeLog("",QString("管理员删除实验类别成功"));
        SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
        ui->labResult->setText("删除成功");
    }
    else
    {
        Global::writeLog("error",QString("管理员删除实验类别失败"));
        ui->labResult->setText("删除失败");
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,"删除失败,原因：" + g.sql.lastError().text());
        box.isShowDialog(true);
        box.exec();
    }
    ui->leEditType->clear();

    //获取实验类别
    ui->listWidgetType->clear();
    g.sql.exec("select eType from eType");
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetType);
        item->setData(Qt::DisplayRole,detail);
    }
}


/**
* @brief 删除课程性质
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_42_clicked()
{
    QList<QListWidgetItem*> list = ui->listWidgetRequire->selectedItems();
    if(list.size() == 0)
        return;

    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"确定删除选定项?.");
    if(!box.exec())
        return;

    bool success = false;
    //删除
    g.connectdb();
    foreach(QListWidgetItem *temp, list)
    {
        QString temp1 = temp->data(Qt::DisplayRole).toString();
        QString sql = QString("delete from eRequire where eRequire = '%1'")
                .arg(temp1);
        success = g.sql.exec(sql);
    }

    if(success)  //判断是否存在
    {
        SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
        ui->labResult->setText("删除成功");
    }
    else
    {
        ui->labResult->setText("删除失败");
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,"删除失败,原因：" + g.sql.lastError().text());
        box.isShowDialog(true);
        box.exec();
    }
    ui->leEditRequire->clear();

    //获取课程性质
    g.sql.clear();
    ui->listWidgetRequire->clear();
    g.sql.exec("select erequire from erequire");
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetRequire);
        item->setData(Qt::DisplayRole,detail);
    }
}


/**
* @brief 删除课程类别
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_43_clicked()
{

    QList<QListWidgetItem*> list = ui->listWidgetCategory->selectedItems();
    if(list.size() == 0)
        return;

    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"确定删除选定项?.");
    if(!box.exec())
        return;

    bool success = false;
    //删除
    g.connectdb();
    foreach(QListWidgetItem *temp, list)
    {
        QString temp1 = temp->data(Qt::DisplayRole).toString();
        QString sql = QString("delete from eCategory where Category = '%1'")
                .arg(temp1);
        success = g.sql.exec(sql);
    }

    if(success)  //判断是否存在
    {
        SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
        ui->labResult->setText("删除成功");
    }
    else
    {
        ui->labResult->setText("删除失败");
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,"删除失败,原因：" + g.sql.lastError().text());
        box.isShowDialog(true);
        box.exec();
    }
    ui->leEditCategory->clear();

    //获取实验类型
    g.sql.clear();
    ui->listWidgetCategory->clear();
    g.sql.exec("select Category from eCategory");
    while(g.sql.next())
    {
        QString detail = g.sql.value(0).toString();
        QListWidgetItem *item = new QListWidgetItem(ui->listWidgetCategory);
        item->setData(Qt::DisplayRole,detail);
    }
}


/**
* @brief 清空远程命令
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_33_clicked()
{
    if(!checkIsAdmin())
        return;

    MyMessageBox box;

    box.iniMsgBox(MyMessageBox::alert,"确定要清空所有远程命令?");
    if(box.exec() == QDialog::Accepted)
    {
        box.iniMsgBox(MyMessageBox::alert,"此操作不可恢复,确定吗?");
        if(box.exec() == QDialog::Accepted)
        {
            //删除
            QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
            for(int i = 0; i < ui->listWidgetOrder->count();i++)  //判断是否存在
            {
                QListWidgetItem *temp =  ui->listWidgetOrder->item(i);
                QString currentOrder = temp->data(Qt::DisplayRole).toString();

                if(currentOrder.contains(":"))
                {
                    QString deleteOrderName = currentOrder.split(":").at(0);
                    deleteOrderName = deleteOrderName.trimmed();
                    set->remove("CmdOrder/" + deleteOrderName);

                }
            }
            SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
            ui->labResult->setText("操作成功!");
            ui->listWidgetOrder->clear();
        }
    }


}


/**
* @brief 清空课程类别
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_35_clicked()
{
    if(!checkIsAdmin())
        return;

    MyMessageBox box;

    box.iniMsgBox(MyMessageBox::alert,"确定清空所有课程类别吗?");
    if(box.exec() == QDialog::Accepted)
    {
        box.iniMsgBox(MyMessageBox::alert,"此操作不可恢复,确定吗?");
        if(box.exec() == QDialog::Accepted)
        {
            g.connectdb();
            bool state = g.sql.exec("truncate table etype");
            if(state)
            {
                SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
                ui->labResult->setText("操作成功!");
                ui->listWidgetType->clear();
            }
        }
    }
}


/**
* @brief 清空所有课程性质
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_37_clicked()
{
    if(!checkIsAdmin())
        return;

    MyMessageBox box;

    box.iniMsgBox(MyMessageBox::alert,"确定清空所有课程性质吗?");
    if(box.exec() == QDialog::Accepted)
    {
        box.iniMsgBox(MyMessageBox::alert,"此操作不可恢复,确定吗?");
        if(box.exec() == QDialog::Accepted)
        {
            g.connectdb();
            bool state = g.sql.exec("truncate table eRequire");
            if(state)
            {
                SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
                ui->labResult->setText("操作成功!");
                ui->listWidgetRequire->clear();
            }
        }
    }
}


/**
* @brief 清空所有课程类型
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_39_clicked()
{
    if(!checkIsAdmin())
        return;

    MyMessageBox box;

    box.iniMsgBox(MyMessageBox::alert,"确定清空所有课程类型吗?");
    if(box.exec() == QDialog::Accepted)
    {
        box.iniMsgBox(MyMessageBox::alert,"此操作不可恢复,确定吗?");
        if(box.exec() == QDialog::Accepted)
        {
            g.connectdb();
            bool state = g.sql.exec("truncate table eCategory");
            if(state)
            {
                SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
                ui->labResult->setText("操作成功!");
                ui->listWidgetCategory->clear();
            }
        }
    }
}


/**
* @brief ftp测试
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_FtpTestBtn_clicked()
{
    FtpServerIP = ui->leFtpIp->text().trimmed();
    FtpNumber = ui->leFtpName->text().trimmed();
    FtpPwd = ui->leFtpPwd->text().trimmed();

    if(FtpServerIP.isEmpty())
    {
        ui->labResult->setText("请输入服务器ip.");
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::information,"请输入服务器ip.");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    if(FtpNumber.isEmpty())
    {
        ui->labResult->setText("请输入账号.");
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::information,"请输入账号.");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    if(FtpPwd.isEmpty())
    {
        ui->labResult->setText("请输入密码.");
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::information,"请输入密码.");
        box.isShowDialog(true);
        box.exec();
        return;
    }

    QString fileName = "ftpTest" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".txt";
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString path = appDataPath + fileName;
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
    if(ui->radioSaveTogeder->isChecked())
    {
        QString serverPath = ui->leServerPath->text().trimmed();
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
    }
    else
        ftp->uploadFile(path,fileName);  //上传测试文件

    reply = ftp->getRelay();  //获取
    connect(reply, SIGNAL(finished()), this, SLOT(OnFinishRelay()));
    connectFtpTimer->start(1000);
    ui->labFtpTestResult->clear();
    ui->labFtpTestIcon->clear();
    ui->labFtpTestResult->setText("正在连接:10");

    timeOutTimes = 10;
    isFtpTest = true;
}


/**
* @brief 网络超时
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::onTimeOut()
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
void AdminOption::OnFinishRelay()
{
    connectFtpTimer->stop();
    MyMessageBox box;
    ui->labFtpTestResult->clear();
    ui->labFtpTestIcon->clear();
    QImage tick(":/images/images/else/tick.ico");
    QImage wrong(":/images/images/else/wrong.ico");

    //无错误返回
    if(reply->error() == QNetworkReply::NoError)
    {
        if(isFtpTest)
        {
            ui->labFtpTestIcon->setPixmap(QPixmap::fromImage(tick));
            ui->labFtpTestResult->setText("测试成功");
            box.iniMsgBox(MyMessageBox::success,"连接成功.");
            box.isShowDialog(true);
            box.exec();
            isFtpTest = false;
        }
        else
        {
            box.iniMsgBox(MyMessageBox::success,QString("文件发送成功"));
            box.isShowDialog(true);
            box.exec();

        }
    }
    else
    {
        //处理错误
        if(isFtpTest)
        {
            ui->labFtpTestIcon->setPixmap(QPixmap::fromImage(wrong));
            if(!ui->radioSaveTogeder->isChecked())
                ui->labFtpTestResult->setText(QString("连接失败，错误原因：%1").arg(reply->errorString()));
            else
                ui->labFtpTestResult->setText(QString("连接失败，请检查路径是否存在，错误原因：%1").arg(reply->errorString()));
            box.iniMsgBox(MyMessageBox::error,QString("连接失败，错误原因：%1<br>").arg(reply->errorString()));
            box.isShowDialog(true);
            box.exec();
            isFtpTest = false;
        }
        else
        {
            box.iniMsgBox(MyMessageBox::error,QString("文件发送失败，错误原因：%1<br>").arg(reply->errorString()));
            box.isShowDialog(true);
            box.exec();
        }
    }


    reply->deleteLater();
}


/**
* @brief 保存ftp账号密码
* @date 2018/1/16
* @author han
* @param
* @return void
*/
void AdminOption::on_FtpSaveBtn_clicked()
{
    //删除
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    FtpServerIP = ui->leFtpIp->text().trimmed();
    FtpNumber = ui->leFtpName->text().trimmed();
    FtpPwd = ui->leFtpPwd->text().trimmed();
    if(FtpServerIP.isEmpty())
    {
        ui->labResult->setText("请输入服务器ip.");
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::information,"请输入服务器ip.");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    if(FtpNumber.isEmpty())
    {
        ui->labResult->setText("请输入账号.");
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::information,"请输入账号.");
        box.isShowDialog(true);
        box.exec();
        return;
    }
    if(FtpPwd.isEmpty())
    {
        ui->labResult->setText("请输入密码.");
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::information,"请输入密码.");
        box.isShowDialog(true);
        box.exec();
        return;
    }

    QString serverPath = "";
    if(ui->radioSaveTogeder->isChecked())
    {
        serverPath = ui->leServerPath->text().trimmed();
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
        set->setValue("FTPServer/ServerPath",serverPath);
        set->setValue("FTPServer/SaveTogether",true);
    }
    else
    {
        set->setValue("FTPServer/SaveTogether",false);
    }
    //异或加密
    QString EncryptIp = Global::getXorEncryptDecrypt(FtpServerIP,21);
    QString EncryptNum = Global::getXorEncryptDecrypt(FtpNumber,21);
    QString EncryptPwd = Global::getXorEncryptDecrypt(FtpPwd,21);

    set->setValue("FTPServer/IP",EncryptIp);
    set->setValue("FTPServer/Account",EncryptNum);
    set->setValue("FTPServer/Password",EncryptPwd);

    QImage tick(":/images/images/else/tick.ico");
    ui->labFtpTestIcon->setPixmap(QPixmap::fromImage(tick));
    ui->labFtpTestResult->setText("保存成功");

    SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
    Global::writeLog("","管理员修改FTP服务器配置.");
}

void AdminOption::on_cbxAdviceType_currentIndexChanged(int)
{
    getAdviceHistory();
}


/**
* @brief 建议列表点击，
* @date 2018/1/18
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetAdvice_clicked(const QModelIndex &index)
{
    int id = AdInfoList.at(index.row())->id;
    int solved = AdInfoList.at(index.row())->isSolved;
    if(solved == 0)
    {
        QString sql = QString("update teacherAdvice set solved = 1 where id = '%1'")
                .arg(QString::number(id));
        g.connectdb();
        g.sql.clear();
        g.sql.exec(sql);
        getAdviceHistory();
    }
}


/**
* @brief 清空所有建议
* @date 2018/1/18
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_44_clicked()
{
    MyMessageBox box;

    box.iniMsgBox(MyMessageBox::alert,"确定清空所有历史建议？");
    if(box.exec() == QDialog::Accepted)
    {
        g.connectdb();
        bool state = g.sql.exec("truncate table teacherAdvice");
        if(state)
        {
            SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
            ui->labResult->setText("操作成功!");
            getAdviceHistory();
        }
    }
}


/**
* @brief 删除选定的建议
* @date 2018/1/18
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_45_clicked()
{
    QList<QListWidgetItem*> list = ui->listWidgetAdvice->selectedItems();
    if(list.size() == 0)
        return;

    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"确定删除选定的内容吗？");
    if(!box.exec())
        return;
    g.connectdb();
    g.sql.clear();
    foreach(QListWidgetItem *sel ,list)  //删除选定课程
    {
        if (sel)
        {
            int r = ui->listWidgetAdvice->row(sel);

            //删除记录
            bool state = g.sql.exec(QString("delete from teacherAdvice where id = '%1'")
                                    .arg(QString::number(AdInfoList.at(r)->id)));
            if(!state)
            {
                g.writeLog("error",g.sql.lastError().text());
                continue;
            }

            //移除表项
            ui->listWidgetAdvice->takeItem(r);
            AdInfoList.removeAt(r);
        }
    }

    getAdviceHistory();
    SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
    ui->labResult->setText("删除成功");
}

void AdminOption::on_listWidgetAdvice_itemSelectionChanged()
{
    QList<QListWidgetItem*> list = ui->listWidgetAdvice->selectedItems();
    if(list.size() == 0)
        ui->pushButton_45->setEnabled(false);
    else
        ui->pushButton_45->setEnabled(true);
}


/**
* @brief 获取维修信息
* @date 2018/1/18
* @author han
* @param
* @return void
*/
void AdminOption::getRepairHistory()
{
    Global g;
    g.connectdb();

    QString sql ;
    if(ui->cbxRepairType->currentText().trimmed() == "已处理")
        sql = QString(" select id,Detail,solved,createTime "
                      " from erepair where solved = 1 ");
    else if(ui->cbxRepairType->currentText().trimmed() == "未处理")
        sql = QString(" select id,Detail,solved,createTime "
                      " from erepair where solved = 0 ");
    else
        sql = QString(" select id,Detail,solved,createTime "
                      " from erepair ");

    g.sql.exec(sql);
    if(g.sql.size() == 0)
        ui->pushButton_46->setEnabled(false);
    else
        ui->pushButton_46->setEnabled(true);


    int solveCount = 0;
    int unsolveCount = 0;

    int i = 0;
    ui->listWidgetRepair->clear();
    repairID.clear();
    repairDetail.clear();
    while(g.sql.next())
    {
        QString id = g.sql.value(0).toString();
        QString detail = g.sql.value(1).toString().trimmed();
        int isSolved = g.sql.value(2).toInt();
        QString createTime = g.sql.value(3).toString().trimmed();
        if(createTime.contains("T"))
            createTime = createTime.replace("T"," ");
        detail = createTime + "\n" + detail;

        lab = new QLabel;
        lab->setWordWrap(true);
        lab->adjustSize();
        lab->setGeometry(QRect(328, 240, 329, 27*4));  //四倍行距
        lab->setAlignment(Qt::AlignTop);
        lab->setText(detail);
        repairDetail.append(detail);

        //添加自定义控件
        icon = new QLabel;
        btn = new QPushButton;
        btn->setMinimumWidth(100);
        btn->setMaximumWidth(100);
        icon->setMaximumSize(20,20);
        icon->setMinimumSize(20,20);
        if(isSolved == g.REPAIRSOLVED)  //已受理
        {
            solveCount++;
            icon->setPixmap(solved);
            btn->setText("已处理");
            btn->setEnabled(false);
        }
        else
        {
            unsolveCount++;
            icon->setPixmap(unSolved);
            btn->setText("处理");
            connect(btn,SIGNAL(clicked()),this,SLOT(onRepairButtonClick()));
        }

        layout = new QHBoxLayout();
        ItemWidget = new QWidget;
        layout->addWidget(icon);
        layout->addWidget(lab);
        layout->addStretch();
        layout->addWidget(btn);
        layout->setContentsMargins(5,0,5,4);
        ItemWidget->setLayout(layout);
        ItemWidget->adjustSize();

        QListWidgetItem *Item = new QListWidgetItem(ui->listWidgetRepair);
        Item->setSizeHint(QSize(0,ItemWidget->height()+20));
        ui->listWidgetRepair->addItem(Item);
        ui->listWidgetRepair->setItemWidget(Item,ItemWidget);
        repairID.append(id);  //把对应ID保存
        i++;
    }
    QString ad = QString("共%1条记录，已处理%2条，待处理%3条")
            .arg(QString::number(i),QString::number(solveCount),QString::number(unsolveCount));
    ui->labRepairResult->setText(ad);
}


/**
* @brief 处理
* @date 2018/1/18
* @author han
* @param
* @return void
*/
void AdminOption::onRepairButtonClick()
{
    //转换为当前相对坐标
    QPoint p = ui->listWidgetRepair->mapFromGlobal(QCursor::pos());
    QModelIndex index = ui->listWidgetRepair->indexAt(p);
    QString id = repairID.at(index.row());

    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::info_no_beep,"确定该问题已处理完毕？");
    if(!box.exec())
    {
        return;
    }

    g.connectdb();
    g.sql.exec(QString("update erepair set solved = 1 where id = '%1'").arg(id));

    getRepairHistory();
}


void AdminOption::on_cbxRepairType_currentIndexChanged(int )
{
    getRepairHistory();
}

void AdminOption::on_listWidgetRepair_itemSelectionChanged()
{
    QList<QListWidgetItem*> list = ui->listWidgetRepair->selectedItems();
    if(list.size() == 0)
        ui->pushButton_47->setEnabled(false);
    else
        ui->pushButton_47->setEnabled(true);
}


/**
* @brief 删除选定项
* @date 2018/1/18
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_47_clicked()
{
    QList<QListWidgetItem*> list = ui->listWidgetRepair->selectedItems();
    if(list.size() == 0)
        return;

    MyMessageBox box;
    box.iniMsgBox(MyMessageBox::alert,"确定删除选定的内容吗？");
    if(!box.exec())
        return;
    g.connectdb();
    g.sql.clear();
    foreach(QListWidgetItem *sel ,list)  //删除选定课程
    {
        if (sel)
        {
            int r = ui->listWidgetRepair->row(sel);

            //删除记录
            bool state = g.sql.exec(QString("delete from erepair where id = '%1'")
                                    .arg(repairID.at(r)));
            if(!state)
            {
                g.writeLog("error",g.sql.lastError().text());
                continue;
            }

            //移除表项
            ui->listWidgetRepair->takeItem(r);
            repairID.removeAt(r);
        }
    }

    getRepairHistory();
   SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
    ui->labResult->setText("删除成功");
}


/**
* @brief 清空维修记录
* @date 2018/1/18
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_46_clicked()
{
    MyMessageBox box;

    box.iniMsgBox(MyMessageBox::alert,"确定清空所有维修记录？");
    if(box.exec() == QDialog::Accepted)
    {
        g.connectdb();
        bool state = g.sql.exec("truncate table erepair");
        if(state)
        {
            SingleTonSToast::getInstance().setMessageVDuration("操作成功", 5000);
            ui->labResult->setText("操作成功!");
        }
        getRepairHistory();
    }
}


void AdminOption::rightClickedOperation()
{
    QAction * action = qobject_cast<QAction*>(sender());

    if(copyAction == action)  //复制
    {
        QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
        QString originalText;
        QModelIndex index;
        QString otherData = "";

        if(ui->tabWidget->currentIndex() == 1)
        {
            index =  ui->tableView->currentIndex();
        }
        else if(ui->tabWidget->currentIndex() == 0)
        {
            index =  ui->tableWidget->currentIndex();
        }
        else if(ui->tabWidget->currentIndex() == 2)
        {
            index =  ui->clientTable->currentIndex();
        }
        else if(ui->tabWidget->currentIndex() == 3)
        {
            if(ui->lessonListWidget->hasFocus())
                index =  ui->lessonListWidget->currentIndex();
            else
                index =  ui->tableWidgetLesson->currentIndex();
        }
        else if(ui->tabWidget->currentIndex() == 4)
        {
            index =  ui->listWidgetAdvice->currentIndex();
        }
        else if(ui->tabWidget->currentIndex() == 5)
        {
            index =  ui->listWidgetRepair->currentIndex();
            otherData = repairDetail.at(index.row());
            qDebug() <<otherData;
        }
        else if(ui->tabWidget->currentIndex() == 6)
        {
            if(ui->listWidgetOrder->hasFocus())
                index =  ui->listWidgetOrder->currentIndex();
            else if(ui->listWidgetType->hasFocus())
                index =  ui->listWidgetType->currentIndex();
            else if(ui->listWidgetRequire->hasFocus())
                index =  ui->listWidgetRequire->currentIndex();
            else if(ui->listWidgetCategory->hasFocus())
                index =  ui->listWidgetCategory->currentIndex();
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

        if(ui->tabWidget->currentIndex() == 1)
        {
            QModelIndex index =  ui->tableView->currentIndex();
            model->setData(index,originalText);
        }

    }
    else if(refreshAction == action)  //刷新
    {
        if(ui->tabWidget->currentIndex() == 1)
        {
            ui->searchTeacherBtn->click();
        }
        else if(ui->tabWidget->currentIndex() == 0)
        {

            if(LaboratoryShowType == 2)
                ui->pushButton_20->click();
            else if(LaboratoryShowType == 3)
                ui->searchMajorBtn->click();
            else
                ui->labUsedBtn->click();
        }
        else if(ui->tabWidget->currentIndex() == 2)
        {

            if(clientShowType == 2)
                ui->searchClientDetailBtn->click();
            else if(clientShowType == 3)
                ui->pushButton_2->click();
            else
                ui->searchClientBtn->click();
        }
        else if(ui->tabWidget->currentIndex() == 3)
        {
            ui->pushButton_23->click();
        }
        else if(ui->tabWidget->currentIndex() == 6)
        {
            initSystemSetInfo();
        }
    }
    else if(deleteAction == action)  //删除
    {
        if(ui->tabWidget->currentIndex() == 1)
        {
            ui->deleteTeaBtn->click();
        }
        else if(ui->tabWidget->currentIndex() == 3)
        {
            ui->pushButton_22->click();
        }
        else if(ui->tabWidget->currentIndex() == 4)
        {
            ui->pushButton_45->click();
        }
        else if(ui->tabWidget->currentIndex() == 5)
        {
            ui->pushButton_47->click();
        }
        else if(ui->tabWidget->currentIndex() == 6)
        {
            if(ui->listWidgetOrder->hasFocus())
                ui->pushButton_40->click();
            else if(ui->listWidgetType->hasFocus())
                ui->pushButton_41->click();
            else if(ui->listWidgetRequire->hasFocus())
                ui->pushButton_42->click();
            else if(ui->listWidgetCategory->hasFocus())
                ui->pushButton_43->click();
        }
    }
    else if(exportAction == action) //导出
    {
        if(ui->tabWidget->currentIndex() == 1)
        {
            ui->exportTeacherBtn->click();
        }
        if(ui->tabWidget->currentIndex() == 0)
        {
            ui->pushButton_19->click();
        }
        if(ui->tabWidget->currentIndex() == 2)
        {
            ui->pushButton_18->click();
        }
        else if(ui->tabWidget->currentIndex() == 3)
        {
            ui->pushButton_26->click();
        }
    }
    else if(importAction == action)  //导入
    {
        if(ui->tabWidget->currentIndex() == 1)
        {
            ui->importTeacherBtn->click();
        }
        else if(ui->tabWidget->currentIndex() == 3)
        {
            ui->pushButton_24->click();
        }
    }
    else if(quitAction == action)//退出
    {
        exit(0);
    }
    else if(logoutAction == action) //切换登录
    {
        this->hide();
        emit logoutFromAdmin();
    }
    else if(addClassAction == action) //添加新课程
    {
        ui->pushButton_21->click();
    }
    else if(addSetAction == action)  //设置界面的添加
    {
        if(ui->listWidgetOrder->hasFocus())
            ui->pushButton_32->click();
        else if(ui->listWidgetType->hasFocus())
            ui->pushButton_34->click();
        else if(ui->listWidgetRequire->hasFocus())
            ui->pushButton_36->click();
        else if(ui->listWidgetCategory->hasFocus())
            ui->pushButton_38->click();
    }
}


/**
* @brief 教师表右键菜单
* @date 2018/1/19
* @author han
* @param
* @return void
*/
void AdminOption::on_tableView_customContextMenuRequested(const QPoint )
{
    popMenu->clear();
    ui->deleteTeaBtn->setEnabled(true);
    popMenu->addAction(refreshAction);
    popMenu->addAction(copyAction);
    popMenu->addAction(pasteAction);
    popMenu->addAction(deleteAction);
    popMenu->addSeparator();
    popMenu->addAction(importAction);
    popMenu->addAction(exportAction);

    QModelIndexList list = ui->tableView->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        deleteAction->setEnabled(false);
        pasteAction->setEnabled(false);
        copyAction->setEnabled(false);
    }
    else
    {
        pasteAction->setEnabled(true);
        deleteAction->setEnabled(true);
        copyAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());

}


/**
* @brief 实验室使用表右键菜单
* @date 2018/1/19
* @author han
* @param
* @return void
*/
void AdminOption::on_tableWidget_customContextMenuRequested(const QPoint )
{
    popMenu->clear();
    popMenu->addAction(refreshAction);
    popMenu->addAction(copyAction);
    popMenu->addSeparator();
    popMenu->addAction(exportAction);

    QList<QTableWidgetItem *> list= ui->tableWidget->selectedItems();
    if(list.size() == 0)
        copyAction->setEnabled(false);
    else
        copyAction->setEnabled(true);
    popMenu->exec(QCursor::pos());
}


/**
* @brief 标题右键菜单
* @date 2018/1/20
* @author han
* @param
* @return void
*/
void AdminOption::on_AdminTitleW_customContextMenuRequested(const QPoint)
{
    titlemenu->exec(QCursor::pos());
}


/**
* @brief 学生机信息表右键菜单
* @date 2018/1/20
* @author han
* @param
* @return void
*/
void AdminOption::on_clientTable_customContextMenuRequested(const QPoint)
{
    popMenu->clear();
    popMenu->addAction(refreshAction);
    popMenu->addAction(copyAction);
    popMenu->addSeparator();
    popMenu->addAction(exportAction);

    QList<QTableWidgetItem *> list= ui->clientTable->selectedItems();
    if(list.size() == 0)
        copyAction->setEnabled(false);
    else
        copyAction->setEnabled(true);
    popMenu->exec(QCursor::pos());
}


/**
* @brief 课程列表右键菜单
* @date 2018/1/20
* @author han
* @param
* @return void
*/
void AdminOption::on_lessonListWidget_customContextMenuRequested(const QPoint)
{
    popMenu->clear();
    ui->pushButton_22->setEnabled(true);
    popMenu->addAction(refreshAction);
    popMenu->addAction(copyAction);
    popMenu->addAction(deleteAction);
    popMenu->addAction(addClassAction);
    popMenu->addAction(importAction);

    QModelIndexList list = ui->lessonListWidget->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        deleteAction->setEnabled(false);
        copyAction->setEnabled(false);
    }
    else
    {
        deleteAction->setEnabled(true);
        copyAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());

}

void AdminOption::on_tableWidgetLesson_customContextMenuRequested(const QPoint)
{
    popMenu->clear();
    popMenu->addAction(copyAction);
    popMenu->addAction(exportAction);

    QList<QTableWidgetItem *> list= ui->tableWidgetLesson->selectedItems();
    if(list.size() == 0)
        copyAction->setEnabled(false);
    else
        copyAction->setEnabled(true);
    popMenu->exec(QCursor::pos());
}


/**
* @brief 建议页面右键菜单
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetAdvice_customContextMenuRequested(const QPoint )
{
    popMenu->clear();
    ui->pushButton_45->setEnabled(true);
    popMenu->addAction(copyAction);
    popMenu->addAction(deleteAction);

    QModelIndexList list = ui->listWidgetAdvice->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        deleteAction->setEnabled(false);
        copyAction->setEnabled(false);
    }
    else
    {
        deleteAction->setEnabled(true);
        copyAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());
}


/**
* @brief 维修界面右键菜单
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetRepair_customContextMenuRequested(const QPoint)
{
    popMenu->clear();
    ui->pushButton_47->setEnabled(true);
    popMenu->addAction(copyAction);
    popMenu->addAction(deleteAction);

    QModelIndexList list = ui->listWidgetRepair->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        deleteAction->setEnabled(false);
        copyAction->setEnabled(false);
    }
    else
    {
        deleteAction->setEnabled(true);
        copyAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());
}


/**
* @brief 远程命令列表右键菜单
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetOrder_customContextMenuRequested(const QPoint)
{
    popMenu->clear();
    ui->pushButton_40->setEnabled(true);
    popMenu->addAction(refreshAction);
    popMenu->addAction(addSetAction);
    popMenu->addAction(copyAction);
    popMenu->addAction(deleteAction);

    QModelIndexList list = ui->listWidgetOrder->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        deleteAction->setEnabled(false);
        copyAction->setEnabled(false);
    }
    else
    {
        deleteAction->setEnabled(true);
        copyAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());
}


/**
* @brief 实验类型右键菜单
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetType_customContextMenuRequested(const QPoint )
{
    popMenu->clear();
    ui->pushButton_41->setEnabled(true);
    popMenu->addAction(refreshAction);
    popMenu->addAction(addSetAction);
    popMenu->addAction(copyAction);
    popMenu->addAction(deleteAction);

    QModelIndexList list = ui->listWidgetType->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        deleteAction->setEnabled(false);
        copyAction->setEnabled(false);
    }
    else
    {
        deleteAction->setEnabled(true);
        copyAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());
}


/**
* @brief 课程性质右键菜单
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetRequire_customContextMenuRequested(const QPoint)
{
    popMenu->clear();
    ui->pushButton_42->setEnabled(true);
    popMenu->addAction(refreshAction);
    popMenu->addAction(addSetAction);
    popMenu->addAction(copyAction);
    popMenu->addAction(deleteAction);

    QModelIndexList list = ui->listWidgetRequire->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        deleteAction->setEnabled(false);
        copyAction->setEnabled(false);
    }
    else
    {
        deleteAction->setEnabled(true);
        copyAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());
}


/**
* @brief 实验类型右键菜单
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void AdminOption::on_listWidgetCategory_customContextMenuRequested(const QPoint )
{
    popMenu->clear();
    ui->pushButton_43->setEnabled(true);
    popMenu->addAction(refreshAction);
    popMenu->addAction(addSetAction);
    popMenu->addAction(copyAction);
    popMenu->addAction(deleteAction);

    QModelIndexList list = ui->listWidgetCategory->selectionModel()->selectedRows();
    if(list.size() == 0)
    {
        deleteAction->setEnabled(false);
        copyAction->setEnabled(false);
    }
    else
    {
        deleteAction->setEnabled(true);
        copyAction->setEnabled(true);
    }

    popMenu->exec(QCursor::pos());
}


/**
* @brief 选择本地路径
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_48_clicked()
{
    QString file_path = QFileDialog::getExistingDirectory(this,"请选择本地存储考勤记录路径","./");
    if(file_path.isEmpty())
    {
        return;
    }
    else
    {
        ui->leShowLocalPath->setText(file_path);
    }
}


/**
* @brief 保存本地存储路径
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_49_clicked()
{
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);
    QString path = ui->leShowLocalPath->text();
    if(path.trimmed().isEmpty())
    {
        SingleTonSToast::getInstance().setMessageVDuration("请选择或输入路径", 5000,1);
        return;
    }
    if(path == "AttendanceRecord")  //默认路径
        path = QDir::currentPath() + "/"+path;

    QDir dir(path);

    if(!dir.exists())  //判断路径是否存在
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,"路径不存在，是否创建？");
        if(!box.exec())
        {
            SingleTonSToast::getInstance().setMessageVDuration("保存失败", 5000,1);
            return;
        }
        if(!dir.mkpath(path))
        {
            box.iniMsgBox(MyMessageBox::error,"路径创建失败，请检查路径是否有非法字符");
            box.isShowDialog(true);
            box.exec();
            return;
        }
    }
    ui->labLocalPath->setText(path);
    set->setValue("LocalSavePath/Path",path);
    Global::writeLog("","管理员修改本地考勤记录保存位置.");
    SingleTonSToast::getInstance().setMessageVDuration("保存成功", 5000);
}


/**
* @brief 打开本地存储路径
* @date 2018/1/21
* @author han
* @param
* @return void
*/
void AdminOption::on_pushButton_50_clicked()
{
    QString path = ui->labLocalPath->text().trimmed();
    if(!path.isEmpty())
    {//两种打开方式
        if(!QDesktopServices::openUrl(QUrl::fromLocalFile(path)))  //win10通过
        {
            if(!QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode)))
            {
                SingleTonSToast::getInstance().setMessageVDuration("打开失败,请检查路径.", 5000,1);
                return;
            }
        }
    }
    else
    {
        SingleTonSToast::getInstance().setMessageVDuration("请先设置路径", 5000,1);
        return;
    }
}
