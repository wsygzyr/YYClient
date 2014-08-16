#ifndef AUDIORECORD_H
#define AUDIORECORD_H

#include <QObject>
#include <QThread>
#include <QFile>
#include <QBuffer>
#include <QAudioInput>

#define RECORD_TIME_INTERVAL            100
#define BITS_PER_BYTE                   8
#define TIMES_OF_ONE_SECOND_TO_100MS    10
#define MS_PER_SECOND                   1000


#define AUDIORECORD_DEBUG  1

#if AUDIORECORD_DEBUG
#include <QDebug>
#define AudioRecordDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define AudioRecordDebug(format,...)
#endif

class AudioRecord : public QObject
{
    Q_OBJECT
public:
    explicit AudioRecord();
    ~AudioRecord();

    //eg: sampleRate(22050) sampleSize(16) channel(2)
public slots:
    void setDataFormat(int sampleRate,int sampleSize,int channel);
    void init();
    void unInit();
    void start();
    void pause();
    void stop();
    bool getPrepare();

    qint64 getRecordTime();
    qint64 getDataSizePerInterval() const;
    void setDataSizePerInterval(const qint64 &value);

    int getSampleRate() const;
    void setSampleRate(int value);

    int getSampleSize() const;
    void setSampleSize(int value);

    int getChannel() const;
    void setChannel(int value);

private:
    QThread *thread;
    QAudioInput *recorder;

    QBuffer *recordBuff;
    qint64 recordBytesCount;
    qint64 recordTime;

    int sampleRate;
    int sampleSize;
    int channel;

    qint64 DataSizePerInterval;

    volatile bool bPrepare;
signals:
    void onPrepared();
    void onRecordData(QByteArray);

public slots:
    void handleRecorderOnNotify();
    void handleThreadOnStarted();
};

#endif // AUDIORECORD_H
