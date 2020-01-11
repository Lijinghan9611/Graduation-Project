#include "fileinfodialog.h"
#include "ui_fileinfodialog.h"
#include <QClipboard>
#include "stoast.h"

FileInfoDialog::FileInfoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileInfoDialog)
{
    ui->setupUi(this);

    //设置标志，显示最大最小按键
    setWindowFlags(Qt::CustomizeWindowHint
                   | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint
                   | Qt::WindowCloseButtonHint);

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(30);  //设置行数
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    menu = new QMenu;
    copyAction = new QAction("复制",this);
    deleteAction= new QAction("删除",this);
    // 添加图标
    QIcon icon(":/image/images/action/copy.png");
    copyAction->setIcon(icon);
    QIcon icon1(":/image/images/action/delete.png");
    deleteAction->setIcon(icon1);

    // 设置快捷键
    copyAction->setShortcut(QKeySequence::Copy);
    connect(menu,SIGNAL(triggered(QAction*)),this,SLOT(onTrayMenuClick(QAction*)));

    fileNameList.clear();
    isSended = false;
    receiveFileCount = 0;
    sendOver = false;
}


/**
* @brief 右键菜单响应
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void FileInfoDialog::onTrayMenuClick(QAction*action)
{
    if(action == copyAction)
    {
        QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
        QModelIndex index =  ui->tableWidget->currentIndex();
        QString originalText = index.data(Qt::DisplayRole).toString();
        clipboard->setText(originalText);                  //设置剪贴板内容
    }
    else if(deleteAction == action)
    {
        QModelIndex index =  ui->tableWidget->currentIndex();
        ui->tableWidget->removeRow(index.row());
        fileNameList.removeAt(index.row());
    }
}

/**
* @brief 初始化界面
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void FileInfoDialog::initDialog(int type)
{
    if(type == 1)
    {
        ui->tableWidget->setColumnCount(4);
        QStringList header;
        header<<"文件名"<<"路径"<<"属性"<<"大小";
        ui->tableWidget->setHorizontalHeaderLabels(header);
        setWindowTitle("从教师端接收文件");
        ui->sendFileBtn->hide();
        ui->selectFileBtn->hide();
    }
    else
    {
        ui->tableWidget->setColumnCount(5);
        QStringList header;
        header<<"文件名"<<"路径"<<"属性"<<"大小" << "状态";
        ui->tableWidget->setHorizontalHeaderLabels(header);

        setWindowTitle("发送文件到教师端");
        ui->openRecFilePath->hide();
    }
}
void FileInfoDialog::closeEvent(QCloseEvent*ev)
{
    ev->ignore();
    this->hide();
}

FileInfoDialog::~FileInfoDialog()
{
    delete ui;
}


/**
* @brief 右键菜单
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void FileInfoDialog::on_tableWidget_customContextMenuRequested(const QPoint &)
{
    menu->clear();
    menu->addAction(copyAction);
    menu->addAction(deleteAction);

    QModelIndex index =  ui->tableWidget->currentIndex();
    QString originalText = index.data(Qt::DisplayRole).toString().trimmed();
    if(originalText.isEmpty())
    {
        copyAction->setEnabled(false);
        deleteAction->setEnabled(false);
    }
    else
    {
        copyAction->setEnabled(true);
        deleteAction->setEnabled(true);
    }
    menu->exec(QCursor::pos());
}


/**
* @brief 清空列表
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void FileInfoDialog::on_clearBtn_clicked()
{
    ui->tableWidget->clearContents();
    receiveFileCount = 0;
    fileNameList.clear();
    ui->selectFileBtn->setEnabled(false);
}


/**
* @brief 选择文件
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void FileInfoDialog::on_selectFileBtn_clicked()
{
    fileNameList.clear();
#ifdef Q_OS_WIN
    fileNameList = QFileDialog::getOpenFileNames(this,"选择文件","/","flies(*)");
#else
    fileNameList = QFileDialog::getOpenFileNames(this,"选择文件","/","flies(*)");
#endif
    if(fileNameList.size() == 0)  //没有选择文件
        return;

    ui->selectFileBtn->setEnabled(true);
    for(int i = 0;i < fileNameList.size();i++)
    {
        if(i == ui->tableWidget->rowCount())
            ui->tableWidget->insertRow(i);
        //"文件名"<<"路径"<<"属性"<<"大小";
        QString filePath = fileNameList.at(i);
        QFileInfo f(filePath);

        //获取扩展名
        QString extension = "";
        if(f.fileName().contains("."))
        {
            QStringList list = f.fileName().split(".");
            extension = list.at(list.size()-1);
        }

        QString fileName = f.fileName();
        QLineEdit *le = new QLineEdit;
        le->setText(filePath);
        double size = f.size() *1.0 /1024 /1024;  //转换成m

        ui->tableWidget->setItem(i,0,new QTableWidgetItem(fileName));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(filePath));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(extension));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(QString::number(size,10,3)+ "m"));

        if(f.size() < 0.5*1024*1024*1024)  //500m,太大会导致软件崩溃
            ui->tableWidget->setItem(i,4,new QTableWidgetItem("待发送"));
        else
        {
            ui->tableWidget->setItem(i,4,new QTableWidgetItem("文件过大"));
            ui->tableWidget->item(i,4)->setForeground(Qt::red);
        }
        //居中
        ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
    isSended = false;
}


/**
* @brief 发送文件
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void FileInfoDialog::on_sendFileBtn_clicked()
{
    if(isSended)  //避免重复发送
    {
        SingleTonSToast::getInstance().setMessageVDuration("已发送，请不要重复操作", 4000,1);
        return;
    }
    isSended = true;
    ui->clearBtn->setEnabled(false);
    ui->sendFileBtn->setEnabled(false);
    ui->selectFileBtn->setEnabled(false);

    sendIndex = 0;
    if(sendIndex < ui->tableWidget->rowCount())  //防止出错
    {
        ui->tableWidget->item(sendIndex,4)->setText("正在发送");
        ui->tableWidget->item(sendIndex,4)->setForeground(Qt::blue);
    }
    sendFile(fileNameList.at(sendIndex));
}

/**
* @brief 发送
* @date 2017/12/24
* @author han
* @return void
*/
void FileInfoDialog::sendFile(QString filename)
{
    if(filename == "")
    {
        return;
    }
    QFileInfo f(filename);
    sendOver = false;
    if(f.size() > 0.5*1024*1024*1024)  //500m,太大会导致软件崩溃
    {
        sendOver = true;
        onTransportFileCallBack();  //自动调用发送结果
        return;
    }
    emit sendFileSignal(filename);
}

/**
* @brief 发送结果
* @date 2017/12/24
* @author han
* @return void
*/
void FileInfoDialog::onTransportFileCallBack()
{
    if(sendIndex < ui->tableWidget->rowCount() && !sendOver)  //防止出错
    {
        ui->tableWidget->item(sendIndex,4)->setText("已发送");
        ui->tableWidget->item(sendIndex,4)->setForeground(Qt::green);
    }
    else if(sendOver)
    {
        sendOver = false;
        ui->tableWidget->item(sendIndex,4)->setText("文件过大");
        ui->tableWidget->item(sendIndex,4)->setForeground(Qt::red);
    }

    sendIndex++;
    if(sendIndex == fileNameList.size()) //发送完毕
    {
        //设置界面
        ui->clearBtn->setEnabled(true);
        ui->sendFileBtn->setEnabled(true);
        ui->selectFileBtn->setEnabled(true);
        return;
    }
    if(sendIndex < ui->tableWidget->rowCount())  //防止出错
    {
        ui->tableWidget->item(sendIndex,4)->setText("正在发送");
        ui->tableWidget->item(sendIndex,4)->setForeground(Qt::blue);
    }
    sendFile(fileNameList.at(sendIndex));  //发送下一个
}



/**
* @brief 收到文件
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void FileInfoDialog::onReceiveFile(const QString &filename)
{
    //显示本窗口
    this->show();
    this->activateWindow();

    int i = receiveFileCount;
    if(i == ui->tableWidget->rowCount())  //插入行
    {
        ui->tableWidget->insertRow(i);
    }

    ui->clearBtn->setEnabled(true);
    ui->openRecFilePath->setEnabled(true);
    receiveFileCount ++ ;
    ui->label->setText(QString("收到%1个文件").arg(receiveFileCount));

    QFileInfo f(filename);
    filePath = f.path();
    //获取扩展名
    QString extension = "";
    if(f.fileName().contains("."))
    {
        QStringList list = f.fileName().split(".");
        extension = list.at(list.size()-1);
    }
    //文件大小
    double size = f.size() *1.0 /1024 /1024;  //转换成m
    //文件名
    QString name = f.fileName();

    ui->tableWidget->setItem(i,0,new QTableWidgetItem(name));
    ui->tableWidget->setItem(i,1,new QTableWidgetItem(filename));
    ui->tableWidget->setItem(i,2,new QTableWidgetItem(extension));
    ui->tableWidget->setItem(i,3,new QTableWidgetItem(QString::number(size,10,3)+ "m"));

    //居中
    ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
}

/**
* @brief 打开文件所在路径
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void FileInfoDialog::on_openRecFilePath_clicked()
{
    if(!filePath.isEmpty())
    {//两种打开方式
        if(!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath)))  //win10通过
        {
            if(!QDesktopServices::openUrl(QUrl(filePath, QUrl::TolerantMode)))
            {
                SingleTonSToast::getInstance().setMessageVDuration(QString("打开路径%1失败").arg(filePath), 7000,1);
                return;
            }
        }
    }
    else
    {
        SingleTonSToast::getInstance().setMessageVDuration("路径不存在", 5000,1);
        return;
    }
}
