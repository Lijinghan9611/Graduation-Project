#ifndef CLASSMANAGE_H
#define CLASSMANAGE_H

#include "Global.h"
#include "ComboboxItem.h"
#include "CommonTitleBar.h"
#include "MyMessageBox.h"
#include "SignIn.h"
#include "ComparePortrait.h"
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPaintEvent>
#include <QTableView>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QTimer>
#include "mysqltablemodel.h"
#include "ftpmanager.h"
#include <QFile>

namespace Ui {
class CLassManage;
}

class CLassManage : public QWidget
{
    Q_OBJECT

public:
    explicit CLassManage(QWidget *parent = 0);
    ~CLassManage();
    void sqlShow(int);
    void paintEvent(QPaintEvent*);
    void closeEvent(QCloseEvent*);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent*event);
    void mouseReleaseEvent(QMouseEvent*event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void cheakAnswer();
    QString ipToPosition(const QString&);
    int getCurrentAttendanceTime();  //获取当前考勤次数
    void initLesson();
    void setSignTimerStart();
    void setSignTimerStop();

private:
    Ui::CLassManage *ui;

    MySqlTableModel * model;
    MySqlTableModel * AttendanceModel;
    QList<QStringList>m_class;  //the whole class list
    Global g;
    int classId;
    bool signOrEndSign;
    int number;                 //student count
    int row ;                   //tableWidget row number
    bool signOrQuestion;  //indetity sign or question
    int rightCount;
    QPoint light;    //the highlight qurey item
    QString rightAnswer;
    QString className , lessonName; //int worklog
    QTimer *signTimer;//auto sign
    QTimer * checkSignStatusTimer;
    bool signSuccess,allStudentPresent;
    QMap<QString,QString>m_map; //use for portrait(ip-id)
    int portraitIndex;
    ComparePortrait * comparePortraitWidget;
    int globalAttendanceTime;  //考勤间隔
    int timeOutTime;  //点名超时次数
    bool ispress;

    QString FtpServerIP;
    QString FtpNumber;
    QString FtpPwd;
    bool sendAuto;
    QNetworkReply *reply;
    bool isFtpTest;
    bool SaveTogether;
    QString saveTogetherPath; //ftp一起存储时的路径
    QString localSavePath;  //本地考勤记录保存路径
    QTimer * saveAttendanceAutoTimer; //自动保存timer
    bool isLastAttendanceSave;  //上一次考勤记录是否以保存

    bool initDropBool;
    bool noShowDetail;   //是否显示查询结果
    QPixmap unSolved;
    QPixmap solved;

    //右键菜单
    QMenu *popMenu;
    QAction *copyAction;
    QAction *deleteAction;
    QAction *refreshAction;
    QAction *exportAction;
    QAction *pasteAction;
    QAction *addAction;

    //文件发送完毕后是否显示弹窗
    bool unShowInfo;

    QTimer *connectFtpTimer;
    int timeOutTimes;

    QList<int> deleteRow;  //保存已删除但未确认的考勤记录行号
    QList<int> deleteInfoRow;  //保存已删除但未确认的统计记录行号

    void setEditButtonEnable(bool);  //设置操作学生信息表格的Button是否可以操作
    void insertAllCallNameInfo();  //插入全部的点名记录
    void searchSeletedAttendance();
    void chenkIsEmpty();
    void initDrop();
    void getAdviceHistory();
    QString convertToString(QString sourceStr);

public slots:
    void lineEditTip(int,int);
    void onTableWidgetSort(int);
    void onSignMg(const QString&,const QString&,const QString&); //deal sign message
    void onAnswerMg(const QString&,const QString&,const QString&,const QString&);

    void onLogOut();
    void onSignTime();//auto sign
    void onCheckSign();
    void onIniPortrait(const QString&,const QImage&);//portrait
    void onComparePortrait(const QString&,const QImage&);

signals:
    void beginSign();
    void retrieveSign();
    void signMgToolTip(const QString&,const QString&,const QString&);  //set tool tip of user message

    void question(const QString&); //A&Q
    void getAnswer();

    void signEnd(const QString &, const QString &, const QString &, const QString &);//ini worklog
    void editWorklog();  //在Login处理

    void importPortrait();  //portrait
    void beginComparePortrait(int);
    void comparePortrait(const QString&,const QImage &);
private slots:
    void onTimeOut();
    void rightClickedOperation();
    void on_tabWidget_currentChanged(int index);
    void on_sendAnswerBtn_clicked();
    void on_callNameBtn_clicked();
    void on_exportStudentInfoBtn_clicked();
    void on_importBtn_clicked();
    void on_deleteSeletedBtn_clicked();
    void on_CancelBtn_clicked();
    void on_saveBtn_clicked();
    void on_pushButton_clicked();
    void on_downloadBtn_clicked();
    void on_addRowBtn_clicked();
    void modelDataChanged(QModelIndex ,QModelIndex);
    void attendanceModelDataChanged(QModelIndex ,QModelIndex);
    //   modelDataChanged(QModelIndex &topLeft,QModelIndex &buttomRight);
    void on_tab3searchAttendanceBtn_clicked();
    void on_cbxLessonAndClass_currentIndexChanged(const QString &arg1);
    void on_cbxWeek_currentIndexChanged(const QString &arg1);
    void on_cbxTime_currentIndexChanged(const QString);
    void on_cbxZhou_currentIndexChanged(const QString &arg1);
    void on_tab3addAttendanceBtn_clicked();
    void on_tab3saveAttendanceBtn_clicked();
    void on_tab3deleteAttendanceBtn_clicked();
    void on_cancelEditAtBtn_clicked();
    void on_tab3downloadAttendanceBtn_clicked();
    void on_showWorkLogBtn_clicked();
    void on_pushButton_2_clicked();
    void on_minBtn_clicked();
    void on_maxBtn_clicked();
    void on_closeBtn_clicked();

    void on_pushButton_3_clicked();

    void OnFinishRelay();
    void on_selectFtpFileBtn_clicked();
    void on_uploadFtpFileBtn_clicked();


    void OnUploadProgress(qint64 bytesSent,qint64 bytesTotal);

    void saveAttendanceAuto(bool isTimeOut);  //自动保存考勤记录

    void on_sendToFtpBtn_clicked();
    void on_pushButton_5_clicked();
    void on_resetReBtn_clicked();
    void on_resetAdBtn_clicked();
    void on_confirmAdBtn_clicked();
    void on_confirmReBtn_clicked();
    void on_tbViewAttendance_customContextMenuRequested(const QPoint);
    void on_tableView_customContextMenuRequested(const QPoint);
    void on_AdviceList_customContextMenuRequested(const QPoint);
    void on_tableWidget_customContextMenuRequested(const QPoint);
    void on_radioSendAuto_clicked(bool checked);
    void on_radioSendManual_toggled(bool checked);

    void onSaveAttendanceAutoTimerTimeOut();

};

#endif // CLASSMANAGE_H
