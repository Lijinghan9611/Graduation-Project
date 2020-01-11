#include "SignIn.h"
#include "ui_SignIn.h"
#include "CoolTitleBar.h"
#include "Global.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QCryptographicHash>
#include <QPalette>

/**
* @brief 添加、找回密码和添加新课程为同一ui
* @date 2017/12/10
* @author han
* @return void
*/
SignIn::SignIn(QWidget *parent,QString str) :
    QDialog(parent),
    ui(new Ui::SignIn)
{
    ui->setupUi(this);
    kind = str;
    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags( Qt::FramelessWindowHint| windowFlags());
    CoolTitleBar * bar = new CoolTitleBar(this);

    QVBoxLayout *pLayout = new QVBoxLayout(this);
    pLayout->addWidget(bar);
    pLayout->addStretch(0);
    pLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pLayout);
    ui->pushButton->setObjectName("surebutton");
    ui->returnBtn->setObjectName("commonbutton");


    if(kind == "findPwd")  //找回密码
    {
        bar->m_pTitleLabel->setText("电子教室教学管理软件-找回密码");
        ui->lineEdit_2->setEchoMode(QLineEdit::Password);
        ui->lineEdit_3->setEchoMode(QLineEdit::Password);
        ui->label_2->setText("新密码:");
        ui->label_16->setText("必填");
        ui->label_16->setStyleSheet("color:red;");
        ui->label->setText("登录账户:");
        ui->label_19->hide();
        connect(ui->lineEdit,SIGNAL(editingFinished()),this,SLOT(check()));
        connect(ui->lineEdit_2,SIGNAL(editingFinished()),this,SLOT(check()));
        connect(ui->lineEdit_3,SIGNAL(editingFinished()),this,SLOT(check()));
    }
    else if(kind == "editTeacher")  //修改教师
    {
        ui->pushButton->setText("保存修改");
        bar->m_pTitleLabel->setText("电子教室教学管理软件-修改个人信息");
        ui->lineEdit_2->setEchoMode(QLineEdit::Password);
        ui->lineEdit_3->setEchoMode(QLineEdit::Password);
        connect(ui->lineEdit_2,SIGNAL(editingFinished()),this,SLOT(check()));
        connect(ui->lineEdit_3,SIGNAL(editingFinished()),this,SLOT(check()));
    }
    else if(kind == "zhuce")  //添加教师
    {
        ui->label_19->hide();
        bar->m_pTitleLabel->setText("电子教室教学管理软件-添加教师");
        ui->lineEdit_2->setEchoMode(QLineEdit::Password);
        ui->lineEdit_3->setEchoMode(QLineEdit::Password);
        connect(ui->lineEdit,SIGNAL(editingFinished()),this,SLOT(check()));
        connect(ui->lineEdit_2,SIGNAL(editingFinished()),this,SLOT(check()));
        connect(ui->lineEdit_3,SIGNAL(editingFinished()),this,SLOT(check()));
    }
    else if(kind =="newclass")  //新建课程
    {
        bar->m_pTitleLabel->setText("电子教室教学管理软件-新建课程");
        ui->radioButton->hide();
        ui->radioButton_2->hide();
        ui->label_6->hide();
        ui->label_4->hide();
        ui->sexWidget->hide();
        ui->wMail->hide();
        ui->label_16->hide();
        ui->lineEdit_5->hide();
        ui->pushButton->setText("新建课程");
        ui->label->setText("班级：");
        ui->label_3->setText("教师编号:");
        ui->label_5->setText("姓名:");
        ui->lineEdit_4->setReadOnly(true);
        int y = ui->label->y();
        ui->label->move(60,y);
        ui->label_2->setText("课程：");
        ui->labmsg->setMaximumHeight(1000);
        ui->labmsg->setText("注：1.如果该课程有多个班一起上，在班级输入框标明即可，"
                            "<br>例如：两个班一起上，可命名为：XXX班-XXX班."
                            "<br>需要导入的学生名单全部放在一个模板文件导入即可."
                            "<br>2.其他信息请到管理员界面完善.");
        //设置课程下拉列表
        Global g;
        g.connectdb();
        QString sql = "select lname from lesson";
        g.sql.exec(sql);
        QStringList word_list;
        for (int i = 0; i <g.sql.size(); ++i)
        {
            g.sql.next();
            word_list << g.sql.value(0).toString();  //添加
        }
        completer = new QCompleter(word_list, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive); //设置不区分大小写
        completer->setFilterMode(Qt::MatchContains);  //设置内容匹配
        completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        ui->lineEdit_2->setCompleter(completer);

        sql = "select DISTINCT cname from lesson";
        g.sql.clear();
        g.sql.exec(sql);
        QStringList word_list1;
        for (int i = 0; i <g.sql.size(); ++i)
        {
            g.sql.next();
            word_list1 << g.sql.value(0).toString();  //添加
        }
        completer1 = new QCompleter(word_list1, this);
        completer1->setCaseSensitivity(Qt::CaseInsensitive); //设置不区分大小写
        completer1->setFilterMode(Qt::MatchContains);  //设置内容匹配
        completer1->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
        ui->lineEdit->setCompleter(completer1);

        connect(ui->lineEdit,SIGNAL(editingFinished()),this,SLOT(check()));
        connect(ui->lineEdit_2,SIGNAL(editingFinished()),this,SLOT(check()));
        connect(ui->lineEdit_3,SIGNAL(editingFinished()),this,SLOT(check()));
        g.disconnectdb();
    }
}


/**
* @brief 初始化修改教师信息界面
* @date 2018/1/31
* @author han
* @param
* @return void
*/
void SignIn::initEditTeacherDialog()
{
    Global g;
    g.connectdb();
    ui->lineEdit->setReadOnly(true);
    ui->lineEdit->setText(g.getTeacherID());
    QString sql = QString("select * from teacher where id ='%1';").arg(g.getTeacherID());
    g.sql.exec(sql);

    if(g.sql.next())  //获取信息
    {
        ui->lineEdit_4->setText(g.sql.value("name").toString());
        QString sex = g.sql.value("sex").toString().trimmed();
        if(sex == "男")
            ui->radioButton->setChecked(true);
        else
            ui->radioButton_2->setChecked(true);
        ui->lineEdit_5->setText(g.sql.value("mail").toString());
    }
}

/**
* @brief lineEdit修改触发
* @date 2018/1/31 修改
* @author han
* @param
* @return void
*/
void SignIn::check()
{
    Global g;
    g.connectdb();
    QImage tick(":/images/images/else/tick.ico");
    QImage wrong(":/images/images/else/wrong.ico");
    QLineEdit *lineEdit = qobject_cast<QLineEdit*>(sender());
    if(kind == "editTeacher")
    {
        if(lineEdit == ui->lineEdit_2)
        {
            if(lineEdit->text() == "")
            {
                ui->label_10->setPixmap(QPixmap::fromImage(wrong));
                ui->label_9->setText("请输入密码");
            }
            else
            {
                ui->label_10->setPixmap(QPixmap::fromImage(tick));
                ui->label_9->clear();
            }
        }
        if(lineEdit == ui->lineEdit_3)
        {
            if(lineEdit->text() == "")
            {
                ui->label_12->setPixmap(QPixmap::fromImage(wrong));
                ui->label_11->setText("请再次输入密码");
                return;
            }
            if(lineEdit->text() != ui->lineEdit_2->text())
            {
                ui->label_12->setPixmap(QPixmap::fromImage(wrong));
                ui->label_11->setText("密码不一致");
            }
            else
            {
                ui->label_12->setPixmap(QPixmap::fromImage(tick));
                ui->label_11->clear();
            }
        }
    }
    if(kind == "zhuce")
    {
        if(lineEdit == ui->lineEdit)
        {
            if(ui->lineEdit->text()=="")
            {
                ui->label_7->setPixmap(QPixmap::fromImage(wrong));
                ui->label_8->setText("教师编号不可以为空");
                return;
            }
            g.sql.exec(QString("select * from teacher where id = %1").arg(ui->lineEdit->text()));
            if(g.sql.next())
            {
                ui->label_7->setPixmap(QPixmap::fromImage(wrong));
                ui->label_8->setText("该编号已存在");
            }
            else
            {
                ui->label_7->setPixmap(QPixmap::fromImage(tick));
                ui->label_8->clear();
            }
        }
        if(lineEdit == ui->lineEdit_2)
        {
            if(lineEdit->text() == "")
            {
                ui->label_10->setPixmap(QPixmap::fromImage(wrong));
                ui->label_9->setText("请输入密码");
            }
            else
            {
                ui->label_10->setPixmap(QPixmap::fromImage(tick));
                ui->label_9->clear();
            }
        }
        if(lineEdit == ui->lineEdit_3)
        {
            if(lineEdit->text() == "")
            {
                ui->label_12->setPixmap(QPixmap::fromImage(wrong));
                ui->label_11->setText("请再次输入密码");
                return;
            }
            if(lineEdit->text() != ui->lineEdit_2->text())
            {
                ui->label_12->setPixmap(QPixmap::fromImage(wrong));
                ui->label_11->setText("密码不一致");
            }
            else
            {
                ui->label_12->setPixmap(QPixmap::fromImage(tick));
                ui->label_11->clear();
            }
        }
    }
    if(kind == "newclass")
    {
        if(lineEdit == ui->lineEdit)
        {
            if(ui->lineEdit->text()=="")
            {
                ui->label_7->setPixmap(QPixmap::fromImage(wrong));
                ui->label_8->setText("班级不可以为空");
                return;
            }
            else
            {
                ui->label_7->setPixmap(QPixmap::fromImage(tick));
                ui->label_8->clear();
            }
        }
        if(lineEdit == ui->lineEdit_2)
        {
            if(ui->lineEdit_2->text()=="")
            {
                ui->label_10->setPixmap(QPixmap::fromImage(wrong));
                ui->label_9->setText("课程不可以为空");
                return;
            }

            g.sql.exec(QString("select cid from lesson where cname='%1' and tid = '%2' and lname = '%3'")
                       .arg(ui->lineEdit->text(),QString::number(g.teacherNum),ui->lineEdit_2->text()));
            if(g.sql.next())
            {
                ui->label_10->setPixmap(QPixmap::fromImage(wrong));
                ui->label_9->setText("已存在此课程");
            }
            else
            {
                ui->label_10->setPixmap(QPixmap::fromImage(tick));
                ui->label_9->clear();
            }
        }
        if(lineEdit == ui->lineEdit_3)
        {
            g.sql.exec(QString("select name from teacher where id = '%1'").arg(ui->lineEdit_3->text()));
            if(!g.sql.next())
            {
                ui->label_12->setPixmap(QPixmap::fromImage(wrong));
                ui->label_11->setText("该教师编号不存在");
                ui->lineEdit_4->clear();
            }
            else
            {
                QString tname = g.sql.value("name").toString();
                ui->lineEdit_4->setText(tname);
                ui->label_12->setPixmap(QPixmap::fromImage(tick));
                ui->label_11->clear();
            }
        }
    }

    if(kind == "findPwd")
    {
        if(lineEdit == ui->lineEdit)
        {
            if(ui->lineEdit->text()=="")
            {
                ui->label_7->setPixmap(QPixmap::fromImage(wrong));
                ui->label_8->setText("登录账户不可以为空");
                return;
            }
            g.sql.exec(QString("select * from teacher where id = '%1'").arg(ui->lineEdit->text()));
            if(!g.sql.next())
            {
                ui->label_7->setPixmap(QPixmap::fromImage(wrong));
                ui->label_8->setText("该账户不存在");
            }
            else
            {
                ui->label_7->setPixmap(QPixmap::fromImage(tick));
                ui->label_8->clear();
            }
        }
        if(lineEdit == ui->lineEdit_2)
        {
            if(lineEdit->text() == "")
            {
                ui->label_10->setPixmap(QPixmap::fromImage(wrong));
                ui->label_9->setText("请输入密码");
            }
            else
            {
                ui->label_10->setPixmap(QPixmap::fromImage(tick));
                ui->label_9->clear();
            }
        }
        if(lineEdit == ui->lineEdit_3)
        {
            if(lineEdit->text() == "")
            {
                ui->label_12->setPixmap(QPixmap::fromImage(wrong));
                ui->label_11->setText("请再次输入密码");
                return;
            }
            if(lineEdit->text() != ui->lineEdit_2->text())
            {
                ui->label_12->setPixmap(QPixmap::fromImage(wrong));
                ui->label_11->setText("密码不一致");
            }
            else
            {
                ui->label_12->setPixmap(QPixmap::fromImage(tick));
                ui->label_11->clear();
            }
        }
    }
}

void SignIn::on_pushButton_clicked()
{
    QImage tick(":/images/images/else/tick.ico");
    QImage wrong(":/images/images/else/wrong.ico");
    Global g;
    g.connectdb();
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss");
    if(kind == "editTeacher")
    {
        if(ui->lineEdit_2->text()=="")
        {
            ui->label_10->setPixmap(QPixmap::fromImage(wrong));
            ui->label_9->setText("请输入密码");
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("修改失败");
            return;
        }
        if(ui->lineEdit_2->text() != ui->lineEdit_3->text())
        {
            ui->label_12->setPixmap(QPixmap::fromImage(wrong));
            ui->label_11->setText("密码不一致");
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("修改失败");
            return;
        }

        Global g;
        g.connectdb();
        bool state;
        //密码加密
        QString strPwdMd5 = g.EncriptPasswordByMD5(ui->lineEdit_3->text());

        if(ui->radioButton->isChecked())
            state = g.sql.exec(QString("update teacher set name='%2', sex='%3',password='%4',mail='%5',lastUpdateTime='%6' "
                                       " where id='%1' ")
                               .arg(ui->lineEdit->text(),ui->lineEdit_4->text(),QString("男"),strPwdMd5,ui->lineEdit_5->text(),time));
        else
            state = g.sql.exec(QString("update teacher set name='%2', sex='%3',password='%4',mail='%5',lastUpdateTime='%6' "
                                       " where id='%1' ")
                               .arg(ui->lineEdit->text(),ui->lineEdit_4->text(),QString("女"),strPwdMd5,ui->lineEdit_5->text(),time));
        if(state)
        {
            ui->label_14->setPixmap(QPixmap::fromImage(tick));
            ui->label_13->setText("修改成功");
            emit newItem(ui->lineEdit->text(),ui->lineEdit_4->text());
        }
        else
        {
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("修改失败");
        }
    }
    //添加教师
    else if(kind =="zhuce")
    {
        if(ui->lineEdit->text()=="")
        {
            ui->label_7->setPixmap(QPixmap::fromImage(wrong));
            ui->label_8->setText("教师编号不可以为空");
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("添加失败");
            return;
        }
        if(ui->lineEdit_2->text()=="")
        {
            ui->label_10->setPixmap(QPixmap::fromImage(wrong));
            ui->label_9->setText("请输入密码");
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("添加失败");
            return;
        }
        if(ui->lineEdit_2->text() != ui->lineEdit_3->text())
        {
            ui->label_12->setPixmap(QPixmap::fromImage(wrong));
            ui->label_11->setText("密码不一致");
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("添加失败");
            return;
        }
        Global g;
        g.connectdb();
        bool state;
        //密码加密
        QString strPwdMd5 = g.EncriptPasswordByMD5(ui->lineEdit_3->text());

        if(ui->radioButton->isChecked())
            state = g.sql.exec(QString("insert into teacher(id,name,sex,password,mail,createTime) "
                                       " values(%1,'%2','%3','%4','%5','%6')")
                               .arg(ui->lineEdit->text(),ui->lineEdit_4->text(),QString("男"),strPwdMd5,ui->lineEdit_5->text(),time));
        else
            state = g.sql.exec(QString("insert into teacher(id,name,sex,password,mail,createTime) "
                                       " values(%1,'%2','%3','%4','%5','%6')")
                               .arg(ui->lineEdit->text(),ui->lineEdit_4->text(),"女",strPwdMd5,ui->lineEdit_5->text(),time));
        if(state)
        {
            ui->label_14->setPixmap(QPixmap::fromImage(tick));
            ui->label_13->setText("添加成功");
            emit newItem(ui->lineEdit->text(),ui->lineEdit_4->text());
        }
        else
        {
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("添加失败");
        }
    }

    //新建课程
    else if(kind == "newclass")
    {
        //班级检查
        if(ui->lineEdit->text() == "")
        {
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("班级名不能为空");
            return;
        }
        //课程检查
        if(ui->lineEdit_2->text() == "")
        {
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("课程名不能为空");
            return;
        }
        g.db.transaction(); //开启事务
        //判断课程是否存在
        g.sql.exec(QString("select cid from lesson where cname='%1' and lname = '%2'")
                   .arg(ui->lineEdit->text(),ui->lineEdit_2->text()));
        if(g.sql.next())
        {
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("已存在此课程");
            return;
        }

        //教师编号检查
        if(!ui->lineEdit_3->text().trimmed().isEmpty())
        {
            g.sql.exec(QString("select name from teacher where id = '%1'").arg(ui->lineEdit_3->text()));
            if(!g.sql.next())
            {
                ui->label_12->setPixmap(QPixmap::fromImage(wrong));
                ui->label_11->setText("该教师编号不存在");
                ui->lineEdit_4->clear();
                return;
            }
            else
            {
                QString tname = g.sql.value("name").toString();
                ui->lineEdit_4->setText(tname);
                ui->label_12->setPixmap(QPixmap::fromImage(tick));
                ui->label_11->clear();
            }
        }
        else
        {
            ui->label_12->setPixmap(QPixmap::fromImage(wrong));
            ui->label_11->setText("教师编号不能为空");
        }

        g.sql.clear();
        QString Id = "";
        QString teacherID = ui->lineEdit_3->text().trimmed();

        //查找最大的cid
        g.sql.exec("select max(cid) from lesson");
        if(g.sql.next())
        {
            int newId = g.sql.value(0).toInt() + 1;
            Id = QString::number(newId,10);
        }
        else  //第一个表
        {
            Id = "0";
        }
        QString classId = "class" + Id;
        g.sql.clear();
        QString sql = QString("create table %1(id bigint primary key ,"
                              "name varchar(30) not null default '0',"
                              "sex varchar(3) not null default '0',"
                              "time int not null default 0,"
                              "absence int not null default 0,"
                              "classname varchar(30) not null default '0',"
                              "portrait varchar(50))").arg(classId);
        bool state = g.sql.exec(sql);  //执行建表
        if(!state)
        {
            g.db.rollback();  // 回滚事务
            g.sql.exec("drop table " + classId);
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("新建表失败！请检查数据库或者错误日志.");
            g.writeLog("work",QString("新建课程失败,原因：%1").arg(g.sql.lastError().text()));

            return;
        }

        //插入课程记录
        sql = QString("insert into lesson(cid,cname,tid,lname) values('%1','%2','%3','%4')")
                .arg(Id,ui->lineEdit->text(),teacherID,ui->lineEdit_2->text());
        g.sql.clear();
        qDebug()<<sql;
        state = g.sql.exec(sql);
        if(!state)
        {  //插入记录失败
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("创建失败，请检查数据库或者错误日志.");
            g.writeLog("work",QString("教师%1新建课程失败,原因：%2").arg(g.getTeacherID(),g.sql.lastError().text()));
            g.db.rollback();  // 回滚事务
            g.sql.exec("drop table " + classId);
            return;
        }
        else
        {
            emit newItem(ui->lineEdit->text(),ui->lineEdit_2->text(),Id);
            ui->label_14->setPixmap(QPixmap::fromImage(tick));
            ui->label_13->setText("创建成功");
            g.writeLog("work",QString("新建课程成功"));
        }

        g.db.commit();  //提交事务
    }

    //找回密码
    else if(kind =="findPwd")
    {
        if(ui->lineEdit->text()=="")
        {
            ui->label_7->setPixmap(QPixmap::fromImage(wrong));
            ui->label_8->setText("登录账户不可以为空");
            return;
        }
        if(ui->lineEdit_2->text()=="")
        {
            ui->label_10->setPixmap(QPixmap::fromImage(wrong));
            ui->label_9->setText("请输入密码");
            return;
        }
        if(ui->lineEdit_2->text() != ui->lineEdit_3->text())
        {
            ui->label_12->setPixmap(QPixmap::fromImage(wrong));
            ui->label_11->setText("密码不一致");
            return;
        }
        //判断邮箱
        if(ui->lineEdit_5->text().trimmed().isEmpty())
        {
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("邮箱不能为空.");
            return;
        }

        g.sql.exec(QString("select mail from teacher where id = %1").arg(ui->lineEdit->text()));
        if(!g.sql.next())
        {
            ui->label_7->setPixmap(QPixmap::fromImage(wrong));
            ui->label_8->setText("该账户不存在");
            return;
        }
        else
        {
            //判断邮箱是否正确
            if(ui->lineEdit_5->text().trimmed() == g.sql.value(0).toString())
            {
                ui->label_7->setPixmap(QPixmap::fromImage(tick));
                ui->label_8->clear();
            }
            else
            {
                ui->label_14->setPixmap(QPixmap::fromImage(wrong));
                ui->label_13->setText("邮箱不正确.");
                return;
            }
        }
        Global g;
        g.connectdb();
        bool state;
        //密码加密
        QString strPwdMd5 = g.EncriptPasswordByMD5(ui->lineEdit_3->text());

        if(ui->radioButton->isChecked())
            state = g.sql.exec(QString("update teacher set name = '%1',sex = '%2',password = '%3',mail = '%4' where id = '%5'")
                               .arg(ui->lineEdit_4->text(),"男",strPwdMd5,ui->lineEdit_5->text(),ui->lineEdit->text()));
        else
            state = g.sql.exec(QString("update teacher set name = '%1',sex = '%2',password = '%3',mail = '%4' where id = '%5'")
                               .arg(ui->lineEdit_4->text(),"女",strPwdMd5,ui->lineEdit_5->text(),ui->lineEdit->text()));
        if(state)
        {
            ui->label_14->setPixmap(QPixmap::fromImage(tick));
            ui->label_13->setText("修改成功");
            emit newItem(ui->lineEdit->text(),ui->lineEdit_4->text());
        }
        else
        {
            ui->label_14->setPixmap(QPixmap::fromImage(wrong));
            ui->label_13->setText("修改失败");
        }
    }
}


void SignIn::closeEvent(QCloseEvent*)
{
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
    ui->label_7->clear();
    ui->label_8->clear();
    ui->label_9->clear();
    ui->label_10->clear();
    ui->label_11->clear();
    ui->label_12->clear();
    ui->label_13->clear();
    ui->label_14->clear();
}


SignIn::~SignIn()
{
    delete ui;
}

/**
* @brief 返回
* @date 2017/12/9
* @author han
* @return void
*/
void SignIn::on_returnBtn_clicked()
{
    reject();
}

void SignIn::on_lineEdit_2_selectionChanged()
{
}
