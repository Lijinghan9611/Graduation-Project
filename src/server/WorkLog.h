#ifndef WORKLOG_H
#define WORKLOG_H

#include <QWidget>
#include <QCloseEvent>
//#include <QListWidget>
#include <QCompleter>
#include <QEventLoop>
#include <QButtonGroup>
namespace Ui {
class WorkLog;
}

class WorkLog : public QWidget
{
    Q_OBJECT

public:
    explicit WorkLog(QWidget *parent = 0);
    void paintEvent(QPaintEvent*);
    void closeEvent(QCloseEvent*);
    void setReturnBtnEnable(bool f);
    void initWorkLogForType3Login(bool flag);
    bool eventFilter(QObject *obj, QEvent *event);
    QString purpose; //登录类型
    bool isAlter; //是否为课堂管理界面修改日志
    ~WorkLog();
public slots:
    void onClick();
    void onLogin();
    void exec();
    void onSignEnd(const QString&,const QString&,const QString&,const QString&);
private slots:
    void on_ButtonGroupClick(const int &);

    void on_leClassName_editingFinished();

    void on_leClassName_returnPressed();

    void on_returnBtn_clicked();

    void on_leClassName_selectionChanged();


    void on_leLessonName_selectionChanged();

    void on_pushButton_clicked();

    void on_classTimeStartCbx_currentIndexChanged(int index);

    void on_classTimeEndCbx_currentIndexChanged(int index);

    void on_classRadio4_toggled(bool checked);

private:
    Ui::WorkLog *ui;
    QCompleter *completer;
    QStringList word_list;
    QCompleter *completerForCourseName;
    QStringList word_listForCourseName;
    QCompleter *completerForClass;
    QStringList word_listForClass;
    QEventLoop *loop;
    QButtonGroup *classButtonGroup;
    void initTeacherList();
    void updateLessionDrop();
    void updateClassDrop();
    void getCourseInfo();
    void classSeletedChange(QString className);
    void initCourseInfo(QString lesson, QString lesson1, QString lesson2, QString teacherName, QDateTime time);  //打开页面触发

   // QListWidget *m_ListWidget ;
};

#endif // WORKLOG_H
