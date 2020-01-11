#include "MainWindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>
#include "UiConfigDialog.h"
#include "NetConfigDialog.h"
#include <QDir>
#include <QStandardPaths>
#include <windows.h>
#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //判断在roaming中是否存在软件的文件夹,用来保存配置文件
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir appDataPathDir(appDataPath);
    if(!appDataPathDir.exists())
    {
        appDataPathDir.mkpath(appDataPath);
    }

    //获取软件所在路径,带文件名
    QString application_path = QApplication::applicationFilePath();
    std::wstring wlpstr = application_path.toStdWString();
    LPCWSTR lpcwStr = wlpstr.c_str();

    //创建一个互斥量来保证只运行一个实例
    HANDLE hMutex = CreateMutex(NULL, TRUE, lpcwStr);
    if(hMutex != NULL)
    {
        if(GetLastError() == ERROR_ALREADY_EXISTS)  //已存在
        {
            QMessageBox::critical(qApp->activeWindow(), QObject::tr("已经有一个实例在运行!"), QObject::tr("同一时间是能运行一个实例."));
            return 1;
        }
    }

    QImage img(":/images/images/else/icon1.png");
    QIcon ico(QPixmap::fromImage(img));
    a.setWindowIcon(ico);

    //获取软件主题
    QSettings * set = new QSettings(Global::getAppConfigPath(),QSettings::IniFormat);

    QString style = set->value("style/style","BlueWhite.qss").toString();
    QFile qss(":/qssfile/files/"+style);
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();

    Global::skinFileName = style;

    //在启动菜单创建快捷，开机启动
    //#ifdef Q_OS_WIN
    //    QString startMenuPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)
    //            .append("/Startup");
    //    QString checkPath = startMenuPath + "/ZQUSERVER.lnk";  //开始菜单路径
    //    QFile file(checkPath);
    //    QDir dir(startMenuPath);
    //    if(dir.exists())  //判断开始菜单路径是否存在
    //    {
    //        if(!file.exists())  //判断是否已经创建快捷,不存在则创建
    //        {
    //            //创建快捷
    //            QFile::link(application_path, startMenuPath.append("/").append("ZQUSERVER.lnk"));
    //        }
    //    }
    //#else
    //#endif

    //创建注册表，开机启动
#ifdef Q_OS_WIN
    if(!set->value("config/AddRegedit").toBool())
    {
        QString application_name = QApplication::applicationName();
        QSettings *settings = new QSettings(REG_RUN, QSettings::NativeFormat);
        QString application_path = QApplication::applicationFilePath();
        settings->setValue(application_name, application_path.replace("/", "\\"));
        set->setValue("config/AddRegedit",true);
    }
#else
#endif


    //第一次运行,显示配置界面
    if(!set->value("config/config").toBool())
    {
        UiConfigDialog dia;
        dia.firstConfig = true;
        dia.dlg.firstConfig = true;
        dia.exec();
    }

    MainWindow w;
    w.showLoginWindow();

    delete set;
    return a.exec();
}
