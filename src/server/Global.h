#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#define UDP_MAXSIZE 1300
#define SERVER_PORT 9611

#define WM_Pic 0x400
#define WM_Word 0x401
#define WM_Common 0x402
#define WM_Mornitor 0x403
#define WM_Camera 0x404
#define WM_Leave 0x405
#define WM_Position 0x406
#define WM_Zoom 0x407
#define WM_ZoomEnd 0x408
#define WM_HostMg 0x409
#define WM_BeginSign 0x410
#define WM_RetrieveSign 0x411
#define WM_SignMg 0x412
#define WM_Question 0x413
#define WM_GetAnswer 0x414
#define WM_AnswerMg 0x415
#define WM_Cmd 0x416
#define WM_CmdResult 0x417
#define WM_CmdCallBack 0x418
#define WM_BrocastBegin 0x419
#define WM_BrocastEnd 0x420
#define WM_CloseComputer 0x421
#define WM_RegetPosition 0x422
#define WM_ReSendPosition 0x423
#define WM_ReStartComputer 0x424
#define WM_EndProtect 0x425
#define WM_CheckClientAttribute 0x426
#define WM_ClientAttribute 0x427
#define WM_TransportFile 0x428
#define WM_TransportFileCallBack 0x429
#define WM_IniPortrait 0x430
#define WM_BeginComparePortrait 0x431
#define WM_SendFileStart 0x432
#define WM_SendFileEnd 0x433
#define WM_RecieveFileStart 0x434
#define WM_RecieveFileEnd 0x435
#define WM_BrocastPic 0x888

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSettings>
#include <QSqlError>
#include <QTextCodec>
#include "ExcelManager.h"
#include <QTableView>
#include <QScrollBar>
#include <QTableWidget>
#include "stoast.h"
class Global
{
public:
    Global();
    void connectdb();
    void disconnectdb();
    static int hostNum;  //在线主机数
    static long long teacherNum;
    static bool isBroadcast;
    static int style;
    static QString skinFileName;  //皮肤
    static bool isAdmin;
    static int week; //周次
    static int workLogId;  //保存当前工作记录的id
    static int AttendanceTime;//保存当前考勤次数
    static QString LocalIP; //本机ip
    static bool shutdownAutoGetIp; //是否关闭自动获取ip
    static bool isReceivingFile;  //是否正在接收文件

    static int LoginType;

    const static int TYPE1 = 1;  //“实验课程”或“技能训练”
    const static int TYPE2 = 2;  //“理论授课”
    const static int TYPE3 = 3;  //其他选项

    const int ADVICEUNSOLVED = 0;  //建议未解决
    const int ADVICESOLVED = 1;    //建议已解决
    const int REPAIRUNSOLVED = 0;  //维修未解决
    const int REPAIRSOLVED = 1;    //维修已解决


    static bool writeLog(QString type, QString log);
    QString getTeacherID();
    QString getWorkLogID();
    static QString getXorEncryptDecrypt(const QString &, const char &);
    static QString byteToQString(const QByteArray &byte);
    static QByteArray qstringToByte(const QString &strInfo);

    int pageCount(QTableView *p);//QTableView 总页数
    BOOL pageTo(QTableView *p, int pageNO);//翻到指定页
    BOOL pageUp(QTableView *p,BOOL isLoop);//上翻
    BOOL pageDown(QTableView *p,BOOL isLoop);//下翻
    BOOL pageHome(QTableView *p);//首页
    BOOL pageEnd(QTableView *p);//末页

    int pageCount(QTableWidget *p);//QTableWidget 总页数
    BOOL pageTo(QTableWidget *p, int pageNO);//翻到指定页
    BOOL pageUp(QTableWidget *p,BOOL isLoop);//上翻
    BOOL pageDown(QTableWidget *p,BOOL isLoop);//下翻
    BOOL pageHome(QTableWidget *p);//首页
    BOOL pageEnd(QTableWidget *p);//末页

public:
    QSqlQuery sql;
    QSqlDatabase db;
    QString EncriptPasswordByMD5(QString password); //MD5加密密码
    static QString getAppConfigPath();
};

#endif // GLOBAL_H
