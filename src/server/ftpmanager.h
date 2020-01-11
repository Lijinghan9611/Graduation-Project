#ifndef FTPMANAGER_H
#define FTPMANAGER_H

/**
* @brief Ftp文件上传，支持大文件
* @author HAN
* @date 2017-11-8
*/
#include <QObject>
#include <QString>
#include <QDebug>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QFile>
#include <QByteArray>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

class FtpManager
{
public:
    FtpManager(QString rftpUsesrname,QString rftpUserPwd,QString rftpIP,int rftpPort = 21);  //账号登录
    QString uploadFile(QString path_fileName,QString ftpFileName,QString teacherName = "");
    QNetworkReply *getRelay();
private:
    QFile *sourceFile;
    QByteArray by_txt;
    QNetworkAccessManager *manager;
    QUrl *ftpUrl;
    QString ftpIP;
    QString ftpUsesrname;
    int ftpPort;
    QString ftpUserPwd;
    QNetworkReply *replyOrg;
};





#endif // FTPMANAGER_H
