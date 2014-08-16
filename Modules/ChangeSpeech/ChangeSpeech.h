#ifndef CHANGESPEECH_H
#define CHANGESPEECH_H

#include <QObject>
#include <QLibrary>
#include <QMutex>

#ifdef _DEBUG
#define CHANGESPEECHDLL "dll/libChangeSpeechd.dll"
#else
#define CHANGESPEECHDLL "dll/libChangeSpeech.dll"
#endif

#define CHANGESPEECH_DEBUG 1

#if CHANGESPEECH_DEBUG
#include <QDebug>
#define ChangeSpeechDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define ChangeSpeechDebug(format, ...)
#endif

class ChangeSpeech : public QObject
{
    Q_OBJECT

    explicit ChangeSpeech(QObject *parent = 0);
    ~ChangeSpeech();
public:
    enum EChangeType
    {
        CHANGE_TYPE_BABY            = 1,
        CHANGE_TYPE_TOMCAT          = 2,
        CHANGE_TYPE_SANTACLAUS      = 3,
        CHANGE_TYPE_TRANSFORMER     = 4,
        CHANGE_TYPE_ECHO            = 5,
        CHANGE_TYPE_ROBOT           = 6,
        CHANGE_TYPE_MAN             = 7,
        CHANGE_TYPE_WOMAN           = 8,
        CHANGE_TYPE_OLDER           = 9,
    };

    enum EAuidoSampleRate
    {
        AudioSampleRate_8k		= 8000,
        AudioSampleRate_16k		= 16000,
        AudioSampleRate_32k		= 32000,
        AudioSampleRate_48k		= 48000,
        AudioSampleRate_11k		= 11025,
        AudioSampleRate_22k		= 22050,
        AudioSampleRate_44k		= 44100,
    };
private:
    static QAtomicPointer<ChangeSpeech> instance;
    static QMutex                       instanceMutex;
    static QLibrary                     changeSpeechDll;
    static void                         *hPitchshift;

public:
    static ChangeSpeech* getInstance();
    static void destroy();

private:
    QMutex  mutex;

public:
    bool init(EChangeType changeType, EAuidoSampleRate audioSampleRate);
    void unInit();
    QByteArray process(QByteArray input);
};

#endif // CHANGESPEECH_H
