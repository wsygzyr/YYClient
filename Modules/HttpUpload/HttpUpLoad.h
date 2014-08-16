#ifndef HTTPUPLOAD_H
#define HTTPUPLOAD_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QFile>
#include <QHttpPart>
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/ihouAuth/des/DES.h"
#include <QThread>
#include <QProcess>
#include "Modules/StateController/StateController.h"


#define HTTPUPLOAD_DEBUG  1

#if HTTPUPLOAD_DEBUG
#define HttpUploadDebug(format, ...)  qDebug("%s, LINE: %d --->"format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define HttpUploadDebug   /\
/
#endif

class HttpUpLoad : public QObject
{
    Q_OBJECT
public:
    explicit HttpUpLoad(QObject *parent = 0);
private:
     QHttpMultiPart *dumpMultiPart;
     QHttpMultiPart *logMultiPart;
     QHttpMultiPart *SingleMultiPart;
     QHttpMultiPart *errorCodeMultiPart;
     QString uploadDumpFile;
     QString uploadLogFile;
     QString uploadSingleFile;
     NodeNotificationCenter *nodeNC;
     StateController    *stateCtrl;
     QStringList m_aFileList;
     QString m_sPath;
     QProcess cmd;


public:
     void zipDumpFile(QString path, QString coreFile, QString logFile);
     void zipLogFile(QString path, QString logFile);
     void setDumpHeaderPart();
     void setLogHeaderPart();
     void setSingleHeadPart(int errorCode);
     void setErrorHeadPart(int errorCode);
     //void
     QStringList fileDirXmlList(QString xmlName,QString Path);

     void dumpfileUpload();
     void broadcastUpload();
     void singleUpload(int errorCode);
     void errorUpload(int errorCode);
signals:

public slots:
    void handleThisOnDumpFinish(QNetworkReply *reply);
    void handleThisOnLogFinish(QNetworkReply *reply);
    void handleThisOnSingleFinish(QNetworkReply *reply);
    void handleThisOnErrorFinish(QNetworkReply *reply);
    void handleStateControllerUploadDumpFile();
    void handleStateControllerUploadLog();
};

#endif // HTTPUPLOAD_H
