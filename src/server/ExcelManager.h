#ifndef EXCELMANAGER_H
#define EXCELMANAGER_H


/**
* @brief excel文件处理
* @date 2018/1/1
* @author han
*/
#include <QtXlsx>
#include "mysqltablemodel.h"
#include <QFile>
#include <QTextStream>
#include <QSqlRecord>
#include <QProgressBar>
class ExcelManager
{
private:
    //对其方式
    QXlsx::Format rAlign;
    QXlsx::Format lAlign;
    QXlsx::Format cHAlign;

    //单元格格式
    QXlsx::Format green;
    QXlsx::Format red;
    QXlsx::Format myColor;
    QXlsx::Format black;
    QXlsx::Format title;
public:
    ExcelManager();
    QString ReadStuInfoExcelFile(QString fileName, QString table, int *totalRow,int * success,QProgressBar *bar = NULL);
    QString ImportTeacherInfo(QString fileName, int *totalRow,int * success,QProgressBar *bar = NULL);
    QString ImportLessonInfo(QString fileName, int *totalRow,int * success,QProgressBar *bar = NULL);

    const int Attendance = 1;
    const int TotalAttendance = 2;

    bool ExportToExcel(const MySqlTableModel *DataModel, const QString fileName,QProgressBar *bar = NULL);
    bool ExportToExcel(const QStringList titleList,const MySqlTableModel *DataModel, const QString fileName,QProgressBar *bar = NULL);
};

#endif // EXCELMANAGER_H
