#ifndef TRANPORTFILEWIDGET_H
#define TRANPORTFILEWIDGET_H

#include <QWidget>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QLineEdit>
#include <QMouseEvent>
#include <QDropEvent>
namespace Ui {
class TranportFileWidget;
}

class TranportFileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TranportFileWidget(int type, QWidget *parent = 0);
    ~TranportFileWidget();
    void closeEvent(QCloseEvent*);
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent*event);
    void mouseReleaseEvent(QMouseEvent*event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void selectFile();
    void sendFile(QString filename);
    QStringList addSubFolderImages(QString path);
public slots:
    void onTransportFileCallBack();
    void onReceiveFile(const QString&,const QString&);
signals:
    void fileName(const QString&);

private slots:
    void on_selectBtn1_clicked();

    void on_sendBtn1_clicked();
    void on_pushButton_clicked();

    void on_minBtn_clicked();

    void on_closeBtn_clicked();

    void on_maxBtn_clicked();

    void OnBtnClicked();

    void on_startRecBtn_clicked();


    void on_openFilePathBtn_clicked();

signals:
    void startRecieveFileSignal();  //开始接收文件
    void stopRecieveFileSignal();  //结束接收文件

private:
    Ui::TranportFileWidget *ui;
    QString name;
    bool ispress;
    QStringList fileNameList;
    bool isSended;  //标记当前文件列表是否已发送
    int sendIndex = 0;  //已文件发送个数
    bool isStartRecieve;
    QStringList receiveFileList;
    int receiveFileCount;

    QString filePath;
    bool sendOver;   //是否为跳过文件
    int currentType = 0;
};

#endif // TRANPORTFILEWIDGET_H
