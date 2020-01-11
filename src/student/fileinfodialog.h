#ifndef FILEINFODIALOG_H
#define FILEINFODIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QMenu>
#include <QFileDialog>
#include <QFileInfo>
#include <QLineEdit>
#include <QDesktopServices>
namespace Ui {
class FileInfoDialog;
}

class FileInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileInfoDialog(QWidget *parent = 0);
    void initDialog(int type);
    void closeEvent(QCloseEvent*ev);
    ~FileInfoDialog();

private:
    Ui::FileInfoDialog *ui;
    QMenu * menu;
    QAction * copyAction;
    QAction *deleteAction;
    QStringList fileNameList;
    bool isSended;
    int sendIndex;
    QString filePath;
    int receiveFileCount;
    bool sendOver;//是否为跳过文件
    void sendFile(QString filename);

public slots:
    void onTrayMenuClick(QAction*action);
    void onTransportFileCallBack();
    void onReceiveFile(const QString &);

private slots:
    void on_tableWidget_customContextMenuRequested(const QPoint &);
    void on_clearBtn_clicked();
    void on_selectFileBtn_clicked();
    void on_sendFileBtn_clicked();

    void on_openRecFilePath_clicked();

signals:
    void sendFileSignal(const QString&);
};

#endif // FILEINFODIALOG_H
