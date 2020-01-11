#ifndef SIGNIN_H
#define SIGNIN_H

#include <QDialog>
#include <QCompleter>
namespace Ui {
class SignIn;
}

class SignIn : public QDialog
{
    Q_OBJECT

public:
    explicit SignIn(QWidget *parent = 0,QString = "");
    ~SignIn();
    void closeEvent(QCloseEvent*);
    void initEditTeacherDialog();

private slots:
    void on_pushButton_clicked();
    void check();

    void on_returnBtn_clicked();

    void on_lineEdit_2_selectionChanged();

signals:
     void newItem(const QString&,const QString&);
     void newItem(const QString&, const QString&,const QString&);

private:
    Ui::SignIn *ui;
    QString kind;
    QCompleter *completer;
    QCompleter *completer1;
};

#endif // SIGNIN_H
