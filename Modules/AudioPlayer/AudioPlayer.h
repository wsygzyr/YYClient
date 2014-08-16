#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>
#include <QThread>

#define AUDIOPLAYER_DEBUG 1

#if AUDIOPLAYER_DEBUG
#include <QDebug>
#define AudioPlayerDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define AudioPlayerDebug(format, ...)
#endif

#include "PCMPlayer/PCMplayer.h"
#include "SourceData/SourceData.h"
#include "SourceData/SourceFile/SourceFile.h"
#include "SourceData/SourceRAM/SourceRAM.h"
#include "Decoder/Decoder.h"
#include "Decoder/WAVDecoder/WAVDecoder.h"
#include "Decoder/MP3Decoder/MP3Decoder.h"



class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AudioPlayer(QObject *parent = 0);
    ~AudioPlayer();

signals:

public slots:


private:
    int                     timerID;
    QThread                 *thread;
    qint64                  endMusicTime;


    void reset();
    void addPcmDataToPcmPlayer();
    void audioFormatParsed();

private:
    enum eState
    {
        eIdleState            = 0,
        eInitState            = 1,
        ePlayingState         = 2,
        ePausedState          = 3,
        eStopedState          = 4
    };
public:
    enum eError
    {
        eNoError              = 0,
        eFileOpenError        = 1,
        eFileNameIsEmptyError = 2,
        eIntervalTimeError    = 3,
        eDecodeError          = 4
    };

    enum SourceDataType
    {
        eNoSetSType            = 0,
        eSourceFile            = 1,
        eSourceUrl             = 2,
        eSourceRAM             = 3
    };

    enum DecoderType
    {
        eNoSetDType            = 0,
        eDecodePCM             = 1,
        eDecodeWAV             = 2,
        eDecodeMP3             = 3
    };

signals:
    void onFinished();
    void onMusicTime(float time);	//20ms
    void onError(eError);
protected:
    void timerEvent(QTimerEvent *);
private slots:
    void handleThreadOnStarted();
    void handlePCMPlayerOnMusicTime(float fTime);


public slots:
    void init();
    void uninit();
    float getCurrentMusicTime();
    bool appendRAMData(QByteArray);


    //new interface

    bool setSourceData(QString fileName,
                       SourceDataType sType,
                       DecoderType dType,
                       float startTime,
                       float endTime);
    bool setSourceData(QString fileName,
                       SourceDataType sType,
                       DecoderType dType);
	//if isPaused is false ,start play audio right now
    //if isWaitForReady is true, return bool back until prepare success
    bool start(bool isPaused, bool isWaitForReady);
    bool resume();
    bool pause();
    bool stop();

    void setPlayerSoundTrack(PCMPlayer::eSoundTrack);

    void setReserveTime(float time);
    void setVolume(qreal newVolume);
    qreal volume();

private:
    QString                 sourceDataName;
    float                   startTime;
    float                   endTime;
    bool                    isPlayWholeSong;

    SourceDataType          sourceDataType;
    DecoderType             decoderType;

    bool                    isPaused;

    SourceData              *sourceData;
    Decoder                 *decoder;
    PCMPlayer               *pcmPlayer;

    bool                    isAudioFormatParsed;
    qint64                  sourceDataReadLength;

    qint64                  startReadPos;           //be used play music pos
    qint64                  endReadPos;
    qint64                  indexReadPos;

    qint64                  beUsedtoDecodeReadPos;  // 用于前期解析歌曲信息的文件pos
    qint64                  beUsedtoDecodeStopPos;

    volatile eState        playState;
    eError        playError;

    float                   reserveTime;
};

#endif // AUDIOPLAYER_H
