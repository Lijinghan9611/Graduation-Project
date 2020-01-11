#include "TranportFileWidget.h"
#include "ui_TranportFileWidget.h"
#include "Global.h"
#include "CoolTitleBar.h"
#include "MyMessageBox.h"
#include <QFileDialog>
#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QFileInfo>
#include <QDebug>
#include <qt_windows.h>
#include <QHeaderView>
#include <QMimeData>
#include <QDirIterator>
#include <QDir>
#ifdef Q_OS_WIN
#pragma comment(lib, "user32.lib")
#endif
TranportFileWidget::TranportFileWidget(int type,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TranportFileWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags( Qt::FramelessWindowHint| windowFlags());
    setWindowTitle("传输文件");

    ui->progressBar->setVisible(false);
    name = "";
    ui->selectBtn1->setObjectName("surebutton");
    ui->sendBtn1->setObjectName("surebutton");
    ui->pushButton->setObjectName("surebutton");
    ui->openFilePathBtn->setObjectName("surebutton");
    ui->startRecBtn->setObjectName("surebutton");


    ui->tableWidget->setRowCount(20);  //设置行数
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->pushButton->setEnabled(false);
    ui->sendBtn1->setEnabled(false);
    fileNameList.clear();


    isSended = false;

    if(type == 1)
    {
        currentType = 1;
        ui->tableWidget->setColumnCount(5);
        QStringList header;
        header<<"文件路径"<<"属性"<<"大小"<<"状态"<<"操作";
        ui->tableWidget->setHorizontalHeaderLabels(header);
        //设置列宽
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->tableWidget->horizontalHeader()->resizeSection(1,120);
        ui->tableWidget->horizontalHeader()->resizeSection(2,120);
        ui->tableWidget->horizontalHeader()->resizeSection(3,120);
        ui->tableWidget->horizontalHeader()->resizeSection(4,100);

        ui->label_2->setText("电子教室教学管理软件-传送文件");
        this->setAcceptDrops(true);  //设置可拉拽文件
        ui->startRecBtn->hide();
        ui->openFilePathBtn->hide();
    }
    else
    {
        currentType = 0;
        ui->tableWidget->setColumnCount(5);
        QStringList header;
        header<<"文件名"<<"文件路径"<<"属性"<<"大小"<<"机位号";
        ui->tableWidget->setHorizontalHeaderLabels(header);
        //设置列宽
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        ui->tableWidget->horizontalHeader()->resizeSection(2,120);
        ui->tableWidget->horizontalHeader()->resizeSection(3,120);
        ui->tableWidget->horizontalHeader()->resizeSection(4,120);

        ui->label_2->setText("电子教室教学管理软件-接收文件");
        ui->Hw->hide();
        ui->label->clear();
        ui->selectBtn1->hide();
        ui->sendBtn1->hide();
        isStartRecieve = false;
    }
}

void TranportFileWidget::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget * pw = this->window();
    if(pw->isTopLevel() && pw->isMaximized())
    {
        ui->maxBtn->setStyleSheet("QPushButton#maxBtn{background-image:url(:/images/images/TiTleBar/btn_restore_normal.png);background-color:transparent;background-repeat:no-repeat;background-position:center;}"
                                  "QPushButton#maxBtn:hover {background:url(:/images/images/TiTleBar/btn_restore_highlight.png),transparent;background-repeat:no-repeat;background-position:center;}"
                                  "QPushButton#maxBtn:pressed{background:url(:/images/images/TiTleBar/btn_restore_down.png),transparent;background-repeat:no-repeat;background-position:center;}");
        ui->maxBtn->setToolTip("恢复");

    }
    else
    {
        ui->maxBtn->setStyleSheet("QPushButton#maxBtn{background-image:url(:/images/images/TiTleBar/btn_max_normal.png);background-color:transparent;background-repeat:no-repeat;background-position:center;}"
                                  "QPushButton#maxBtn:hover {background:url(:/images/images/TiTleBar/btn_max_highlight.png),transparent;background-repeat:no-repeat;background-position:center;}"
                                  "QPushButton#maxBtn:pressed{background:url(:/images/images/TiTleBar/btn_max_down.png),transparent;background-repeat:no-repeat;background-position:center;}");
        ui->maxBtn->setToolTip("最大化");
    }

    QWidget::paintEvent(ev);
}


/**
* @brief 发送结果
* @date 2017/12/24
* @author han
* @return void
*/
void TranportFileWidget::onTransportFileCallBack()
{
    ui->progressBar->setValue((ui->progressBar->value())+1);
    int value = ui->progressBar->value();
    int maxValue = ui->progressBar->maximum();
    if(value < maxValue)  //未发送完毕
        return;
    ui->labSuccessCount->setText(QString::number(sendIndex+1,10));
    if(sendIndex < ui->tableWidget->rowCount() && !sendOver)  //防止出错
    {
        ui->tableWidget->item(sendIndex,3)->setText("已发送");
        ui->tableWidget->item(sendIndex,3)->setForeground(Qt::green);
    }
    else if(sendOver)
    {
        ui->tableWidget->item(sendIndex,3)->setText("文件过大");
        ui->tableWidget->item(sendIndex,3)->setForeground(Qt::red);
        sendOver = false;
    }
    sendIndex++;
    if(sendIndex == fileNameList.size()) //发送完毕
    {
        //设置界面
        ui->pushButton->setEnabled(true);
        ui->sendBtn1->setEnabled(true);
        ui->selectBtn1->setEnabled(true);
        ui->progressBar->setVisible(false);
        return;
    }
    if(sendIndex < ui->tableWidget->rowCount())  //防止出错
    {
        ui->tableWidget->item(sendIndex,3)->setText("正在发送");
        ui->tableWidget->item(sendIndex,3)->setForeground(Qt::blue);
    }
    sendFile(fileNameList.at(sendIndex));  //发送下一个
}


/**
* @brief 选择文件
* @date 2017/12/24
* @author han
* @return void
*/
void TranportFileWidget::selectFile()
{
    fileNameList.clear();
#ifdef Q_OS_WIN
    fileNameList = QFileDialog::getOpenFileNames(this,"选择文件","/","flies(*)");
#else
    fileNameList = QFileDialog::getOpenFileNames(this,"选择文件","/","flies(*)");
#endif
    if(fileNameList.size() == 0)  //没有选择文件
        return;
    for(int i = 0;i < fileNameList.size();i++)
    {
        if(i == ui->tableWidget->rowCount())  //插入行
            ui->tableWidget->insertRow(i);
        //"文件路径"<<"属性"<<"大小"<<"状态";
        QString filePath = fileNameList.at(i);
        QFileInfo f(filePath);

        //获取扩展名
        QString extension = "";
        if(f.fileName().contains("."))
        {
            QStringList list = f.fileName().split(".");
            extension = list.at(list.size()-1);
        }

        QLineEdit *le = new QLineEdit;
        le->setText(filePath);
        double size = f.size() *1.0 /1024 /1024;  //转换成m

        ui->tableWidget->setCellWidget(i,0,le);
        //ui->tableWidget->setItem(i,0,new QTableWidgetItem(filePath));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(extension));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(size,10,3)+ "m"));

        if(f.size() < 0.5*1024*1024*1024)  //500m,太大会导致软件崩溃
            ui->tableWidget->setItem(i,3,new QTableWidgetItem("待发送"));
        else
        {
            ui->tableWidget->setItem(i,3,new QTableWidgetItem("文件过大"));
            ui->tableWidget->item(i,3)->setForeground(Qt::red);
        }
        QPushButton *pBtn = new QPushButton("删除");
        pBtn->setObjectName("deleteBtn");
        connect(pBtn, SIGNAL(clicked()), this, SLOT(OnBtnClicked()));
        ui->tableWidget->setCellWidget(i,4,pBtn);

        //居中
        ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
    ui->laFileCount->setText(QString::number(fileNameList.size(),10));
    //ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->sendBtn1->setEnabled(true);
    ui->pushButton->setEnabled(true);
    isSended = false;
}


/**
* @brief 发送
* @date 2017/12/24
* @author han
* @return void
*/
void TranportFileWidget::sendFile(QString filename)
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
        onTransportFileCallBack();
        return;
    }
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(Global::hostNum);
    emit fileName(filename);  //ThreadSend::onTransportFile()  接收
}
void TranportFileWidget::closeEvent(QCloseEvent *)
{
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
    name = "";
}


TranportFileWidget::~TranportFileWidget()
{
    delete ui;
}

void TranportFileWidget::on_selectBtn1_clicked()
{
    selectFile();
}


/**
* @brief 发送按键
* @date 2017/12/24
* @author han
* @return void
*/
void TranportFileWidget::on_sendBtn1_clicked()
{
    bool isResend = false;
    if(isSended)  //避免重复发送
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::information,"是否重新发送");
        if(!box.exec())
            return;
        isResend = true;
    }
    isSended = true;
    ui->sendBtn1->setEnabled(false);
    ui->selectBtn1->setEnabled(false);
    ui->progressBar->setVisible(true);
    if(isResend)  //如果是重新发送
    {
        for(int i = 0;i < fileNameList.size();i++)
        {
            if(i < ui->tableWidget->rowCount())  //避免出错
            {
                ui->tableWidget->item(i,3)->setText("待发送");
                ui->tableWidget->item(i,3)->setForeground(Qt::black);
            }
        }

    }
    sendIndex = 0;
    if(sendIndex < ui->tableWidget->rowCount())  //防止出错
    {
        ui->tableWidget->item(sendIndex,3)->setText("正在发送");
        ui->tableWidget->item(sendIndex,3)->setForeground(Qt::blue);
    }
    sendFile(fileNameList.at(sendIndex));
}

/**
* @brief 当用户拖动文件到窗口部件上时候，就会触发dragEnterEvent事件
* @date 2017/12/24
* @author han
* @return void
*/
void TranportFileWidget::dragEnterEvent(QDragEnterEvent *event)
{
    //如果为文件，则支持拖放
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

/**
* @brief 当用户放下这个文件后，就会触发dropEvent事件
* @date 2017/12/24
* @author han
* @return void
*/
void TranportFileWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
        return;

    QStringList tempList;
    tempList.clear();
    int startIndex;
    if(fileNameList.size() == 0)  //获取表格的文件数
        startIndex = 0;
    else
        startIndex = fileNameList.size();

    foreach(QUrl url, urls)  //获取拉进来的文件列表
    {
        QString file_name = url.toLocalFile();

        QFileInfo f(file_name); //判断是否为路径

        QStringList temp;
        if(f.isDir())  //为路径
        {
            temp = addSubFolderImages(file_name);  //获取文件
            tempList.append(temp);
            fileNameList.append(temp);
        }
        else
        {
            tempList.append(file_name);
            fileNameList.append(file_name);
        }
        //qDebug() << file_name;
    }

    if(tempList.size() == 0)  //没有选择文件
        return;

    for(int i = 0;i < tempList.size(); i++)  //添加到表格后面
    {
        if(i + startIndex == ui->tableWidget->rowCount())
            ui->tableWidget->insertRow(i + startIndex);
        //"文件路径"<<"属性"<<"大小"<<"状态";
        QString filePath = tempList.at(i);
        QFileInfo f(filePath);
        //获取扩展名
        QString extension = "";
        if(f.fileName().contains("."))
        {
            QStringList list = f.fileName().split(".");
            extension = list.at(list.size()-1);
        }

        QLineEdit *le = new QLineEdit;
        le->setText(filePath);
        ui->tableWidget->setCellWidget(i + startIndex,0,le);
        double size = f.size() *1.0 /1024 /1024;  //转换成m

        //ui->tableWidget->setItem(i + startIndex,0,new QTableWidgetItem(filePath));
        ui->tableWidget->setItem(i + startIndex,1,new QTableWidgetItem(extension));
        ui->tableWidget->setItem(i + startIndex,2,new QTableWidgetItem(QString::number(size,10,3)+ "m"));

        if(f.size() < 0.5*1024*1024*1024)  //500m,太大会导致软件崩溃
            ui->tableWidget->setItem(i + startIndex,3,new QTableWidgetItem("待发送"));
        else
        {
            ui->tableWidget->setItem(i + startIndex,3,new QTableWidgetItem("文件过大"));
            ui->tableWidget->item(i + startIndex,3)->setForeground(Qt::red);
        }

        QPushButton *pBtn = new QPushButton("删除");
        pBtn->setObjectName("deleteBtn");
        connect(pBtn, SIGNAL(clicked()), this, SLOT(OnBtnClicked()));
        ui->tableWidget->setCellWidget(i + startIndex,4,pBtn);

        //居中
        ui->tableWidget->item(i + startIndex,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i + startIndex,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->item(i + startIndex,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
    ui->laFileCount->setText(QString::number(fileNameList.size(),10));
    //ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->sendBtn1->setEnabled(true);
    ui->pushButton->setEnabled(true);
    isSended = false;
}


/**
* @brief 表格的删除按键点击
* @date 2017/12/26
* @author han
* @return void
*/
void TranportFileWidget::OnBtnClicked()
{
    QPushButton * senderObj=qobject_cast<QPushButton *>(sender());
    if(senderObj == 0)
    {
        return;
    }
    QModelIndex index =ui->tableWidget->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row=index.row();
    //移除文件
    ui->tableWidget->removeRow(row);
    fileNameList.removeAt(row);
    ui->laFileCount->setText(QString::number(fileNameList.size(),10));
    if(fileNameList.size() == 0)
    {
        ui->sendBtn1->setEnabled(false);
        isSended = false;
    }
}

/**
* @brief 遍历文件夹
* @date 2017/12/24
* @author han
* @return void
*/
QStringList TranportFileWidget::addSubFolderImages(QString path)
{
    //判断路径是否存在
    QDir dir(path);
    if(!dir.exists())
    {
        return QStringList();
    }

    //获取所选文件类型过滤器
    QStringList filters;
    //    文件过滤
    //    filters<<QString("*.jpeg")<<QString("*.jpg")<<QString("*.png")<<QString("*.tiff")<<QString("*.gif")<<QString("*.bmp");

    //定义迭代器并设置过滤器
    QDirIterator dir_iterator(path,
                              filters,
                              QDir::Files | QDir::NoSymLinks,
                              QDirIterator::Subdirectories);
    QStringList string_list;
    while(dir_iterator.hasNext())  //遍历
    {
        dir_iterator.next();
        QFileInfo file_info = dir_iterator.fileInfo();
        QString absolute_file_path = file_info.absoluteFilePath();
        string_list.append(absolute_file_path);
        qDebug() << absolute_file_path;
    }

    return string_list;
}

/**
* @date 2017/12/24
* @author han
* @brief 清空列表
* @return void
*/
void TranportFileWidget::on_pushButton_clicked()
{ 
    ui->selectBtn1->setEnabled(true);
    ui->sendBtn1->setEnabled(true);
    ui->progressBar->hide();

    fileNameList.clear();
    ui->labSuccessCount->setText("0");
    ui->laFileCount->setText("0");
    ui->tableWidget->clearContents();
    ui->openFilePathBtn->setEnabled(false);
    receiveFileCount = 0;
}

void TranportFileWidget::on_minBtn_clicked()
{
    this->showMinimized();
}

void TranportFileWidget::on_closeBtn_clicked()
{
    this->close();
}

void TranportFileWidget::on_maxBtn_clicked()
{
    if(this->isMaximized()) //如果已经是最大化了，就显示正常的
    {
        this->showNormal();
    }
    else
    {
        this->showMaximized();
    }
}


void TranportFileWidget::mousePressEvent(QMouseEvent *event)
{
#ifdef Q_OS_WIN
    if (ReleaseCapture())
    {
        QWidget *pWindow = this->window();
        if (pWindow->isTopLevel())
        {
            SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
        }
    }
    event->ignore();
#else
    if(event->button() == Qt::LeftButton)
    {
        QWidget * p = this->window();
        if(p->isTopLevel())
        {
            m_DragPosition = event->globalPos() - p->pos();
            ispress = true;
            event->accept();
        }
    }
#endif
}

void TranportFileWidget::mouseMoveEvent(QMouseEvent *event)
{
#ifdef Q_OS_WIN
    Q_UNUSED(event);
#else
    if (ispress && event->buttons() && Qt::LeftButton)
    {
        QWidget * p = this->window();
        if(p->isTopLevel())
        {
            if(p->isMaximized())
                p->showNormal();
            p->move(event->globalPos() - m_DragPosition);
            event->accept();
        }
    }
#endif
}

void TranportFileWidget::mouseReleaseEvent(QMouseEvent *)
{
    ispress =false;
    update();
}

void TranportFileWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
        emit ui->maxBtn->clicked();
}


/**
* @brief 文件接收
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void TranportFileWidget::on_startRecBtn_clicked()
{
    if(isStartRecieve)
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::info_no_beep,"确定停止接收?");
        if(box.exec())
        {
            Global::isReceivingFile = false;
            ui->label->clear();
            isStartRecieve = false;
            ui->startRecBtn->setText("开始接收");
            emit stopRecieveFileSignal();
        }
        return;
    }

    Global::isReceivingFile = true;
    ui->label->setText("等待中...");
    ui->startRecBtn->setText("停止接收");
    isStartRecieve = true;
    receiveFileCount = 0;
    emit startRecieveFileSignal();
}


/**
* @brief 收到文件
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void TranportFileWidget::onReceiveFile(const QString&ip, const QString &filename)
{
    //header<<"文件路径"<<"属性"<<"大小"<<"机位号";
    receiveFileList.append(filename);
    int i = receiveFileList.size() - 1;
    if(i == ui->tableWidget->rowCount())  //插入行
    {
        ui->tableWidget->insertRow(i);
    }

    ui->pushButton->setEnabled(true);
    ui->openFilePathBtn->setEnabled(true);
    receiveFileCount ++ ;
    ui->label->setText(QString("收到%1个文件").arg(receiveFileCount));
    //获取机位号
    QSettings * set = new QSettings("config.ini",QSettings::IniFormat);
    QString position = set->value("client/"+ip,"").toString();

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
    ui->tableWidget->setItem(i,4,new QTableWidgetItem(position));

    //居中
    ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(i,2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(i,3)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableWidget->item(i,4)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

}


/**
* @brief 打开文件所在路径
* @date 2018/1/25
* @author han
* @param
* @return void
*/
void TranportFileWidget::on_openFilePathBtn_clicked()
{
    if(!filePath.isEmpty())
    {//两种打开方式
        if(!QDesktopServices::openUrl(QUrl::fromLocalFile(filePath)))  //win10通过
        {
            if(!QDesktopServices::openUrl(QUrl(filePath, QUrl::TolerantMode)))
            {
                MyMessageBox box;
                box.iniMsgBox(MyMessageBox::error,QString("打开路径%1失败").arg(filePath));
                box.isShowDialog(true);
                box.exec();
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
