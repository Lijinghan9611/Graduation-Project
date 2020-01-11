#include "Global.h"
#include "MyMessageBox.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QStandardPaths>

Global::Global()
{
}

int Global::hostNum = 0;
long long Global::teacherNum = 0;

bool Global::isReceivingFile = false;  //是否正在接收文件
bool Global::isBroadcast = false;
int Global::style = WM_Common;
bool Global::isAdmin = false;
int Global::week = 1;
int Global::LoginType = 0;
int Global::workLogId = -1;
int Global::AttendanceTime = 0;
QString Global::LocalIP = ""; //本机ip
bool Global::shutdownAutoGetIp = false; //是否关闭自动获取ip
QString Global::skinFileName = "";

void Global::connectdb()
{
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QMYSQL");
    QString hostname,username,password,dbname;
    QSettings *set = new QSettings(getAppConfigPath(),QSettings::IniFormat);
    hostname = set->value("database/hostname").toString();
    username = set->value("database/username").toString();
    password =set->value("database/password").toString();
    dbname = set->value("database/dbname").toString();
    delete set;

    db.setHostName(hostname);
    db.setUserName(username);
    db.setPassword(password);
    db.setDatabaseName(dbname);

    if(!db.open())
    {
        MyMessageBox box;
        box.iniMsgBox(MyMessageBox::error,QString("MYSQL打开失败！错误代码:%1").arg(db.lastError().text()));
        box.exec();
    }
    sql = QSqlQuery(db);
}


/**
* @brief 获取配置文件路径
* @date 2018/2/17
* @author han
* @param
* @return void
*/
QString Global::getAppConfigPath()
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

#ifdef Q_OS_WIN
    return appDataPath + "/config.ini";
#else
    return "config.ini";
#endif
}

/**
* @brief 获取教师id
* @date 2017/12/13
* @author han
* @return void
*/
QString Global::getTeacherID()
{
    return QString::number(teacherNum,10);
}


/**
* @brief 获取日志id
* @date 2018/1/6
* @author han
* @return void
*/
QString Global::getWorkLogID()
{
    return QString::number(workLogId,10);
}
/**
* @brief 写入日志
* @date 2017/12/13
* @author han
* @return void
*/
bool Global::writeLog(QString type, QString log)
{
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDateTime t = QDateTime::currentDateTime();
    QString time = t.toString("yyyy-MM-dd hh:mm:ss info:");
    QString path = appDataPath + "/Log/";

    QDir dir;
    dir.setPath(path);
    if(!dir.exists())  //目录不存在,创建新目录
    {
        if(!dir.mkpath(path))
        {
            return false;
        }
    }

    if(type == "error")
        path = path + "error.log";
    else
        path = path + "work.log";

    QFile f(path);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Append|QIODevice::Text))
    {
        qDebug() << "Open failed." ;
        return false;
    }
    QTextStream txtOutput(&f);
    txtOutput << time + log << endl;
    f.close();

    return true;
}

void Global::disconnectdb()
{
    db.close();
}

/**
* @brief Ftp加解密
* @date 2017/12/15
* @author han
* @return QString
*/
QString Global::getXorEncryptDecrypt(const QString &str, const char &key)
{
    QString result;
    QByteArray bs = qstringToByte(str);

    for(int i=0; i<bs.size(); i++){
        bs[i] = bs[i] ^ key;
    }

    result = byteToQString(bs);
    return result;
}


/**
* @brief 把QByteArray转换成QString
* @date 2017/12/15
* @author han
* @return QString
*/
QString Global::byteToQString(const QByteArray &byte)
{
    QString result;
    if(byte.size() > 0){
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        result = codec->toUnicode(byte);
    }

    return result;
}

QByteArray Global::qstringToByte(const QString &strInfo)
{
    QByteArray result;
    if(strInfo.length() > 0){
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        result = codec->fromUnicode(strInfo);
    }

    return result;
}


/**
* @brief MD5加密密码,将加密后的密码存到数据库，管理员密码也是如此，
* 能保证该密码只有自己知道，即便是管理员也无法查看教师自己修改后的密码
* @date 2018/1/10
* @author han
* @return void
*/
QString Global::EncriptPasswordByMD5(QString password)
{
    QByteArray bytePwd = password.toLatin1();
    QByteArray bytePwdMd5 = QCryptographicHash::hash(bytePwd, QCryptographicHash::Md5);
    return bytePwdMd5.toHex();
}


/**
* @brief QTableView 总页数
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @return void
*/
int Global::pageCount(QTableView *p)
{
    if(p == NULL || p->model()->rowCount() == 0)
        return 0;
    int rowCount = p->model()->rowCount();
    int rowHeight = p->rowHeight(0);
    int tableViewHeight = p->height();
    int rowCountPerPage = tableViewHeight/rowHeight-1;//每页显示行数
    int ret = rowCount/rowCountPerPage;
    int tem = rowCount%rowCountPerPage;
    if (tem != 0)  //最后一页
        ret++;
    return ret;

}

/**
* @brief QTableView 翻到指定页
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @return void
*/
BOOL Global::pageTo(QTableView *p, int pageNO)
{
    if(p == NULL|| p->model()->rowCount() == 0)
        return false;
    int maxPage = pageCount(p);

    if(pageNO > maxPage)  //页码大于最大页数
        return false;

    int rowCount = p->model()->rowCount();
    int rowHeight = p->rowHeight(0);
    int tableViewHeight = p->height();
    int rowCountPerPage = tableViewHeight/rowHeight-1;		//每页显示行数
    int canNotViewCount = rowCount-rowCountPerPage;			//看不见的行数
    if(canNotViewCount == 0)
        return false;
    int maxValue = p->verticalScrollBar()->maximum();		// 当前SCROLLER最大显示值
    if(maxValue == 0)
        return false;
    int pageValue = (maxValue*rowCountPerPage)/canNotViewCount;
    p->verticalScrollBar()->setSliderPosition(pageValue*(pageNO-1));  //滚动

    return true;
}


/**
* @brief QTableView,上一页
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @param isLoop 是否循环翻页
* @return void
*/
BOOL Global::pageUp(QTableView *p,BOOL isLoop)//上翻
{
    if(p == NULL|| p->model()->rowCount() == 0)  return false;

    int rowCount = p->model()->rowCount();

    int rowHeight = p->rowHeight(0);

    int tableViewHeight = p->height();

    int rowCountPerPage = tableViewHeight/rowHeight-1;		//每页显示行数

    int canNotViewCount = rowCount-rowCountPerPage;			//看不见的行数

    if(canNotViewCount == 0)
        return false;

    int maxValue = p->verticalScrollBar()->maximum();		// 当前SCROLLER最大显示值
    if(maxValue == 0) return false;
    int pageValue = (maxValue*rowCountPerPage)/canNotViewCount;
    int nCurScroller = p->verticalScrollBar()->value();		//获得当前scroller值
    if(nCurScroller>0)
        p->verticalScrollBar()->setSliderPosition(nCurScroller-pageValue);
    else
    {
        if(isLoop == TRUE)
            p->verticalScrollBar()->setSliderPosition(maxValue);
    }

    return true;
}


/**
* @brief 下一页
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @param isLoop 是否循环翻页
* @return void
*/
BOOL Global::pageDown(QTableView *p,BOOL isLoop)//下翻
{
    if(p == NULL|| p->model()->rowCount() == 0)
        return false;

    int rowCount = p->model()->rowCount();
    int rowHeight = p->rowHeight(0);
    int tableViewHeight = p->height();
    int rowCountPerPage = tableViewHeight/rowHeight-1;		//每页显示行数
    int canNotViewCount = rowCount-rowCountPerPage;			//看不见的行数

    if(canNotViewCount == 0)
        return false;
    int maxValue = p->verticalScrollBar()->maximum();		// 当前SCROLLER最大显示值

    if(maxValue == 0)
        return false;
    int pageValue = (maxValue*rowCountPerPage)/canNotViewCount;
    int nCurScroller = p->verticalScrollBar()->value();		//获得当前scroller值

    if(nCurScroller<maxValue)
        p->verticalScrollBar()->setSliderPosition(nCurScroller+pageValue);
    else  //最后一页
    {
        if(isLoop == TRUE)  //翻到首页
            p->verticalScrollBar()->setSliderPosition(0);
    }

    return true;
}


/**
* @brief 首页
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @param
* @return void
*/
BOOL Global::pageHome(QTableView *p)
{
    if(p == NULL|| p->model()->rowCount() == 0)  return false;
    int maxValue = p->verticalScrollBar()->maximum(); // 当前SCROLLER最大显示值
    if(maxValue == 0)  return false;
    p->verticalScrollBar()->setSliderPosition(0);
    return true;
}


/**
* @brief 最后一页
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @param
* @return void
*/
BOOL Global::pageEnd(QTableView *p)
{
    if(p == NULL|| p->model()->rowCount() == 0)
        return false;
    int maxValue = p->verticalScrollBar()->maximum(); // 当前SCROLLER最大显示值
    if(maxValue == 0)
        return false ;
    p->verticalScrollBar()->setSliderPosition(maxValue);
    return true;
}




/**
* @brief QTableWidget 总页数
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @return void
*/
int Global::pageCount(QTableWidget *p)
{
    if(p == NULL || p->model()->rowCount() == 0)
        return 0;
    int rowCount = p->model()->rowCount();
    int rowHeight = p->rowHeight(0);
    int tableViewHeight = p->height();
    int rowCountPerPage = tableViewHeight/rowHeight-1;//每页显示行数
    int ret = rowCount/rowCountPerPage;
    int tem = rowCount%rowCountPerPage;
    if (tem != 0)  //最后一页
        ret++;
    return ret;

}

/**
* @brief QTableWidget 翻到指定页
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @return void
*/
BOOL Global::pageTo(QTableWidget *p, int pageNO)
{
    if(p == NULL|| p->model()->rowCount() == 0)
        return false;
    int maxPage = pageCount(p);

    if(pageNO > maxPage)  //页码大于最大页数
        return false;

    int rowCount = p->model()->rowCount();
    int rowHeight = p->rowHeight(0);
    int tableViewHeight = p->height();
    int rowCountPerPage = tableViewHeight/rowHeight-1;		//每页显示行数
    int canNotViewCount = rowCount-rowCountPerPage;			//看不见的行数
    if(canNotViewCount == 0)
        return false;
    int maxValue = p->verticalScrollBar()->maximum();		// 当前SCROLLER最大显示值
    if(maxValue == 0)
        return false;
    int pageValue = (maxValue*rowCountPerPage)/canNotViewCount;
    p->verticalScrollBar()->setSliderPosition(pageValue*(pageNO-1));  //滚动

    return true;
}


/**
* @brief QTableWidget,上一页
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @param isLoop 是否循环翻页
* @return void
*/
BOOL Global::pageUp(QTableWidget *p,BOOL isLoop)//上翻
{
    if(p == NULL|| p->model()->rowCount() == 0)  return false;

    int rowCount = p->model()->rowCount();

    int rowHeight = p->rowHeight(0);

    int tableViewHeight = p->height();

    int rowCountPerPage = tableViewHeight/rowHeight-1;		//每页显示行数

    int canNotViewCount = rowCount-rowCountPerPage;			//看不见的行数

    if(canNotViewCount == 0)
        return false;

    int maxValue = p->verticalScrollBar()->maximum();		// 当前SCROLLER最大显示值
    if(maxValue == 0) return false;
    int pageValue = (maxValue*rowCountPerPage)/canNotViewCount;
    int nCurScroller = p->verticalScrollBar()->value();		//获得当前scroller值
    if(nCurScroller>0)
        p->verticalScrollBar()->setSliderPosition(nCurScroller-pageValue);
    else
    {
        if(isLoop == TRUE)
            p->verticalScrollBar()->setSliderPosition(maxValue);
    }

    return true;
}


/**
* @brief 下一页
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @param isLoop 是否循环翻页
* @return void
*/
BOOL Global::pageDown(QTableWidget *p,BOOL isLoop)//下翻
{
    if(p == NULL|| p->model()->rowCount() == 0)
        return false;

    int rowCount = p->model()->rowCount();
    int rowHeight = p->rowHeight(0);
    int tableViewHeight = p->height();
    int rowCountPerPage = tableViewHeight/rowHeight-1;		//每页显示行数
    int canNotViewCount = rowCount-rowCountPerPage;			//看不见的行数

    if(canNotViewCount == 0)
        return false;
    int maxValue = p->verticalScrollBar()->maximum();		// 当前SCROLLER最大显示值

    if(maxValue == 0)
        return false;
    int pageValue = (maxValue*rowCountPerPage)/canNotViewCount;
    int nCurScroller = p->verticalScrollBar()->value();		//获得当前scroller值

    if(nCurScroller<maxValue)
        p->verticalScrollBar()->setSliderPosition(nCurScroller+pageValue);
    else  //最后一页
    {
        if(isLoop == TRUE)  //翻到首页
            p->verticalScrollBar()->setSliderPosition(0);
    }

    return true;
}


/**
* @brief 首页
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @param
* @return void
*/
BOOL Global::pageHome(QTableWidget *p)
{
    if(p == NULL|| p->model()->rowCount() == 0)  return false;
    int maxValue = p->verticalScrollBar()->maximum(); // 当前SCROLLER最大显示值
    if(maxValue == 0)  return false;
    p->verticalScrollBar()->setSliderPosition(0);
    return true;
}

/**
* @brief 最后一页
* @date 2018/1/10
* @author http://blog.csdn.net/liuguangzhou123/article/details/8109089
* @param
* @return void
*/
BOOL Global::pageEnd(QTableWidget *p)
{
    if(p == NULL|| p->model()->rowCount() == 0)
        return false;
    int maxValue = p->verticalScrollBar()->maximum(); // 当前SCROLLER最大显示值
    if(maxValue == 0)
        return false ;
    p->verticalScrollBar()->setSliderPosition(maxValue);
    return true;
}


