#ifndef ADMINOPTION_H
#define ADMINOPTION_H

#include <QWidget>
#include <QCloseEvent>
#include "Global.h"
#include "SignIn.h"
#include "mysqltablemodel.h"
#include <QHeaderView>
#include <QCheckBox>
#include <QDesktopServices>
#include <QDir>
#include "ftpmanager.h"
#include <QTimer>
#include <QMenu>
#include "MyButton.h"
#include <QHBoxLayout>
#include <QFont>

namespace Ui {
class AdminOption;
}

class AdInfo
{
public:
    int index;
    int id;
    int isSolved;
};

class AdminOption : public QWidget
{
    Q_OBJECT

public:
    explicit AdminOption(QWidget *parent = 0);
    ~AdminOption();
    void closeEvent(QCloseEvent*);
    void paintEvent(QPaintEvent*ev);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent*event);
    void mouseReleaseEvent(QMouseEvent*event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    void setAdminPwd(QString);
    MySqlTableModel *model;
    Global g ;
    void exportPartData(QTableWidget *table);

signals:
    void logoutFromAdmin();
public slots:
    void onTableWidgetSort(int);

private slots:
    void onModelDataChanged(QModelIndex index, QModelIndex);
    void on_labUsedBtn_clicked();

    void on_searchMajorBtn_clicked();


    void on_clearAllBtn_clicked();

    void on_searchTeacherBtn_clicked();

    void on_deleteTeaBtn_clicked();

    void on_cancelTeaBtn_clicked();

    void on_saveTeaBtn_clicked();

    void on_searchClientBtn_clicked();

    void on_removeAllClientDataBtn_clicked();

    void on_searchClientDetailBtn_clicked();

    void on_saveSysIniBtn_clicked();

    void on_minBtn_clicked();

    void on_maxBtn_clicked();

    void on_closeBtn_clicked();

    void on_pushButton_clicked();

    void on_AddTeacher(QString id,QString name);

    void on_exportTeacherBtn_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_importTeacherBtn_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_16_clicked();

    void on_pushButton_17_clicked();

    void on_lelLabPage_returnPressed();



    void on_leTeacherPage_returnPressed();

    void on_leClientPage_returnPressed();

    void on_pushButton_18_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();
    void on_lessonListWidget_clicked(const QModelIndex &index);

    void on_pushButton_27_clicked();

    void on_importStudBtn_clicked();

    void on_pushButton_25_clicked();

    void on_pushButton_26_clicked();

    void on_saveLessonEditBtn_clicked();

    void on_pushButton_23_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_24_clicked();

    void on_pushButton_22_clicked();

    void on_lessonListWidget_itemSelectionChanged();

    void on_pushButton_28_clicked();

    void on_pushButton_29_clicked();

    void on_tableView_clicked(const QModelIndex &);

    void on_pushButton_30_clicked();

    void on_pushButton_31_clicked();

    void on_pushButton_32_clicked();

    void on_listWidgetOrder_clicked(const QModelIndex &index);

    void on_listWidgetOrder_itemSelectionChanged();

    void on_listWidgetType_itemSelectionChanged();

    void on_listWidgetRequire_itemSelectionChanged();

    void on_listWidgetCategory_itemSelectionChanged();

    void on_listWidgetType_clicked(const QModelIndex &index);

    void on_listWidgetRequire_clicked(const QModelIndex &index);

    void on_listWidgetCategory_clicked(const QModelIndex &index);

    void on_pushButton_34_clicked();

    void on_pushButton_36_clicked();

    void on_pushButton_38_clicked();

    void on_pushButton_40_clicked();

    void on_pushButton_41_clicked();

    void on_pushButton_42_clicked();

    void on_pushButton_43_clicked();

    void on_pushButton_33_clicked();

    void on_pushButton_35_clicked();

    void on_pushButton_37_clicked();

    void on_pushButton_39_clicked();

    void on_FtpTestBtn_clicked();

    void OnFinishRelay();

    void onTimeOut();

    void on_FtpSaveBtn_clicked();

    void on_cbxAdviceType_currentIndexChanged(int);

    void on_listWidgetAdvice_clicked(const QModelIndex &index);

    void on_pushButton_44_clicked();

    void on_pushButton_45_clicked();

    void on_listWidgetAdvice_itemSelectionChanged();

    void onRepairButtonClick();
    void on_cbxRepairType_currentIndexChanged(int);

    void on_listWidgetRepair_itemSelectionChanged();

    void on_pushButton_47_clicked();

    void on_pushButton_46_clicked();

    void rightClickedOperation();
    void on_tableView_customContextMenuRequested(const QPoint);

    void on_tableWidget_customContextMenuRequested(const QPoint);
    void on_AdminTitleW_customContextMenuRequested(const QPoint);

    void on_clientTable_customContextMenuRequested(const QPoint);

    void on_lessonListWidget_customContextMenuRequested(const QPoint);

    void on_tableWidgetLesson_customContextMenuRequested(const QPoint);

    void on_listWidgetAdvice_customContextMenuRequested(const QPoint);

    void on_listWidgetRepair_customContextMenuRequested(const QPoint );

    void on_listWidgetOrder_customContextMenuRequested(const QPoint );

    void on_listWidgetType_customContextMenuRequested(const QPoint);

    void on_listWidgetRequire_customContextMenuRequested(const QPoint );

    void on_listWidgetCategory_customContextMenuRequested(const QPoint);

    void on_pushButton_48_clicked();

    void on_pushButton_49_clicked();

    void on_pushButton_50_clicked();

private:
    Ui::AdminOption *ui;
    SignIn * signDialog;
    SignIn * newLessonDialog;
    bool ispress;
    QList<int> deleteRow;  //保存已删除但未确认的行
    QStringList headerList;
    QStringList header;
    QStringList lessonID;  //保存课程对应的记录id
    QStringList lessonHeader;
    int currentLessonIndex;
    QString classId;  //保存课程对应学生表的id

    QString FtpServerIP;
    QString FtpNumber;
    QString FtpPwd;
    QNetworkReply *reply;
    bool isFtpTest;

    QTimer *connectFtpTimer;
    int timeOutTimes;
    QPixmap unSolved;
    QPixmap solved;
    QList<AdInfo*> AdInfoList;

    QLabel * lab;
    QHBoxLayout *layout;
    QWidget * ItemWidget;
    QPushButton * btn;
    QLabel * icon;
    QStringList repairID;
    QStringList repairDetail;

    //右键菜单
    QMenu *popMenu;
    QMenu * titlemenu;
    QAction *copyAction;
    QAction *deleteAction;
    QAction *refreshAction;
    QAction *exportAction;
    QAction *importAction;
    QAction *logoutAction;
    QAction *quitAction;
    QAction *pasteAction;
    QAction *addClassAction;
    QAction *addSetAction;


    int LaboratoryShowType = 0;  //实验室界面查询的类型
    int clientShowType = 0;  //客户机信息界面查询的类型

    void initLesson();
    void initLessonInfo(int index);
    bool isTeacherExit(QString id);
    void initSystemSetInfo();
    void getAdviceHistory();
    void getRepairHistory();

    bool checkIsAdmin();  //检查是否为管理员在操作
};

#endif // ADMINOPTION_H
