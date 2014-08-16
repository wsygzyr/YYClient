#ifndef WAVDECODER_H
#define WAVDECODER_H

#include <QObject>
#include "../Decoder.h"

#define WAVDECODER_DEBUG 1

#if WAVDECODER_DEBUG
#include <QDebug>
#define WAVDecoderDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define WAVDecoderDebug(format, ...)
#endif

struct WAVFMT;

class WAVDecoder : public Decoder
{
    Q_OBJECT
public:
    explicit WAVDecoder();
    ~WAVDecoder();

signals:
    void onDuration(float duration);     //ms
    void onAudioFormat(int sampleRate,
                       int bitsPerSample,
                       int channels,
                       int bitRate);
public slots:

public:
    bool getDecoderState();
    QByteArray decode(QByteArray wavData);
    void getAudioFormat(int *sampleRate,
                        int *bitsPerSample,
                        int *channels,
                        int *bitRate);
private:
    WAVFMT                      *format;
    bool                        isAudioFormatNotified;
};

#endif // WAVDECODER_H
