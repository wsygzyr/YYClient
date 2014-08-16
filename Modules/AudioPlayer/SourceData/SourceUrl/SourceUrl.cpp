#include "SourceUrl.h"

SourceUrl::SourceUrl(QString url)
{
    if (!parseUrl(url))
    {
        SourceUrlDebug("url parse failed!");
        return;
    }
    thread = new QThread;
    thread->setObjectName("SourceUrlThread");
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()),
            this, SLOT(handleThreadOnStarted()));
    isDownloadFinished = false;
    thread->start();
}

bool SourceUrl::parseUrl(QString url)
{
//    if ()
    return true;
}

void SourceUrl::handleThreadOnStarted()
{
    startTimer(25);
}

void SourceUrl::timerEvent(QTimerEvent *)
{
    //step 1. whether download finish
    if (isDownloadFinished)
    {
        return;
    }
}

qint64 SourceUrl::getSourceFileLength()
{
    return 0;
}

bool SourceUrl::getErrorCode()
{
    return true;
}

QByteArray SourceUrl::readFileData(qint64 dataLength,
                                  qint64 startReatePos,
                                  qint64 endReadPos)
{
    QByteArray data;
    return data;
}
