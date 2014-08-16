#ifndef HTTPDOWNLOAD_H
#define HTTPDOWNLOAD_H


#include <QObject>

#include <QtNetwork>
#include <QByteArray>
#include <QLinkedList>
#include <QTimer>
#include <QThread>

#define HTTPDOWNLOAD_DEBUG  1

#if HTTPDOWNLOAD_DEBUG
#define HttpDownloadDebug(format, ...)  qDebug("%s, LINE: %d --->"format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define HttpDownloadDebug   /\
/
#endif

class HttpDownloadInfo
{
public:
    QString keyword;
    QUrl url;
    int fileSize;
    qint64 allBytesRead;
    QString cacheFilePath;

};

class HttpDownload : public QObject
{
    Q_OBJECT

private:
    HttpDownload();
    ~HttpDownload();

/*
 *  singleton
 */
private:
    static QAtomicPointer<HttpDownload> instance;
    static QMutex instanceMutex;

public:
    static HttpDownload *getInstance();
    static void destroy();

protected:
    void timerEvent(QTimerEvent *);

private:
    QThread                     *thread;
private slots:
    void handleThreadStarted();

private:
    QNetworkAccessManager       *httpStream;
    QNetworkReply               *httpReply;
    bool                        m_blDownLoadError;

private slots:
    void handleHttpProgress(/*qint64 bytesRead, qint64 totalBytes*/);
    void handleHttpDone();
    void handleHttpError(QNetworkReply::NetworkError errorCode);
    void handleSSLError(QNetworkReply* networkReply,QList<QSslError> errorList);

private:
    QLinkedList<HttpDownloadInfo>   *downloadInfoList;
    HttpDownloadInfo                currentDownloadInfo;
    bool                            bDownloading;
    int                             downLoadTimes;
signals:
    void onFileSize(QString keyword, int fileSize);
    void onProgress(QString keyword, const QByteArray &data, qint64 allByteRead );
    void onFinish(QString keyword, bool error,qint64 allByteRead);
    void onClearData(QString keyword);

public:
    bool addRequest(QString keyword, const QUrl &url, const QString &cacheFilePath);

private:
    int JudgeFileType(QString fileName);

};

#endif // HTTPDOWNLOAD_H
