#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include "Tool_Bar.h"
#include "MornitorView.h"
#include "Server.h"
#include "LabelStyle.h"
#include "Global.h"
#include <QUdpSocket>
#include <QTimer>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent*);
    QSystemTrayIcon *tray ;
    QLabel *labTeacherName;
    void showLoginWindow();

public slots:
    void onShowSituation();  //statubar
    void trayClick(QSystemTrayIcon::ActivationReason);
    void onTrayMenuClick(QAction *p);
    void onHideTray();
    void onShowTray();

	void onSendServerTimerTimeOut();  //发送服务端id

    void onChangeTname(const QString &);

signals:
    void onlogoutFromAdmin();

private:
    Ui::MainWindow *ui;
    MornitorView * pMornitorView ;
    Tool_Bar * pToolBar;
    LabelStyle * pLabelStyle ;
	QUdpSocket *sendServerUdpSocket;  //发送服务器ip的udp
	QTimer *sendServerTimer;  //发送服务器ip的timer
	int sendServerPort;  //发送服务器ip的广播端口
    QString getIP();
    QStringList LocalIpList;
    bool isFirst;
    QMenu *trayMenu;
    QAction *exitiAction;
    QAction *showMainAction;
    QAction *logoutAction;


};

#endif // MAINWINDOW_H
