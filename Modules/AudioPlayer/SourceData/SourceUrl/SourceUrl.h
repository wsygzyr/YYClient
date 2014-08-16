#ifndef SOURCEURL_H
#define SOURCEURL_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QFile>
#include "../SourceData.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>

#define SOURCEURL_DEBUG 1

#if SOURCEURL_DEBUG
#include <QDebug>
#define SourceUrlDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define SourceUrlDebug(format, ...)
#endif

class SourceUrl : public SourceData
{
    Q_OBJECT
public:
    explicit SourceUrl(QString url);

private:
    QNetworkAccessManager       *httpStream;
    QNetworkReply               *httpReply;

    QFile                       file;
    qint64                      fileLength;

    QThread                     *thread;
    QMutex                      mutex;

    bool                        isDownloadFinished;
private:
    bool parseUrl(QString url);
private slots:
    void handleThreadOnStarted();
protected:
    void timerEvent(QTimerEvent *);
public:
    qint64 getSourceFileLength();
    bool getErrorCode();
    QByteArray readFileData(qint64 dataLength,
                           qint64 startReatePos,
                           qint64 endReadPos);
};

#endif // SOURCEURL_H
