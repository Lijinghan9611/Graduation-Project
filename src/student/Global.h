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

#include <QString>
#include <QStandardPaths>
class Global
{
public:
    static QString getAppConfigPath()
    {
        QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

#ifdef Q_OS_WIN
        return appDataPath + "/config.ini";
#else
        return "config.ini";
#endif
    }
};

#endif // GLOBAL_H



