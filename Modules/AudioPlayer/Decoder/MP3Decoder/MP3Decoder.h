#ifndef MP3DECODER_H
#define MP3DECODER_H

#include <QObject>
#include <QMutex>
#include <QBuffer>
#include <QLibrary>
#include <QList>
#include <QByteArray>
#include "../Decoder.h"

#ifdef _DEBUG
#define MADDLL "dll/libMad.dll"
#else
#define MADDLL "dll/libMad.dll"
#endif

#define MP3DECODER_DEBUG 1

#if MP3DECODER_DEBUG
#include <QDebug>
#define MP3DecoderDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define MP3DecoderDebug(format, ...)
#endif

class MP3Decoder : public Decoder
{
    Q_OBJECT
public:
    explicit MP3Decoder(qint64 decoderBufferLen,
                        qint64 fileLength);
    ~MP3Decoder();
private:
    bool                        isLoadDllSuccess;

signals:
    void onDuration(float duration);     //ms
    void onAudioFormat(int sampleRate, int bitsPerSample, int channels);
public slots:

private:
    qint64                      decoderBufferLen;
    bool                        isAudioFormatNotified;
    bool                        isDurationNotified;
    QLibrary                    libMad;
    int                         handle;
    qint64                      fileLength;
public:
    QByteArray decode(QByteArray mp3Data);
    bool getDecoderState();
    void getAudioFormat(int *sampleRate,
                        int *bitsPerSample,
                        int *channels,
                        int *bitRate);
};

#endif // MP3DECODER_H
