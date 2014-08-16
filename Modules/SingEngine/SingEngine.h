#ifndef SINGENGINE_H
#define SINGENGINE_H

#include <QObject>
#include <QMutex>

#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include "Modules/AudioRecord/AudioRecord.h"
#include "Modules/SESMark/SesMarkCaller.h"
#include "Modules/LyricParse/LyricParse.h"
#include "Modules/AACEncode/AACEncodeCaller.h"
//#include "Modules/AudioPlayer/AudioPlayer.h"
#include "Modules/DecodeAudioFile/DecodeAudioFile.h"
#include "Modules/ChangeSpeech/ChangeSpeech.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/FilePlayer/FilePlayer.h"

#define READ_SIZE    1024
#define WAV_HEAD_SIZE   44

#define SOUND_TRACK_STEREO    2

#define SINGENGINE_DEBUG  1

#if SINGENGINE_DEBUG
#include <QDebug>
#define SingEngineDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define SingEngineDebug(format,...)
#endif

//instance
class SingEngine : public QObject
{
    Q_OBJECT
public:
    explicit SingEngine();
    ~SingEngine();

    void setVolume(int volume);
    int  getVolume()const;

    int getRecordSampleRate() const;
    int getRecordSampleSize() const;
    int getRecordChannel() const;

    static SingEngine *getInstance();
    static void destroy();
public slots:
    //eg.   sampleRate(22050) sampleSize(16) channel(2)
    void init(QString mp3FilePath, QString outFilePath, int sectionIndex, int sampleRate, int bits, int channels);
    void unInit();
    bool start(int sentenceIndex);
    void stop();

public:
    enum error
    {
        loadDecodeDllFailed = -1,
        loadDllFuncFailed = -2,
        openFileFailed = -3,
        audioPlayerStartFailed = -4
    };
private:
    void mixData(QByteArray &recordData);
private:
    static QAtomicPointer<SingEngine> instance;
    static QMutex instanceMutex;

    YYNotificationCenter *yync;
    NodeNotificationCenter *nc;
    PkInfoManager        *info;

    FilePlayer      *audioPlayer;
    AudioRecord     *audioRecord;
    QThread         *thread;
    SESMarkCaller   *ses;
    LyricParse      *lyricParse;
    DecodeAudioFile  decodeAudio;
    ChangeSpeech    *speechChange;
    ConfigHelper    *cfg;

    QAudioFormat *pFormat;
    QAudioDeviceInfo *pAudioDeviceInfo;
    QAudioOutput *pAudioOutput;
    QIODevice * pOutputIODevice;


/*
 *  Prop
 */
private:
    Prop           curProp;

private:
    QFile         pcmFile;
    QString       pcmFilePath;

    qint64        recordBytesNum;
    qint64        sectionBeginBytesNum;
    qint64        sectionEndBytesNum;
    qint64        recordCountNum;  // cychen3 added, used in handleAudioRecordOnRecordData
    qint64        recordCountFlags;
    float         volume;
    float         deltaVol;
    QTimer        *volTimer;
    QTimer        *recordTimer;
    QMutex        mutex;

    float         recordTime;

    int           factor;
    typedef enum
    {
        NONE,
        FADE_IN,
        FADE_OUT
    }FadeType;
    FadeType      fadeType;


signals:
    void onMusicTime(float time);
    void onRecordTime(float time);
    void onScore(Score,Score);
    void onPCMData(QByteArray outArray);
    void onPCMBegin(int sampleRate,int channels,int bits);
    void onPCMEnd();
    void onRecordData(QByteArray recordData);
    void onSingReady();
    void onSingFinish(bool);
    void onError(SingEngine::error errorCode);

    void onAudioRecordInit();
    void onAudioRecordUnInit();
    void onAudioRecordStart();
    void onAudioRecordStop();

    void onStartAudioPlayer(QString,bool);
    void onStopAudioPlayer();
private slots:
    void handleAudioRecordOnRecordData(QByteArray byteArray);
    void handleAudioPlayerOnMusicTime(float);
    void handleTimerOnCheckAudioInput();

    void handleThreadOnStart();
    void handleVolTimerOnTimeOut();
    void handleRecordTimerOnTimeOut();
public:
    void propActive(Prop);
    void propDeactive();

    float getRecordTime();

private slots:
    void startFadeIn(float msec);
    void startFadeOut(float msec);

    void checkFadeOut(float musicTime);
};

#endif // SINGENGINE_H
