#include "excelmanager.h"
#include "Global.h"
ExcelManager::ExcelManager()
{
    //对其方式
    rAlign.setHorizontalAlignment(QXlsx::Format::AlignRight);
    lAlign.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    cHAlign.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    //单元格格式
    green.setFontColor(Qt::green);
    red.setFontColor(Qt::red);
    myColor.setFontColor(QColor(147,112,219));
    black.setFontColor(Qt::black);
    title.setFontBold(true);
    title.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
}


/**
* @brief 导出考勤记录excel文件
* @date 2018/1/5
* @author han
* @return void
*/
bool ExcelManager::ExportToExcel(const MySqlTableModel *DataModel,const QString fileName, QProgressBar *bar)
{
    QXlsx::Document xlsx(fileName);
    QXlsx::Worksheet *sheet = xlsx.currentWorksheet();  //获取当前工作表
    //设置宽度
    xlsx.setColumnWidth(1, 1, 20);
    xlsx.setColumnWidth(3, 5, 20);
    //写入标题
    sheet->write(1, 1, "日期",title);
    sheet->write(1, 2, "周次",title);
    sheet->write(1, 3, "课程",title);
    sheet->write(1, 4, "班别",title);
    sheet->write(1, 5, "学号",title);
    sheet->write(1, 6, "姓名",title);
    sheet->write(1, 7, "第n次点名",title);
    sheet->write(1, 8, "结果",title);
    sheet->write(1, 9, "机位号",title);

    if(bar != NULL)
    {
        bar->setMaximum(DataModel->rowCount());  //设置进度条最大值
        bar->setValue(0);
    }
    //导出
    for(int i = 0;i < DataModel->rowCount();i++)
    {
        QString lessonDate = DataModel->record(i).value(1).toString();
        QString week = DataModel->record(i).value(2).toString();
        QString lname = DataModel->record(i).value(3).toString();
        QString cname = DataModel->record(i).value(4).toString();
        QString id = DataModel->record(i).value(5).toString();
        QString sname = DataModel->record(i).value(6).toString();
        QString aTime = DataModel->record(i).value(7).toString();
        QString result = DataModel->record(i).value(8).toString();
        QString position = DataModel->record(i).value(10).toString();

        QXlsx::RichString richResult;
        if("已签到" == result.trimmed())
            richResult.addFragment(result,green);
        else if("缺勤" == result.trimmed())
            richResult.addFragment(result,red);
        else if("早退" == result.trimmed())
            richResult.addFragment(result,myColor);
        else
            richResult.addFragment(result,black);

        sheet->write(i+2, 1, lessonDate,cHAlign);
        sheet->write(i+2, 2, week,cHAlign);
        sheet->write(i+2, 3, lname,cHAlign);
        sheet->write(i+2, 4, cname,cHAlign);
        sheet->write(i+2, 5, id,cHAlign);
        sheet->write(i+2, 6, sname,cHAlign);
        sheet->write(i+2, 7, aTime,cHAlign);
        sheet->write(i+2, 8, richResult,cHAlign);
        sheet->write(i+2, 9, position,cHAlign);
        if(bar != NULL)
        {
            bar->setValue(i);
        }
    }
    if(bar != NULL)
    {
        bar->setValue(bar->maximum()); //进度条设置为最大值
        bar->hide();
        bar->setValue(0);
    }
    xlsx.save();  //保存

    return true;
}


/**
* @brief 导出信息到excel,通用
* @date 2018/1/5
* @author han
* @return void
*/
bool ExcelManager::ExportToExcel(const QStringList titleList,const MySqlTableModel *DataModel, const QString fileName, QProgressBar *bar)
{
    int colum = titleList.size();
    if(colum > DataModel->columnCount())
        return false;
    QXlsx::Document xlsx(fileName);
    QXlsx::Worksheet *sheet = xlsx.currentWorksheet();  //获取当前工作表
    //设置宽度
    xlsx.setColumnWidth(1, colum, 20);
    if(bar != NULL)
    {
        bar->setMaximum(DataModel->rowCount());  //设置进度条最大值
        bar->setValue(0);
    }

    //写入标题
    for(int i = 0;i < titleList.size();i++)
        sheet->write(1, i + 1, titleList.at(i),title);

    //导出
    for(int row = 0;row < DataModel->rowCount();row++)  //行
    {
        for(int col = 0;col < colum; col++) //列
        {
            QString data = DataModel->record(row).value(col).toString();
            sheet->write(row+2, col+1, data,cHAlign);
        }
        if(bar != NULL)
        {
            bar->setValue(row);
        }
    }
    if(bar != NULL)
    {
        bar->setValue(bar->maximum()); //进度条设置为最大值
        bar->hide();
        bar->setValue(0);
    }
    xlsx.save();  //保存

    return true;
}

/**
* @brief 读取学生信息excel文件,并保存到数据库
* @date 2018/1/1
* @author han
* @return 结果
*/
QString ExcelManager::ReadStuInfoExcelFile(QString fileName, QString table, int *totalRow, int *success, QProgressBar *bar)
{

    QXlsx::Document xlsx(fileName);
    QXlsx::CellRange range;
    range = xlsx.dimension();
    int sheet_row = range.rowCount();//行
    int sheet_col = range.columnCount();

    *totalRow = sheet_row;

    if(bar != NULL)
    {
        bar->setMaximum(*totalRow);  //设置进度条最大值
        bar->setValue(0);
    }

    if(sheet_col == 0)
        return "未获取到行，如果是xls格式的excel文件请先转换为xlsx格式.";
    if(sheet_col != 4)
        return "Excel文件列数不正确，请按照模板填写学生信息";
    int successCount = 0;
    Global g;
    g.connectdb();
    QString error = "导入失败的学号为：";
    bool hadFail = false;
    for (int row=1; row<=sheet_row; ++row)
    {
        QString studentID;    //学号
        QString studentName;  //姓名
        QString sex;          //性别
        QString className;    //班级
        if (QXlsx::Cell *cell=xlsx.cellAt(row, 1))
            studentID = cell->value().toString();
        if (QXlsx::Cell *cell=xlsx.cellAt(row, 2))
            studentName = cell->value().toString();
        if (QXlsx::Cell *cell=xlsx.cellAt(row, 3))
            sex = cell->value().toString();
        if (QXlsx::Cell *cell=xlsx.cellAt(row, 4))
            className = cell->value().toString();

        if(studentID.contains("学号")) //跳过标题
        {
            *totalRow = *totalRow - 1;
            continue;
        }
        if(!studentID.isEmpty() && !studentName.isEmpty())  //数据有效
        {
            QString sql = QString("insert into %1(id,name,sex,classname) values(%2,'%3','%4','%5')")
                    .arg(table,studentID,studentName,sex,className);
            qDebug() <<sql;
            if(g.sql.exec(sql))
                successCount ++;
            else
            {
                hadFail = true;
                error += studentID +" ";
            }
        }
        if(bar != NULL)
        {
            bar->setValue(row);
        }
    }
    if(bar != NULL)
    {
        bar->setValue(bar->maximum()); //进度条设置为最大值
        bar->hide();
        bar->setValue(0);
    }
    *success = successCount;
    if(hadFail)
        return error;
    else
        return "";
}


/**
* @brief 导入教师信息
* @date 2018/1/10
* @author han
* @return void
*/
QString ExcelManager::ImportTeacherInfo(QString fileName, int *totalRow, int * success, QProgressBar *bar)
{
    QXlsx::Document xlsx(fileName);
    QXlsx::CellRange range;

    range = xlsx.dimension();
    int sheet_row = range.rowCount();//行
    int sheet_col = range.columnCount();

    *totalRow = sheet_row;
    if(bar != NULL)
    {
        bar->setMaximum(*totalRow);  //设置进度条最大值
        bar->setValue(0);
    }

    if(sheet_col == 0)
        return "未获取到行，如果是xls格式的excel文件请先转换为xlsx格式.";
    if(sheet_col != 2)
        return "Excel文件列数不正确，请按照'教师编号''姓名'的形式填写信息";
    int successCount = 0;
    Global g;
    g.connectdb();
    QString error = "导入失败的教师编号为：";
    bool hadFail = false;


    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh-mm-ss");

    //默认密码加密,MD5
    QString strPwdMd5 = g.EncriptPasswordByMD5("123456");

    for (int row=1; row<=sheet_row; ++row)
    {
        QString ID;    //编号
        QString Name;  //姓名
        if (QXlsx::Cell *cell=xlsx.cellAt(row, 1))
            ID = cell->value().toString();
        if (QXlsx::Cell *cell=xlsx.cellAt(row, 2))
            Name = cell->value().toString();

        if(ID.contains("教师编号") || Name.contains("姓名")) //跳过标题
        {
            *totalRow = *totalRow - 1;
            continue;
        }
        if(!ID.isEmpty() && !Name.isEmpty())  //数据有效
        {
            QString sql = QString("insert into teacher(id,name,password,mail,createTime) values('%1','%2','%3','sample@sample.com','%4')")
                    .arg(ID,Name,strPwdMd5,time);

            if(g.sql.exec(sql))
                successCount ++;
            else
            {
                hadFail = true;
                error += ID +" ";
            }
        }
        if(bar != NULL)
        {
            bar->setValue(row);
        }
    }
    if(bar != NULL)
    {
        bar->setValue(bar->maximum()); //进度条设置为最大值
        bar->hide();
        bar->setValue(0);
    }
    *success = successCount;
    if(hadFail)
        return error;
    else
        return "";
}


/**
* @brief 导入课程信息
* @date 2018/1/12
* @author han
* @param
* @return void
*/
QString ExcelManager::ImportLessonInfo(QString fileName, int *totalRow,int * success,QProgressBar *bar)
{
    QXlsx::Document xlsx(fileName);
    QXlsx::CellRange range;

    range = xlsx.dimension();
    int sheet_row = range.rowCount();//行
    int sheet_col = range.columnCount();

    *totalRow = sheet_row - 1;

    if(bar != NULL)
    {
        bar->setMaximum(*totalRow);  //设置进度条最大值
        bar->setValue(0);
    }

    if(sheet_col == 0)
        return "未获取到行，如果是xls格式的excel文件请先转换为xlsx格式.";
    if(sheet_col < 3)
        return "Excel文件列数小于3，请按照'教师名称''班别''授课数量''课程1''课程2'...的形式填写信息，授课数量和后面课程的列数必须准确.";
    int successCount = 0;
    Global g;
    g.connectdb();
    QString error = "导入失败的课程为：";
    bool hadFail = false;

    int count = 0;
    for (int row=2; row<=sheet_row; ++row)
    {
        QString Name;  //姓名
        QString cname;  //班别
        int LessonCount = 0;//数量
        QString lname;//课程名
        if (QXlsx::Cell *cell=xlsx.cellAt(row, 1))
            Name = cell->value().toString();

        if (QXlsx::Cell *cell=xlsx.cellAt(row, 2))
            cname = cell->value().toString();
        if (QXlsx::Cell *cell=xlsx.cellAt(row, 3))
            LessonCount = cell->value().toInt();
        else
            LessonCount = 0;
        if(Name.contains("教师") || Name.contains("姓名")) //跳过标题
        {
            *totalRow = *totalRow - 1;
            continue;
        }
        if(LessonCount == 0)  //教师的授课数目为0
        {
            *totalRow = *totalRow - 1;
            continue;
        }
        if(!Name.isEmpty() && !Name.isEmpty())  //数据有效
        {
            for(int i = 1;i <= LessonCount ;i++)
            {
                if (QXlsx::Cell *cell=xlsx.cellAt(row, 3+i))
                    lname = cell->value().toString();

                if(!lname.trimmed().isEmpty())  //课程名不为空
                {
                    qDebug()<< Name << lname<<cname;
                    QString teacherID = "";
                    g.db.transaction(); //开启事务
                    //判断课程是否存在
                    g.sql.clear();
                    QString sql1 = QString(" select teacher.id from lesson LEFT JOIN teacher on lesson.tid = teacher.id "
                                           " where teacher.name='%1' and lname = '%2' and cname='%3'")
                                   .arg(Name,lname,cname);
                    g.sql.exec(sql1);


                    if(g.sql.next())
                    {
                        hadFail = true;
                        error += QString("<br>%1 %2 %3：课程已存在").arg(Name,lname,cname);
                        count++;
                        if(bar != NULL)
                            bar->setValue(count); //设置滚动条
                        continue;
                    }

                    //教师检查
                    g.sql.clear();
                    QString sql2 = QString("select id from teacher where name = '%1'").arg(Name);
                    g.sql.exec(sql2);

                    if(!g.sql.next())
                    {
                        hadFail = true;
                        error += QString("<br>%1 %2 %3：教师不存在，请先添加该教师.").arg(Name,lname,cname);
                        count++;
                        if(bar != NULL)
                            bar->setValue(count); //设置滚动条
                        continue;
                    }
                    teacherID = g.sql.value(0).toString();

                    g.sql.clear();

                    QString Id = "";//课程id

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
                        g.writeLog("work",QString("新建课程失败,原因：%1").arg(g.sql.lastError().text()));

                        hadFail = true;
                        error += QString("<br>%1 %2 %3：创建表失败，详情请查看日志.").arg(Name,lname,cname);
                        count++;
                        if(bar != NULL)
                            bar->setValue(count); //设置滚动条
                        continue;
                    }


                    //插入课程记录
                    sql = QString("insert into lesson(cid,cname,tid,lname,createTime) values('%1','%2','%3','%4',now())")
                            .arg(Id,cname,teacherID,lname);
                    g.sql.clear();
                    state = g.sql.exec(sql);
                    if(!state)
                    {  //插入记录失败
                        g.writeLog("work",QString("教师%1新建课程失败,原因：%2").arg(g.getTeacherID(),g.sql.lastError().text()));
                        g.db.rollback();  // 回滚事务
                        g.sql.exec("drop table " + classId);

                        hadFail = true;
                        error += QString("<br>%1 %2 %3：创建失败，详情请查看日志.").arg(Name,lname,cname);
                        count++;
                        if(bar != NULL)
                            bar->setValue(count); //设置滚动条
                        continue;
                    }

                    //插入班级信息
                    sql = QString("insert into classinfo(classname,major,tablename) values('%1','','%2')")
                            .arg(cname,"class"+Id);

                    g.sql.clear();
                    g.sql.exec(sql);

                    g.db.commit();  //提交事务
                }
                successCount ++;
                count++;
                if(bar != NULL)
                    bar->setValue(count); //设置滚动条
            }
        }
    }

    if(bar != NULL)
    {
        bar->setValue(bar->maximum()); //进度条设置为最大值
        bar->hide();
        bar->setValue(0);
    }
    *success = successCount;
    if(hadFail)
        return error;
    else
        return "";
}
