#include "HttpDownload.h"
#include <QList>
#include <windows.h>

#define DOWNLOADTIMES 3

HttpDownload::HttpDownload()
{
    downloadInfoList = new QLinkedList<HttpDownloadInfo>;
    bDownloading = false;
    downLoadTimes = 0;
    m_blDownLoadError = false;


    thread = new QThread();


    this->moveToThread(thread);
    QObject::connect(thread, SIGNAL(started()), this, SLOT(handleThreadStarted()));

    httpStream = NULL;

    thread->start();
}

HttpDownload::~HttpDownload()
{
    if(thread)
    {
        thread->quit();
        thread->wait(100);
        delete thread;
        thread = 0;
    }

    delete downloadInfoList;
}

QAtomicPointer<HttpDownload> HttpDownload::instance;
QMutex HttpDownload::instanceMutex;
HttpDownload *HttpDownload::getInstance()
{
//    HttpDownloadDebug("getInstance!");
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            HttpDownloadDebug("create instance!");
            instance = new HttpDownload();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void HttpDownload::destroy()
{
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}

void HttpDownload::handleThreadStarted()
{
    startTimer(25);
    thread->setObjectName("httpDownloadThread-"+ QString::number((int)QThread::currentThreadId()));
}

void HttpDownload::timerEvent(QTimerEvent *event)
{
//    HttpDownloadDebug("onTimerEvent! thread name is: %s", QThread::currentThread()->objectName().toUtf8().data());

    if (bDownloading)
    {
        return;
    }

    if (0 == downloadInfoList->count())
    {
        return;
    }

    bDownloading = true;

    currentDownloadInfo = downloadInfoList->first();

    QString cacheFilePath = currentDownloadInfo.cacheFilePath;
    if (cacheFilePath != NULL)
    {
        QFile::remove(cacheFilePath);
        HttpDownloadDebug("file removed! %s", cacheFilePath.toUtf8().data());
    }

    QUrl url = currentDownloadInfo.url;
    HttpDownloadDebug("start new download process, url is: %s", url.toString().toUtf8().data());

    if (!httpStream)
    {
        httpStream = new QNetworkAccessManager();
//        QObject::connect(QThread::currentThread(), SIGNAL(finished()), httpStream, SLOT(deleteLater()));
    }
    httpReply = httpStream->get(QNetworkRequest(url));

    connect(httpStream, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(handleSSLError(QNetworkReply*,QList<QSslError>)));
//    connect(httpReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(handleHttpProgress(qint64, qint64)));
    connect(httpReply, SIGNAL(readyRead()), this, SLOT(handleHttpProgress()));
    connect(httpReply, SIGNAL(finished()), this, SLOT(handleHttpDone()));
    connect(httpReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(handleHttpError(QNetworkReply::NetworkError)));

}


void HttpDownload::handleHttpProgress(/*qint64 bytesRead, qint64 totalBytes*/)
{
//    QVariant var1 = httpReply->header(QNetworkRequest::ContentTypeHeader);
//    QVariant var2 = httpReply->header(QNetworkRequest::LocationHeader);

//    HttpDownloadDebug("var1 header is: %s", var1.toString().toUtf8().data());
//    HttpDownloadDebug("var2 header is: %s", var2.toString().toUtf8().data());
//     QList<QByteArray> headerList = httpReply->rawHeaderList();



    if (0 == currentDownloadInfo.fileSize)
    {
        QVariant var = httpReply->header(QNetworkRequest::ContentLengthHeader);
        currentDownloadInfo.fileSize = var.toLongLong();
        HttpDownloadDebug("file size is: %d", currentDownloadInfo.fileSize);
        emit onFileSize(currentDownloadInfo.keyword, currentDownloadInfo.fileSize);
    }

    QByteArray data = httpReply->readAll();

    HttpDownloadDebug("readLen is: %d", data.length());
    currentDownloadInfo.allBytesRead += data.length();
    HttpDownloadDebug("allBytesRead is: %d", currentDownloadInfo.allBytesRead);
    QString cacheFilePath = currentDownloadInfo.cacheFilePath;
    emit onProgress(currentDownloadInfo.keyword, data, currentDownloadInfo.allBytesRead);


    HttpDownloadDebug("cacheFilePath is: %s", cacheFilePath.toUtf8().data());

}

void HttpDownload::handleHttpDone()
{
    HttpDownloadDebug("downloading finished! allBytesRead %d", currentDownloadInfo.fileSize);

    QList<QNetworkReply::RawHeaderPair> headerPair = httpReply->rawHeaderPairs();
    HttpDownloadDebug("http header is:");
    for(int i=0; i<headerPair.size(); i++)
    {
        QNetworkReply::RawHeaderPair temp = headerPair[i];
        HttpDownloadDebug("%s : %s",temp.first.data(), temp.second.data());
    }

    int http_status = httpReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    HttpDownloadDebug("http attribute is: %d", http_status);
/*    if (http_status == 302 || http_status==301)
    {
        HttpDownloadDebug("http attribute is: %d", http_status);
    }*/


    if(m_blDownLoadError == true && downLoadTimes < DOWNLOADTIMES)
    {
        HttpDownloadDebug("downloading failed! downLoadTimes:%d", downLoadTimes);
        m_blDownLoadError = false;
        httpReply->deleteLater();
        downLoadTimes++;
        bDownloading = false;
        emit onClearData(currentDownloadInfo.keyword);
        Sleep(1000);
        return;
    }

    if(0 == currentDownloadInfo.allBytesRead && downLoadTimes < DOWNLOADTIMES)
    {
        HttpDownloadDebug("downloading failed allBytesRead == 0! downLoadTimes:%d", downLoadTimes);
        m_blDownLoadError = false;
        httpReply->deleteLater();
        downLoadTimes++;
        bDownloading = false;
        emit onClearData(currentDownloadInfo.keyword);
        Sleep(1000);
        return;
    }


    httpReply->deleteLater();
    downloadInfoList->erase(downloadInfoList->begin());
    if((DOWNLOADTIMES == downLoadTimes && m_blDownLoadError == true) || (DOWNLOADTIMES == downLoadTimes && currentDownloadInfo.allBytesRead == 0))
    {
        emit onFinish(currentDownloadInfo.keyword, true,currentDownloadInfo.allBytesRead);
    }
    else
    {
        emit onFinish(currentDownloadInfo.keyword, false,currentDownloadInfo.allBytesRead);
    }

    downLoadTimes = 0;
    m_blDownLoadError = false;
    bDownloading = false;
}

/*
int HttpDownload::JudgeFileType(QString fileName)
{
    if(fileName.indexOf(".section.xml") != -1)
    {
        return 1;
    }
    else if(fileName.indexOf(".xml") != -1)
    {
        return 2;
    }
    else if(fileName.indexOf(".mp3") != -1)
    {
        return 3;
    }
    else
    {
        return 0;
    }
}
*/

void HttpDownload::handleHttpError(QNetworkReply::NetworkError errorCode)
{
    HttpDownloadDebug("error is: %d", errorCode);
    m_blDownLoadError = true;

 //   httpReply->deleteLater();
 //   downloadInfoList->erase(downloadInfoList->begin());
 //   downLoadTimes = 0;
 //   emit onFinish(currentDownloadInfo.keyword, true , 0);

 //   bDownloading = false;

}

void HttpDownload::handleSSLError(QNetworkReply *networkReply, QList<QSslError> errorList)
{
    HttpDownloadDebug("SSL error is");

    for(int i=0; i<errorList.size(); i++)
    {
       HttpDownloadDebug("SSLError is: %d", errorList[i]);
    }
    networkReply->ignoreSslErrors();
}


/*
 *  Interface
 */
bool HttpDownload::addRequest(QString keyword, const QUrl &url, const QString &cacheFilePath)
{
//    HttpDownloadDebug("%s", QThread::currentThread()->objectName().toUtf8().data());
    HttpDownloadDebug("keyword is: %s, url is: %s, cacheFilePath is: %s",
                      keyword.toUtf8().data(),
                      url.toString().toUtf8().data(),
                      cacheFilePath. toUtf8().data());

    if (!url.isValid())
    {
        HttpDownloadDebug("Error:URL is invalid.");
        return false;
    }

    if (url.scheme() != "http")
    {
        HttpDownloadDebug("Error:URL must start with 'http:'");
        return false;
    }

    if (url.path().isEmpty())
    {
        HttpDownloadDebug("Error:URL's path is empty.");
        return false;
    }

    if("" != cacheFilePath)
    {
        int pos = cacheFilePath.lastIndexOf("/");
        QString folderPath = cacheFilePath.left(pos + 1);
        QDir *dir = new QDir;
        if (!dir->exists(folderPath))
        {
            if(!dir->mkpath(folderPath))
            {
                HttpDownloadDebug("Error:make path failed. :%s", folderPath.toUtf8().data());
                delete dir;
                return false;
            }
        }
        delete dir;
    }


    HttpDownloadInfo info;
    info.keyword = keyword;
    info.url = url;
    info.cacheFilePath = cacheFilePath;
    info.fileSize = 0;
    info.allBytesRead = 0;

    downloadInfoList->append(info);

    return true;
}
