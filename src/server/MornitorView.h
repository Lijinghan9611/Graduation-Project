#ifndef MORNITORVIEW_H
#define MORNITORVIEW_H

#include "LabelView.h"
#include "ZoomLabel.h"
#include "Global.h"
#include "CmdWidget.h"
#include "SendMg.h"
#include "ClientAtribute.h"
#include<QList>
#include <QWidget>
#include <QPaintEvent>
#include <QMap>
#include <QTimer>
#include <QGridLayout>

class MornitorView : public QWidget
{
    Q_OBJECT
public:
    explicit MornitorView(QWidget *parent = 0);
    void paintEvent(QPaintEvent*);
    ~MornitorView();

public:
    int xLabelCount,yLabelCount;
    QString direction;
    char firstLeter;
    QString firstNum;
    QMap<QString,LabelView*>m_map;
    QMap<QString, QLabel*> word_map;  //显示位置
    QStringList lackNumList;
    bool commonState,mornitorState,cameraState;
    QString zoomIp;
    QString sendMgIp;
    QString currentStyle;
    QString clientIp,clientMac,clientName;

public slots:
    void onHideLabel(const QString&, const bool &flag);
    void onSetLabelStyle(const QString&,quint32);
    void onSetAllLabel(const quint32&);
    void onImageChange(const QString&,const QImage&);
    void onLeave(const QString&);

    void onMornitor(const QString&);
    void onZoomImageCome(const QString&,const QImage&);
    void onZoomEnd();

    void onHostMg(const QString&,const QString&,const QString&);  //tooltip
    void onSignMgToolTip(const QString&,const QString&,const QString&);
    void onSendMg(const QString&);
    void onSendMg2(const QString&);
    void onSkinChange();
    void onCheckClientMg(const QString&,const QString&,const QString&);
    void onClientAttribute(const QString&,const QString&,const QString&,const QString&);
    void onCloseZoomLabel();//退出登录时关闭zoomlabel
    void onClientComeCheckExits(const QString&);

signals:
    void zoomMornitor(const QString&);
    void zoomEnd(const QString&);
    void beginCmd(const QString&,const QString&,const QString&);
    void closeComputer(const QString&);
    void reGetPosition(const QString&);
    void restartComputer(const QString&);
    void msg(const QString&,const QString&);
    void clientCome(QString);
    void clientLeave(QString);
    void checkClientAttribute(const QString&);
    void signMg(const QString&,const QString&,const QString&);

private:
    ZoomLabel * zoomLabel;
    SendMg * sendMg;
    ClientAtribute *clientAttribute;
    QGridLayout *mainLayout;
    int UnknownClientCount;  //座位号不明确的主机
    int addColumnCount;  //新一行添加的个数
    bool isFirstAdd;
    QStringList UnknownClientPosition;

    void addClientToLayout(int i, int j, bool isLack, QString unKnownPosition = "",QString ip = "");
};

#endif // MORNITORVIEW_H
