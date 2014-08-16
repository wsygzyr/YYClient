#ifndef DECODER_H
#define DECODER_H

#include <QObject>

class Decoder : public QObject
{
    Q_OBJECT
public:
    explicit Decoder(QObject *parent = 0);

signals:
    void onAudioFormat(int sampleRate,
                       int bitsPerSample,
                       int channels,
                       int bitRate);
public slots:
public:
    QList<QByteArray>           temporaryPcmData;
public:
    virtual QByteArray decode(QByteArray wavData) = 0;
    virtual bool getDecoderState() = 0;
    virtual void getAudioFormat(int *sampleRate,
                                int *bitsPerSample,
                                int *channels,
                                int *bitRate) = 0;
};

#endif // DECODER_H
