#ifndef NETCONFIGDIALOG_H
#define NETCONFIGDIALOG_H

#include <QDialog>
#include <QNetworkInterface>
#include <QSettings>
#include <QListWidget>
#include <QMessageBox>

namespace Ui {
class NetConfigDialog;
}

class NetConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetConfigDialog(QWidget *parent = 0);
    ~NetConfigDialog();
    bool firstConfig;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::NetConfigDialog *ui;
    QListWidget *m_ListWidget ;
};

#endif // NETCONFIGDIALOG_H
