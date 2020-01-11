#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QListWidget>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include "SignIn.h"
#include "UiConfigDialog.h"
#include "WorkLog.h"
#include "CoolTitleBar.h"

namespace Ui {
class LogIn;
}

class LogIn : public QDialog
{
    Q_OBJECT

public:
    explicit LogIn(QWidget *parent = 0);
    ~LogIn();
    void closeEvent(QCloseEvent*);
    SignIn *signin ;
    WorkLog * workLog;
    //CoolTitleBar * bar ;
    QLineEdit *idLineEdit;
    QString teacherName;
    QString adminPwd;
    void resetAll();

    void setFindLabEnable(bool f);
private slots:
    void on_pushButton_clicked();
    void comboClick(const QString&, const QString&name);
    void comboEnter();
    void onNewItem(const QString&,const QString&);
    void on_pushButton_3_clicked();
    void onSignEndBeignWorkLog();
    void onEditWorkLog();
    void onEditingFinish();

    void on_cbxPurpose_currentTextChanged(const QString &arg1);

    void on_leName_editingFinished();


    void on_pushButton_2_clicked();

protected:
     bool eventFilter(QObject *obj, QEvent *ev);
private:
    Ui::LogIn *ui;
    QListWidget *m_ListWidget;   
    bool labFind;

    QStringList getIpList();
signals:
    void loginSignal();

    void signEnd(const QString &, const QString &, const QString &, const QString &);//ini worklog

};

#endif // LOGIN_H
