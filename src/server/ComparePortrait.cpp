#include "ComparePortrait.h"
#include "ui_ComparePortrait.h"
#include "Global.h"
#include "CommonTitleBar.h"
#include "QVBoxLayout"
#include <QDebug>
#include <QLabel>
#include <QStyleOption>
#include <QPainter>

ComparePortrait::ComparePortrait(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComparePortrait)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_QuitOnClose,false);
    setWindowFlags( Qt::FramelessWindowHint| windowFlags());
    setWindowTitle("学生图像对比");
    CommonTitleBar * bar = new CommonTitleBar(this);
    bar->setObjectName("black");
    QVBoxLayout * pLayout  =  new QVBoxLayout(this);
    pLayout->addWidget(bar);
    pLayout->addStretch(0);
    pLayout->setContentsMargins(0,0,0,0);
    setLayout(pLayout);

    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setColumnCount(3);
    QStringList header;
    header<<"学号"<<"已保存图像"<<"点名时图像";
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setColumnWidth(0,200);
    ui->tableWidget->setColumnWidth(1,175);
    ui->tableWidget->setColumnWidth(2,175);
}

void ComparePortrait::paintEvent(QPaintEvent *ev)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    QWidget::paintEvent(ev);
}

void ComparePortrait::onBegin(int classId)
{
    ui->tableWidget->clearContents();
    QString table = "class" + QString::number(classId);
    Global g;
    g.connectdb();
    g.sql.exec(QString("select id,portrait from %1").arg(table));
    ui->tableWidget->setRowCount(g.sql.size());
    int i = 0;
    while(g.sql.next())
    {
        QImage img;
        if(g.sql.value(1).toString() == "")
            img.load(":/images/images/else/common.jpg");
        else
            img.load(g.sql.value(1).toString());

        ui->tableWidget->setItem(i,0,new QTableWidgetItem(g.sql.value(0).toString()));
        QLabel *label = new QLabel(this);
        label->setPixmap(QPixmap::fromImage(img.scaled(170,120)));
        ui->tableWidget->setCellWidget(i,1,label);
        ui->tableWidget->setRowHeight(i,130);
        i++;
    }
}

void ComparePortrait::onClientImgCome(const QString &id, const QImage &img)
{
    for(int i =0 ;i < ui->tableWidget->rowCount();i++)
    {
        if(ui->tableWidget->item(i,0)->text() == id)
        {
            QLabel *label = new QLabel(this);
            label->setPixmap(QPixmap::fromImage(img));
            ui->tableWidget->setCellWidget(i,2,label);
            return;
        }
    }
}


ComparePortrait::~ComparePortrait()
{
    delete ui;
}
