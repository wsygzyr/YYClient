#ifndef PCMPLAYER_H
#define PCMPLAYER_H

#include <QObject>
#include <QList>
#include <QByteArray>
#include <QIODevice>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioFormat>
#include <QMutex>
#include <QThread>
#include <QTimer>

#define PCMPLAYER_DEBUG 1
#if PCMPLAYER_DEBUG
#include <QDebug>
#define PCMPlayerDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define PCMPlayerDebug(format, ...)
#endif

class PcmDataQueue;

enum PCMPlayerState;

class PCMPlayer : public QObject
{
    Q_OBJECT
public:
    explicit PCMPlayer(qint64 samplesPerSec,
                       qint64 channels,
                       qint64 bitNumPerSample,
                       qint64 bitRate,
                       qint64 bufferCount = 10,
                       QObject *parent = 0);
    ~PCMPlayer();

    enum eSoundTrack
    {
        eLeftSoundTrack     = 0,
        eRightSoundTrack    = 1,
        eNormalSoundTrack   = 2
    };

private:
    qint64              samplesPerSec;
    qint64              channels;
    qint64              bitNumPerSample;
    qint64              bitRate;
    int                 bufferCount;

    PcmDataQueue        *dataQueue;
    QAudioFormat        *pcmFormat;
    QAudioOutput        *audioOutput;
    QIODevice           *audioOutputIODevice;


    int                 timerID;
    QThread             *thread;

    bool                isPaused;        //use control player play

    qint64              writeTotalSize;
    PCMPlayerState      pcmPlayerState;

    eSoundTrack         soundTrack;
    qreal               playerVolume;

    QMutex              mutex;
private:
    QByteArray disposeSoundTrack(QByteArray originalData);
    QByteArray disposeVolume(QByteArray originalData);
private slots:
    void handleThreadOnStarted();
    void handleThreadOnFinished();
protected:
    void timerEvent(QTimerEvent *);



signals:
    void onBufferUsed();        //when play one data over emit it
    void onMusicTime(const float&);             //emit time (ms)
    void onFinished();
public:
    bool isDataQueueFull();                        //get list is full?
    bool isDataQueueEmpty();
    int  getQueueFreeCount();         //get list free count

    bool addPcmDataToDataQueue(const QByteArray&);     //add data
    float getMusicTime();                       //get current play time

    bool isPlaying();
    bool setSoundTrack(eSoundTrack soundTrack);

    float getRemainTime();
    void setVolume(qreal newVolume);
    qreal volume();

public:
    void start();
    void resume();
    void pause();
    void stop();
};

#endif // PCMPLAYER_H
