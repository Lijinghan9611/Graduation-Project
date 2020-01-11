#include "ftpmanager.h"
#include <QException>
/**
* @brief 账号登录,初始化
* @author HAN
* @date 2017-11-8
*/
FtpManager::FtpManager(QString rftpUsesrname,QString rftpUserPwd,QString rftpIP,int rftpPort)
{
    ftpIP =  rftpIP;
    ftpUsesrname = rftpUsesrname;
    ftpPort = rftpPort;
    ftpUserPwd = rftpUserPwd;

}

/**
* @brief 利用QNetworkAccessManager上传文件到ftp
* @author HAN
* @date 2017-11-8
* @author fileName 上传的文件，包含路径
* @author ftpFileName 服务器上的路径
* @return
*/
QString FtpManager::uploadFile(QString path_fileName, QString ftpFileName, QString teacherName)
{
    if(ftpIP == "")
    {
        return "请设置ftp服务器信息";
    }
    try{
        sourceFile = new QFile(path_fileName);      //设置文件路径
        sourceFile->open(QIODevice::ReadOnly);  //读取模式
        //by_txt=sourceFile->readAll();   //全部读取,遇到大文件时会内存溢出
        //sourceFile->close();            //关闭文件
        QString serverPath = "";  //服务器上的路径
        if(teacherName != "")
            serverPath = teacherName + "/" + ftpFileName;
        else
            serverPath = ftpFileName;
        manager = new QNetworkAccessManager;
        ftpUrl = new QUrl();  //设置QUrl数据 QString("ftp://") +ftpIP + "/" + ftpFileName
        ftpUrl->setScheme("ftp");//设置该计划描述了URL的类型（或协议）
        ftpUrl->setHost(ftpIP);//设置主机地址
        ftpUrl->setPort(ftpPort);           //端口
        ftpUrl->setUserName(ftpUsesrname);  //ftp用户名
        ftpUrl->setPassword(ftpUserPwd);   //ftp密码
        ftpUrl->setPath(serverPath);//设置URL路径。该路径是自带权限后的URL的一部分，但在查询字符串之前

        replyOrg = manager->put(QNetworkRequest(*ftpUrl), sourceFile);  //上传
    }catch(QException e)
    {
        return "错误："+QString(e.what());
    }

    return "success";
}


/**
* @brief 获取上传得到的QNetworkReply，用于进度条显示、获取上传结果等
* @date 2017/12/15
* @author han
* @return void
*/
QNetworkReply *FtpManager::getRelay()
{
    return replyOrg;
}




























