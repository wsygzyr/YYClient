#include "HttpUpLoad.h"

#include <Modules/JsonPrase/cJSON.h>
#include <stdlib.h>

HttpUpLoad::HttpUpLoad(QObject *parent) :
    QObject(parent)
{
    dumpMultiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    nodeNC = NodeNotificationCenter::getInstance();
    stateCtrl = StateController::getInstance();
    m_sPath = ConfigHelper::getInstance()->getPluginPath() + "log/";
    connect(stateCtrl,SIGNAL(onStateUploadDumpFile()),this,SLOT(handleStateControllerUploadDumpFile()));
    connect(stateCtrl, SIGNAL(onStateUploadLog()), this, SLOT(handleStateControllerUploadLog()));
}

/* notice:
 * zipFile(QString logFileName) first should set logFileName in main.cpp
 * zipFile(QString logFileName) func should set fputs("C:\\iHouLog\\core.dmp", listp);
 *
 *
 */


void HttpUpLoad::zipDumpFile(QString path, QString coreFile, QString logFile )
{

    HttpUploadDebug("WR dump file");
    /*
    QString name = ConfigHelper::getInstance()->getPluginPath() + "log/error.zip";
    QFile fileError(name);
    if(fileError.exists())
    {
        fileError.remove();
    }
    */

    //if ihoulog.txt and core.dmp not exist,return
    QString logFileName = path + logFile;
    uploadDumpFile = QString("%1error%2.zip").arg(path).arg(PkInfoManager::getInstance()->getMe().getID());
    QString timeStr = coreFile.left(37);

    FILE *inp = fopen(logFileName.toStdString().c_str(), "rb");
    FILE *outp = fopen(QString(path + timeStr + "log.txt").toStdString().c_str(), "wb");
    char *buffer;
    long inSize_ALL, inSize_PART, inSize_PART1, result;

    if (inp && outp) {
        fseek(inp, 0, SEEK_END);
        inSize_ALL = ftell(inp);
        rewind(inp);
        if (inSize_ALL > 2 * 1024 * 1024)
        {
            fseek(inp, -2 * 1024 * 1024, SEEK_END);
            inSize_PART1 = ftell(inp);
            inSize_PART = inSize_ALL - inSize_PART1;
        }
        else
        {
            fseek(inp , 0 ,SEEK_SET);
            inSize_PART = inSize_ALL;
        }

        buffer = (char *)malloc(sizeof(char) * inSize_PART);
        if (NULL == buffer) {
            HttpUploadDebug("MM Error");
            return;
        }

        //std::cout << inSize_PART << std::endl;
        long count = 0;
        while (result = fread(buffer, 1, inSize_PART, inp)) {
            if (result != fwrite(buffer, 1, result, outp)) {
                HttpUploadDebug("WR Error");
                return;
            }
            count += result;
        }
    }


    FILE *listp = fopen(QString("%1errorlist%2.txt").arg(path).arg(PkInfoManager::getInstance()->getMe().getID()).toStdString().c_str(), "w");
    if (listp) {
        fputs(QString("\"" + path + timeStr + "log.txt\"\n").toUtf8().data(), listp);
        fputs(QString("\"" + path + coreFile +"\"\n").toUtf8().data(), listp);
    }
    else
    {
        HttpUploadDebug("errorlist name is %s", QString("%1errorlist%2.txt").arg(path).arg(PkInfoManager::getInstance()->getMe().getID()).toStdString().c_str());
    }


    if(inp)
        fclose(inp);
    if(outp)
        fclose(outp);
    if(listp)
        fclose(listp);
    free(buffer);

    m_aFileList.push_back(QString("log/"+ timeStr + "log.txt"));
    m_aFileList.push_back(QString("log/" + coreFile));

    //    cmd.setNativeArguments(QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=error%3.zip").arg(QString("%1errorlist%2.txt").arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID())).arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID()));
    cmd.setNativeArguments(QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=error%3.zip").arg(QString("%1errorlist%2.txt").arg("log/").arg(PkInfoManager::getInstance()->getMe().getID())).arg("log/").arg(PkInfoManager::getInstance()->getMe().getID()));
    cmd.setWorkingDirectory(ConfigHelper::getInstance()->getPluginPath());
    cmd.start("makecab.exe");
    //    cmd.start("cmd", QStringList()<<"makecab.exe"<<"/s"<<QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=error.zip").arg(m_sPath + "list.txt").arg("D:/"));
    cmd.waitForStarted();
    cmd.waitForFinished();
    HttpUploadDebug("makecab.exe %s", QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=error%3.zip").arg(QString("%1errorlist.txt").arg("log/")).arg("log/").arg(PkInfoManager::getInstance()->getMe().getID()).toStdString().c_str());

}

void HttpUpLoad::zipLogFile(QString path, QString logFile)
{
    HttpUploadDebug("WR log file");

    QString logFileName = path + logFile;
    uploadLogFile = QString("%1log%2.zip").arg(path).arg(PkInfoManager::getInstance()->getMe().getID());
    HttpUploadDebug("uploadLogFile is %s", uploadLogFile.toStdString().c_str());

    FILE *inp = fopen(logFileName.toStdString().c_str(), "rb");
    FILE *outp = fopen(QString(path + ConfigHelper::getInstance()->getLogFileName() + ".txt").toStdString().c_str(), "wb");
    char *buffer;
    long inSize_ALL, inSize_PART, inSize_PART1, result;

    if (inp && outp) {
        fseek(inp, 0, SEEK_END);
        inSize_ALL = ftell(inp);
        rewind(inp);
        if (inSize_ALL > 2 * 1024 * 1024)
        {
            fseek(inp, -2 * 1024 * 1024, SEEK_END);
            inSize_PART1 = ftell(inp);
            inSize_PART = inSize_ALL - inSize_PART1;
        }
        else
        {
            fseek(inp , 0 ,SEEK_SET);
            inSize_PART = inSize_ALL;
        }

        buffer = (char *)malloc(sizeof(char) * inSize_PART);
        if (NULL == buffer) {
            HttpUploadDebug("MM Error");
            return;
        }

        //std::cout << inSize_PART << std::endl;
        long count = 0;
        while (result = fread(buffer, 1, inSize_PART, inp))
        {
            if (result != fwrite(buffer, 1, result, outp))
            {
                HttpUploadDebug("WR Error");
                return;
            }
            count += result;
        }
    }


    FILE *listp = fopen(QString("%1loglist%2.txt").arg(path).arg(PkInfoManager::getInstance()->getMe().getID()).toStdString().c_str(), "w");;
    if (listp)
    {
        fputs(QString("\"" + path + ConfigHelper::getInstance()->getLogFileName() + ".txt\"\n").toStdString().c_str(), listp);
    }

    if(inp)
        fclose(inp);
    if(outp)
        fclose(outp);
    if(listp)
        fclose(listp);
    free(buffer);

//    m_aFileList.push_back(QString("log/"+ timeStr + "log.txt"));
//    m_aFileList.push_back(QString("log/" + coreFile));


}


void HttpUpLoad::setDumpHeaderPart()
{
    HttpUploadDebug("setDumpHeaderPart...");

    //userid
    QHttpPart useridPart;
    useridPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    useridPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"userID\""));

    useridPart.setBody(QByteArray(QString("%1").arg(PkInfoManager::getInstance()->getMe().getID()).toLocal8Bit().data()));
    dumpMultiPart->append(useridPart);

    //roomid
    QHttpPart roomidPart;
    roomidPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    roomidPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"roomId\""));

    roomidPart.setBody(QByteArray(QString("%1").arg(PkInfoManager::getInstance()->getChannelID()).toLocal8Bit().data()));
    dumpMultiPart->append(roomidPart);

    //errorCode
    QHttpPart errorCodePart;
    errorCodePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    errorCodePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"errorCode\""));

    errorCodePart.setBody(QByteArray(""));
    dumpMultiPart->append(errorCodePart);

    //dump file
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,  QVariant("form-data; name=\"dump\"; filename=\"dump.zip\""));
    QFile *file = new QFile(uploadDumpFile, this);

    if(!file->exists())
    {
        HttpUploadDebug("log file can not find, %s", uploadDumpFile.toUtf8().data());
        delete file;
        file =NULL;
        return ;
    }

    file->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(file);
    file->setParent(dumpMultiPart); // we cannot delete the file now, so delete it with the multiPart
    dumpMultiPart->append(filePart);
}

void HttpUpLoad::setLogHeaderPart()
{
    HttpUploadDebug("setLogHeaderPart...");

    logMultiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //userid
    QHttpPart useridPart;
    useridPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    useridPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"userID\""));

    useridPart.setBody(QByteArray(QString("%1").arg(PkInfoManager::getInstance()->getMe().getID()).toLocal8Bit().data()));
    logMultiPart->append(useridPart);

    //roomid
    QHttpPart roomidPart;
    roomidPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    roomidPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"roomId\""));

    roomidPart.setBody(QByteArray(QString("%1").arg(PkInfoManager::getInstance()->getChannelID()).toLocal8Bit().data()));
    logMultiPart->append(roomidPart);

    //errorCode
    QHttpPart errorCodePart;
    errorCodePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    errorCodePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"errorCode\""));

    errorCodePart.setBody(QByteArray(""));
    logMultiPart->append(errorCodePart);

    //log file
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,  QVariant("form-data; name=\"dump\"; filename=\"log.zip\""));
    QFile *file = new QFile(uploadLogFile, this);

    if(!file->exists())
    {
        HttpUploadDebug("log file can not find, %s", uploadLogFile.toUtf8().data());
        delete file;
        file =NULL;
        return ;
    }

    file->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(file);
    file->setParent(logMultiPart); // we cannot delete the file now, so delete it with the multiPart
    logMultiPart->append(filePart);
}

void HttpUpLoad::setSingleHeadPart(int errorCode)
{
    HttpUploadDebug("HttpUpLoad setSingleHeadPart...");

    SingleMultiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //userid
    QHttpPart useridPart;
    useridPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    useridPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"userID\""));

    useridPart.setBody(QByteArray(QString("%1").arg(PkInfoManager::getInstance()->getMe().getID()).toLocal8Bit().data()));
    SingleMultiPart->append(useridPart);

    //roomid
    QHttpPart roomidPart;
    roomidPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    roomidPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"roomId\""));

    roomidPart.setBody(QByteArray(QString("%1").arg(PkInfoManager::getInstance()->getChannelID()).toLocal8Bit().data()));
    SingleMultiPart->append(roomidPart);

    //errorCode
    QHttpPart errorCodePart;
    errorCodePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    errorCodePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"errorCode\""));

    errorCodePart.setBody(QByteArray(QString("%1").arg(errorCode).toLocal8Bit().data()));
    SingleMultiPart->append(errorCodePart);

    //log file
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("multipart/form-data"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,  QVariant("form-data; name=\"dump\"; filename=\"log.zip\""));
    QFile *file = new QFile(uploadLogFile, this);

    if(!file->exists())
    {
        HttpUploadDebug("log file can not find, %s", uploadLogFile.toUtf8().data());
        delete file;
        file =NULL;
        return ;
    }

    file->open(QIODevice::ReadOnly);
    filePart.setBodyDevice(file);
    file->setParent(SingleMultiPart); // we cannot delete the file now, so delete it with the multiPart
    SingleMultiPart->append(filePart);
}

void HttpUpLoad::setErrorHeadPart(int errorCode)
{
    HttpUploadDebug("HttpUpLoad setErrorHeadPart...");

    errorCodeMultiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    //userid
    QHttpPart useridPart;
    useridPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    useridPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"userID\""));

    useridPart.setBody(QByteArray(QString("%1").arg(PkInfoManager::getInstance()->getMe().getID()).toLocal8Bit().data()));
    errorCodeMultiPart->append(useridPart);

    //roomid
    QHttpPart roomidPart;
    roomidPart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    roomidPart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"roomId\""));

    roomidPart.setBody(QByteArray(QString("%1").arg(PkInfoManager::getInstance()->getChannelID()).toLocal8Bit().data()));
    errorCodeMultiPart->append(roomidPart);

    //errorCode
    QHttpPart errorCodePart;
    errorCodePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    errorCodePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"errorCode\""));

    errorCodePart.setBody(QByteArray(QString("%1").arg(errorCode).toLocal8Bit().data()));
    errorCodeMultiPart->append(errorCodePart);
}

void HttpUpLoad::handleStateControllerUploadDumpFile()
{
    HttpUploadDebug("upload dump file ... %s", uploadDumpFile.toUtf8().data());

    /*
     *  cipher = DES（1970-01-01T00:00:00后的毫秒数 + “_”+ GUID + “_”+ “iHouPlugin!”）
     */
    qint64 millisecond = QDateTime::currentMSecsSinceEpoch(); + PkInfoManager::getInstance()->getMillisecondsDiff();
    QString plain = QString::number(millisecond) + "_" + QUuid::createUuid().toString() + "_" + "iHouPlugin!";
    HttpUploadDebug("plain is: %s", plain.toUtf8().data());
    char cipherBuf[1024];
    int cipherLen = DES_Encrypt(plain.toUtf8().data(), cipherBuf, "ihouihou");
    QString cipher = QString(QByteArray(cipherBuf, cipherLen));
    HttpUploadDebug("cipher is: %s", cipher.toUtf8().data());

    QUrl url(ConfigHelper::getInstance()->getUploadPrefix());

    QNetworkRequest request(url);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    request.setRawHeader(QString("cipher").toUtf8(),cipher.toUtf8());

    manager->post(request, dumpMultiPart);
    dumpMultiPart->setParent(this); // delete the multiPart with the reply

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleThisOnDumpFinish(QNetworkReply*)));
}

void HttpUpLoad::handleStateControllerUploadLog()
{
    HttpUploadDebug("HttpUpLoad handleThisOnFinish");
//    QString args = QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=log%3.zip").arg(QString("%1loglist%2.txt").arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID())).arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID());
    QString args = QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=log%3.zip").arg(QString("%1loglist%2.txt").arg("log/").arg(PkInfoManager::getInstance()->getMe().getID())).arg("log/").arg(PkInfoManager::getInstance()->getMe().getID());
    cmd.setNativeArguments(args);
    HttpUploadDebug("args is %s", args.toStdString().c_str());
//    cmd.setNativeArguments(QString("/f C:/Users/cychen3/AppData/Roaming/duowan/yy/business/yymusicconfig/4_5/2221/907/plugin/log/loglist1005967985.txt /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=C:/Users/cychen3/AppData/Roaming/duowan/yy/business/yymusicconfig/4_5/2221/907/plugin/log/ /d cabinetnametemplate=log1005967985.zip"));
    cmd.setWorkingDirectory(ConfigHelper::getInstance()->getPluginPath());
    cmd.start("makecab.exe");
    HttpUploadDebug("makecab.exe %s", QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=log%3.zip").arg(QString("%1loglist%2.txt").arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID())).arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID()).toStdString().c_str());
//    cmd.start("cmd", QStringList()<<"makecab.exe"<<"/s"<<QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=error.zip").arg(m_sPath + "list.txt").arg("D:/"));
    cmd.waitForStarted();
    cmd.waitForFinished();

    setLogHeaderPart();

    HttpUploadDebug("upload log file ... %s", uploadLogFile.toLocal8Bit().data());

    zipLogFile(ConfigHelper::getInstance()->getPluginPath() + "log/", ConfigHelper::getInstance()->getLogFileName());



    qint64 millisecond = QDateTime::currentMSecsSinceEpoch(); + PkInfoManager::getInstance()->getMillisecondsDiff();
    QString plain = QString::number(millisecond) + "_" + QUuid::createUuid().toString() + "_" + "iHouPlugin!";
    HttpUploadDebug("plain is: %s", plain.toUtf8().data());
    char cipherBuf[1024];
    int cipherLen = DES_Encrypt(plain.toUtf8().data(), cipherBuf, "ihouihou");
    QString cipher = QString(QByteArray(cipherBuf, cipherLen));
    HttpUploadDebug("cipher is: %s", cipher.toUtf8().data());

    QUrl url(ConfigHelper::getInstance()->getUploadPrefix());

    QNetworkRequest request(url);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    request.setRawHeader(QString("cipher").toUtf8(),cipher.toUtf8());

    QNetworkReply * reply = manager->post(request, logMultiPart);
    logMultiPart->setParent(reply); // delete the multiPart with the reply

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleThisOnLogFinish(QNetworkReply*)));

}

void HttpUpLoad::handleThisOnDumpFinish(QNetworkReply* reply)
{
    HttpUploadDebug("Dump file upload replay...");
    if(!reply)
    {
        HttpUploadDebug("HttpUpLoad handleThisOnFinish reply is NULL");
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    HttpUploadDebug("reply is %s" , replyByteArray.data());
    reply->deleteLater();
    reply = NULL;

    if(QFile::remove(ConfigHelper::getInstance()->getPluginPath() + "log/error.zip"))
    {
        QString temp = ConfigHelper::getInstance()->getPluginPath() + "log/error.zip";
    }



    cJSON *jsReply = cJSON_Parse(replyByteArray.data());
    {
        if (!jsReply)
        {
            HttpUploadDebug("error: parse JSON failed!");
            return;
        }

        cJSON *jsRetCode = cJSON_GetObjectItem(jsReply , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpUploadDebug("httpUpload dump upload get error,retcode is %s" , retCode.toUtf8().data());
            cJSON_Delete(jsReply);
            return;
        }
    }
	cJSON_Delete(jsReply);

    /*
    QString name = ConfigHelper::getInstance()->getPluginPath() + "log/" + coreFile;
    QFile file(name);
    if(file.exists())
    {
        file.remove();
    }
    */

    QFile::remove(ConfigHelper::getInstance()->getPluginPath() + "log/list.txt");
    for(int i=0; i<m_aFileList.size(); i++)
    {
        QString name = ConfigHelper::getInstance()->getPluginPath() + m_aFileList[i];
        QFile file(name);
        if(file.exists())
        {
            file.remove();
        }
    }

//    QFile::remove(ConfigHelper::getInstance()->getPluginPath() + "log/log.txt");
    m_aFileList.clear();

    HttpUploadDebug("remove dump success...");

//    this->deleteLater();
}

void HttpUpLoad::handleThisOnLogFinish(QNetworkReply* reply)
{
    HttpUploadDebug("Log file upload replay...");
    if(!reply)
    {
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    HttpUploadDebug("reply is %s" , replyByteArray.data());
    reply->deleteLater();
    reply = NULL;

    cJSON *jsReply = cJSON_Parse(replyByteArray.data());
    {
        if (!jsReply)
        {
            HttpUploadDebug("error: parse JSON failed!");
            return;
        }

        cJSON *jsRetCode = cJSON_GetObjectItem(jsReply , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpUploadDebug("httpUpload log upload get error,retcode is %s" , retCode.toUtf8().data());
            cJSON_Delete(jsReply);
            return;
        }
    }
    cJSON_Delete(jsReply);

    //QFile::remove(ConfigHelper::getInstance()->getPluginPath() + "log/log" + PkInfoManager::getInstance()->getMe().getID() +".zip");
   // QFile::remove(ConfigHelper::getInstance()->getPluginPath() + "log/list" + PkInfoManager::getInstance()->getMe().getID() +".txt");

//    this->deleteLater();
}




QStringList HttpUpLoad::fileDirXmlList(QString xmlName,QString Path) //文件list
{	//xmlName 文件类型 Path 路径

    QStringList fileList;
    QDir dir(Path);
    if (!dir.exists()) return fileList;
    dir.setFilter(QDir::Dirs|QDir::Files);
    //dir.setSorting(QDir::DirsFirst);
    dir.setSorting(QDir::Time |QDir::Reversed);
    //排序方式 修改时间从小到大
    QFileInfoList list = dir.entryInfoList();
    int i=0,filecont=0;
    do{
        QFileInfo fileInfo = list.at(i);

        if(fileInfo.fileName() == "." || fileInfo.fileName()== "..")
        {
            qDebug()<<"filedir="<<fileInfo.fileName();
            i++;
            continue;
        }

        bool bisDir=fileInfo.isDir();
        if(bisDir)
        { }
        else
        {

          QString currentFileName=fileInfo.fileName();
            bool Reght=currentFileName.endsWith(xmlName, Qt::CaseInsensitive);
            if(Reght)
            {
                fileList<<currentFileName;
                qDebug()<<"filelist sort="<<currentFileName;
                filecont++;
            }
        }


        i++;
    }while(i<list.size());
    return fileList;
}

void HttpUpLoad::dumpfileUpload()
{
    HttpUploadDebug("HttpUpLoad dumpfileUpload ... %s", uploadDumpFile.toUtf8().data());

    bool haveFile = false;

    //get dump file
    QStringList fileList = fileDirXmlList("", ConfigHelper::getInstance()->getPluginPath()+"log/");
    for(int i=0; i<fileList.size(); i++)
    {
      if(fileList[i].indexOf("core.dmp") != -1)
      {
          QString commonStr = fileList[i].left(37);
          for(int j=0; j<fileList.size(); j++)
          {
              if(fileList[j].indexOf(commonStr) != -1)
              {
                  HttpUploadDebug("has dump and log file");
                  zipDumpFile(ConfigHelper::getInstance()->getPluginPath()+"log/", fileList[i], fileList[j]);
                  HttpUploadDebug("start setDumpHeaderPart");
                  setDumpHeaderPart();
                  haveFile = true;
                  break;
              }
          }
      }
    }

    /*
     *  cipher = DES（1970-01-01T00:00:00后的毫秒数 + “_”+ GUID + “_”+ “iHouPlugin!”）
     */
    qint64 millisecond = QDateTime::currentMSecsSinceEpoch(); + PkInfoManager::getInstance()->getMillisecondsDiff();
    QString plain = QString::number(millisecond) + "_" + QUuid::createUuid().toString() + "_" + "iHouPlugin!";
    HttpUploadDebug("plain is: %s", plain.toUtf8().data());
    char cipherBuf[1024];
    int cipherLen = DES_Encrypt(plain.toUtf8().data(), cipherBuf, "ihouihou");
    QString cipher = QString(QByteArray(cipherBuf, cipherLen));
    HttpUploadDebug("cipher is: %s", cipher.toUtf8().data());

    QUrl url(ConfigHelper::getInstance()->getUploadPrefix());

    QNetworkRequest request(url);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    request.setRawHeader(QString("cipher").toUtf8(),cipher.toUtf8());

    manager->post(request, dumpMultiPart);
    dumpMultiPart->setParent(this); // delete the multiPart with the reply

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleThisOnDumpFinish(QNetworkReply*)));
}

void HttpUpLoad::broadcastUpload()
{
    HttpUploadDebug("HttpUpLoad broadcastUpload");
    QString args = QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=log%3.zip").arg(QString("%1loglist%2.txt").arg("log/").arg(PkInfoManager::getInstance()->getMe().getID())).arg("log/").arg(PkInfoManager::getInstance()->getMe().getID());
    cmd.setNativeArguments(args);
    HttpUploadDebug("args is %s", args.toStdString().c_str());
    cmd.setWorkingDirectory(ConfigHelper::getInstance()->getPluginPath());
    cmd.start("makecab.exe");
    HttpUploadDebug("makecab.exe %s", QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=log%3.zip").arg(QString("%1loglist%2.txt").arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID())).arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID()).toStdString().c_str());
    cmd.waitForStarted();
    cmd.waitForFinished();

    setLogHeaderPart();

    HttpUploadDebug("upload log file ... %s", uploadLogFile.toLocal8Bit().data());

    zipLogFile(ConfigHelper::getInstance()->getPluginPath() + "log/", ConfigHelper::getInstance()->getLogFileName());

    qint64 millisecond = QDateTime::currentMSecsSinceEpoch(); + PkInfoManager::getInstance()->getMillisecondsDiff();
    QString plain = QString::number(millisecond) + "_" + QUuid::createUuid().toString() + "_" + "iHouPlugin!";
    HttpUploadDebug("plain is: %s", plain.toUtf8().data());
    char cipherBuf[1024];
    int cipherLen = DES_Encrypt(plain.toUtf8().data(), cipherBuf, "ihouihou");
    QString cipher = QString(QByteArray(cipherBuf, cipherLen));
    HttpUploadDebug("cipher is: %s", cipher.toUtf8().data());

    QUrl url(ConfigHelper::getInstance()->getUploadPrefix());

    QNetworkRequest request(url);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    request.setRawHeader(QString("cipher").toUtf8(),cipher.toUtf8());

    QNetworkReply * reply = manager->post(request, logMultiPart);
    logMultiPart->setParent(reply); // delete the multiPart with the reply

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleThisOnLogFinish(QNetworkReply*)));
}

void HttpUpLoad::singleUpload(int errorCode)
{
    HttpUploadDebug("HttpUpLoad singleUpload, errorCode is:%d", errorCode);

    HttpUploadDebug("makecab.exe %s", QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=log%3.zip").arg(QString("%1loglist%2.txt").arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID())).arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID()).toStdString().c_str());

    zipLogFile(ConfigHelper::getInstance()->getPluginPath() + "log/", ConfigHelper::getInstance()->getLogFileName());

    QString args = QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=log%3.zip").arg(QString("%1loglist%2.txt").arg("log/").arg(PkInfoManager::getInstance()->getMe().getID())).arg("log/").arg(PkInfoManager::getInstance()->getMe().getID());
    cmd.setNativeArguments(args);
    HttpUploadDebug("args is %s", args.toStdString().c_str());
    cmd.setWorkingDirectory(ConfigHelper::getInstance()->getPluginPath());
    cmd.start("makecab.exe");
    HttpUploadDebug("makecab.exe %s", QString(" /f %1 /d compressiontype=mszip /d compressionmemory=21 /d maxdisksize=1024000000 /d diskdirectorytemplate=%2 /d cabinetnametemplate=log%3.zip").arg(QString("%1loglist%2.txt").arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID())).arg(m_sPath).arg(PkInfoManager::getInstance()->getMe().getID()).toStdString().c_str());
    cmd.waitForStarted();
    cmd.waitForFinished();

    setSingleHeadPart(errorCode);

    HttpUploadDebug("upload log file ... %s", uploadLogFile.toLocal8Bit().data());

    qint64 millisecond = QDateTime::currentMSecsSinceEpoch(); + PkInfoManager::getInstance()->getMillisecondsDiff();
    QString plain = QString::number(millisecond) + "_" + QUuid::createUuid().toString() + "_" + "iHouPlugin!";
    HttpUploadDebug("plain is: %s", plain.toUtf8().data());
    char cipherBuf[1024];
    int cipherLen = DES_Encrypt(plain.toUtf8().data(), cipherBuf, "ihouihou");
    QString cipher = QString(QByteArray(cipherBuf, cipherLen));
    HttpUploadDebug("cipher is: %s", cipher.toUtf8().data());

    QUrl url(ConfigHelper::getInstance()->getUploadPrefix());

    QNetworkRequest request(url);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    request.setRawHeader(QString("cipher").toUtf8(),cipher.toUtf8());

    QNetworkReply * reply = manager->post(request, SingleMultiPart);
    SingleMultiPart->setParent(reply); // delete the multiPart with the reply

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleThisOnSingleFinish(QNetworkReply*)));
}

void HttpUpLoad::errorUpload(int errorCode)
{
    HttpUploadDebug("HttpUpLoad errorUpload, errorCode is:%d", errorCode);

    setErrorHeadPart(errorCode);

    /*
     *  cipher = DES（1970-01-01T00:00:00后的毫秒数 + “_”+ GUID + “_”+ “iHouPlugin!”）
     */
    qint64 millisecond = QDateTime::currentMSecsSinceEpoch(); + PkInfoManager::getInstance()->getMillisecondsDiff();
    QString plain = QString::number(millisecond) + "_" + QUuid::createUuid().toString() + "_" + "iHouPlugin!";
    HttpUploadDebug("plain is: %s", plain.toUtf8().data());
    char cipherBuf[1024];
    int cipherLen = DES_Encrypt(plain.toUtf8().data(), cipherBuf, "ihouihou");
    QString cipher = QString(QByteArray(cipherBuf, cipherLen));
    HttpUploadDebug("cipher is: %s", cipher.toUtf8().data());

    QUrl url(ConfigHelper::getInstance()->getUploadPrefix());

    QNetworkRequest request(url);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    request.setRawHeader(QString("cipher").toUtf8(),cipher.toUtf8());

    QNetworkReply * reply = manager->post(request, errorCodeMultiPart);
    errorCodeMultiPart->setParent(reply); // delete the multiPart with the reply
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleThisOnErrorFinish(QNetworkReply*)));

}

void HttpUpLoad::handleThisOnSingleFinish(QNetworkReply *reply)
{
    HttpUploadDebug("Log file upload replay...");
    if(!reply)
    {
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    HttpUploadDebug("reply is %s" , replyByteArray.data());
    reply->deleteLater();
    reply = NULL;

    cJSON *jsReply = cJSON_Parse(replyByteArray.data());
    {
        if (!jsReply)
        {
            HttpUploadDebug("error: parse JSON failed!");
            return;
        }

        cJSON *jsRetCode = cJSON_GetObjectItem(jsReply , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpUploadDebug("httpUpload log upload get error,retcode is %s" , retCode.toUtf8().data());
            cJSON_Delete(jsReply);
            return;
        }
    }
    cJSON_Delete(jsReply);

    QFile::remove(ConfigHelper::getInstance()->getPluginPath() + "log/log" + PkInfoManager::getInstance()->getMe().getID() +".zip");
    QFile::remove(ConfigHelper::getInstance()->getPluginPath() + "log/list" + PkInfoManager::getInstance()->getMe().getID() +".txt");

}


void HttpUpLoad::handleThisOnErrorFinish(QNetworkReply *reply)
{
    HttpUploadDebug("Log file upload replay...");
    if(!reply)
    {
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    HttpUploadDebug("reply is %s" , replyByteArray.data());
    reply->deleteLater();
    reply = NULL;

    cJSON *jsReply = cJSON_Parse(replyByteArray.data());
    {
        if (!jsReply)
        {
            HttpUploadDebug("error: parse JSON failed!");
            return;
        }

        cJSON *jsRetCode = cJSON_GetObjectItem(jsReply , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpUploadDebug("httpUpload log upload get error,retcode is %s" , retCode.toUtf8().data());
            cJSON_Delete(jsReply);
            return;
        }
    }
    cJSON_Delete(jsReply);
}
