#ifndef UICONFIGDIALOG_H
#define UICONFIGDIALOG_H

#include <QDialog>
#include <QSettings>
#include "NetConfigDialog.h"
#include "Global.h"


namespace Ui {
class UiConfigDialog;
}

class UiConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UiConfigDialog(QWidget *parent = 0);
    ~UiConfigDialog();
    NetConfigDialog dlg;
    bool firstConfig;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
public:
    Ui::UiConfigDialog *ui;
};

#endif // UICONFIGDIALOG_H
