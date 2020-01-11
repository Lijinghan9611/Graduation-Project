#include "MainWindow.h"
#include <QApplication>
#include "Config.h"
#include "Global.h"
#include <QSettings>
#include <QHostInfo>
#include <windows.h>
#include <QMessageBox>
#define REG_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //获取软件所在路径,带文件名
    QString application_path = QApplication::applicationFilePath();
    std::wstring wlpstr = application_path.toStdWString();
    LPCWSTR lpcwStr = wlpstr.c_str();
    //创建一个互斥量来保证只运行一个实例
    HANDLE hMutex = CreateMutex(NULL, TRUE, lpcwStr);
    if(hMutex != NULL)
    {
        if(GetLastError() == ERROR_ALREADY_EXISTS)
        {
            QMessageBox::critical(qApp->activeWindow(), QObject::tr("已经有一个实例在运行!"), QObject::tr("同一时间是能运行一个实例."));
            return 1;
        }
    }

    qApp->setStyleSheet("QLineEdit{font-family:幼园;font-size:15px;border: 1px solid gray;  border-radius: 3px; padding: 1px 2px 1px 2px;}"
                        "QLineEdit:hover{border:2px solid rgb(50, 180, 250);}"
                        "Sign QLabel{font-size:15px;font-family:幼园;color:white;}"
                        "Question QLabel{font-size:16px;font-family:幼园;color:white;}"
                        "QPushButton{background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgb(46,46,46),stop:1 rgb(66,66,66));color:white;padding:4px;border-radius:6px;}"
                        "QPushButton:hover{background-color:rgb(80,80,80);}"
                        "QPushButton:pressed{background-color:rgb(30,30,30)}"
                        "MsgLabel{background-color:beige;border: 7px solid grey; }"
                        "QLabel#msglabel{color:black;font-size:20px;font-family:幼圆}"
                        "");
    Config con;
    QSettings * set = new QSettings(Global::getAppConfigPath(), QSettings::IniFormat);
    /*
    //在启动菜单创建快捷开机启动
#ifdef Q_OS_WIN
    QString startMenuPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)
            .append("/Startup");
    QString checkPath = startMenuPath + "/ZQUCLIENT.lnk";  //开始菜单路径
    QFile file(checkPath);
    QDir dir(startMenuPath);
    if(dir.exists())  //判断开始菜单路径是否存在
    {
        if(!file.exists())  //判断是否已经创建快捷,不存在则创建
        {
            //创建快捷
            QFile::link(application_path, startMenuPath.append("/").append("ZQUCLIENT.lnk"));
        }
    }
#else
#endif
*/
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

    if (!set->value("config/config").toBool())
    {
        if (con.exec() == QDialog::Accepted)  //第一次运行，打开配置界面
        {
            MainWindow w;
            // w.show();  //待注释
            w.showLogin();
           return a.exec();
        }
    }
    else
    {
        MainWindow w;
        //w.show();  //待注释
        w.showLogin();
        return a.exec();
    }
}
