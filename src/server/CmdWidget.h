#ifndef CMDWIDGET_H
#define CMDWIDGET_H

#include "Global.h"
#include "CmdLineEdit.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "CoolTitleBar.h"

class CmdWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CmdWidget(QWidget *parent = 0);
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent *);
    bool onlyone;
    QString ip;
    QTextEdit * pTxtEdit;
    CmdLineEdit * pLineEdit;
    CoolTitleBar *bar;

    void initForOneClient(const QString& IP, const QString& userName, const QString& userID);

signals:
    void cmd(const QString&,bool,const QString&);

public slots:
    void onReturnPressed();
    void onCmdResult(int);
    void onCmdCallBack(const QString&);
    void on_clearBtnClick();
    void on_execBtnClick();

private:
    int index;
    int successCount;
    QPushButton *clearBtn;
    QPushButton *execBtn;
    QComboBox *orderCbx;
    QStringList orderList;

    void execOrder();
};

#endif // CMDWIDGET_H
