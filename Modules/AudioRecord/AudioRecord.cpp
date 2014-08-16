#include "AudioRecord.h"

AudioRecord::AudioRecord()
{
    recorder = NULL;
    recordBuff = NULL;
    bPrepare = false;
    recordTime = 0;

    thread = NULL;
}

AudioRecord::~AudioRecord()
{
}

void AudioRecord::setDataFormat(int sampleRate, int sampleSize, int channel)
{
   AudioRecordDebug("AudioRecord setDataFormat()...");

   this->sampleRate = sampleRate;
   this->sampleSize = sampleSize;
   this->channel = channel;
}

void AudioRecord::init()
{
    AudioRecordDebug("AudioRecord init()...");

    recordTime = 0;
    bPrepare = false;
    recordBytesCount = 0;
    DataSizePerInterval = sampleRate*sampleSize*channel/TIMES_OF_ONE_SECOND_TO_100MS/BITS_PER_BYTE;

    thread = new QThread();

    this->moveToThread(thread);
    QObject::connect(thread, SIGNAL(started()), this, SLOT(handleThreadOnStarted()));
    thread->start(QThread::HighestPriority);
    while(bPrepare == false);
    emit onPrepared();
}

void AudioRecord::unInit()
{
    AudioRecordDebug("AudioRecord unInit()...");

    if (thread)
    {
        thread->quit();
    }
}

void AudioRecord::start()
{
    AudioRecordDebug("AudioRecord start()...");
//    AudioRecordDebug("thread:%s",QThread::currentThread()->objectName().toUtf8().data());

    recorder->start(recordBuff);
}

void AudioRecord::pause()
{
    recorder->suspend();
}

void AudioRecord::stop()
{
    AudioRecordDebug("AudioRecord stop()...");
//    AudioRecordDebug("thread:%s",QThread::currentThread()->objectName().toUtf8().data());
    recorder->stop();
}

bool AudioRecord::getPrepare()
{
    return this->bPrepare;
}

qint64 AudioRecord::getRecordTime()
{
    return this->recordTime;
}


qint64 AudioRecord::getDataSizePerInterval() const
{
    return DataSizePerInterval;
}

void AudioRecord::setDataSizePerInterval(const qint64 &value)
{
    DataSizePerInterval = value;
}

int AudioRecord::getSampleRate() const
{
    return sampleRate;
}

void AudioRecord::setSampleRate(int value)
{
    sampleRate = value;
}

int AudioRecord::getSampleSize() const
{
    return sampleSize;
}

void AudioRecord::setSampleSize(int value)
{
    sampleSize = value;
}
int AudioRecord::getChannel() const
{
    return channel;
}

void AudioRecord::setChannel(int value)
{
    channel = value;
}


void AudioRecord::handleRecorderOnNotify()
{
//    AudioRecordDebug("AudioRecord handleRecorderOnNotify()...");
//    AudioRecordDebug("thread:%s",QThread::currentThread()->objectName().toUtf8().data());

    QByteArray bufArray;
    int pos = recordBuff->pos();
    recordBuff->seek(recordBytesCount);

    bufArray = recordBuff->read(DataSizePerInterval);

    recordBytesCount += bufArray.length();
    recordTime = pos*MS_PER_SECOND/this->sampleRate/this->channel/this->sampleSize/BITS_PER_BYTE;

    recordBuff->seek(pos);

    emit onRecordData(bufArray);
}

void AudioRecord::handleThreadOnStarted()
{
    AudioRecordDebug("AudioRecord handleThreadOnStarted()...");
    thread->setObjectName("AudioRecordThread-"+ QString::number((int)QThread::currentThreadId()));
    AudioRecordDebug("current thread:%x",QThread::currentThreadId());
    QAudioFormat format;
    format.setChannelCount(this->channel);
    format.setFrequency(this->sampleRate);
    format.setSampleSize(this->sampleSize);
    format.setCodec("audio/pcm");

    QAudioDeviceInfo devInfo = QAudioDeviceInfo::defaultInputDevice();
    if (!devInfo.isFormatSupported(format))
    {
        format = devInfo.nearestFormat(format);
    }

    recorder = new QAudioInput(format);
    recorder->setNotifyInterval(RECORD_TIME_INTERVAL);
    QObject::connect(recorder, SIGNAL(notify()), this, SLOT(handleRecorderOnNotify()));

    QObject::connect(thread, SIGNAL(finished()), recorder, SLOT(deleteLater()));


    recordBuff = new QBuffer();
    if(!recordBuff->open(QIODevice::ReadWrite))
    {
        AudioRecordDebug("open buffer failed, recordBuff->errorString is: %s",
                         recordBuff->errorString().toUtf8().data());
        return;
    }

    QObject::connect(thread, SIGNAL(finished()), recordBuff, SLOT(deleteLater()));
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    bPrepare = true;
}


