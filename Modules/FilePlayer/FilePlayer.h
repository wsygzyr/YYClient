#ifndef FILEPLAYER_H
#define FILEPLAYER_H

#include <QAudioOutput>
#include <QFile>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QTime>
#define FILEPLAYER_DEBUG 1
#if FILEPLAYER_DEBUG
#include <QDebug>
#define FilePlayerDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define FilePlayerDebug(format, ...)
#endif

class FilePlayer:public QObject
{
    Q_OBJECT
public:
    FilePlayer(QString fileName , bool bSelfDelete );
    ~FilePlayer();
public slots:
    void start();
    void stop();
private:
    QByteArray buf;
    QThread *thread;
    QAudioOutput  *fileOutPlayer;
    QIODevice *device;
    bool   bSelfDelete;
    QString fileName;
    float time;

    QTimer *timer;
    bool bStart;
    QMutex mutex;
    QTime   elapseTime;
    int   offset;
signals:
    void onMusicTime(float time);
    void finished();
public slots:
    void handleTimerOnTimeOut();
    void handleOnThreadStart();

};

#endif // FILEPLAYER_H
