#include "PCMplayer.h"
#include <QFile>
class PcmDataQueue
{
private:
    QList<QByteArray>       queue;
    int                     bufferCount;
    QMutex                  mutex;
    qint64                  bufferSize;
    qint64                  totalSize;

public:
    PcmDataQueue(const int& bufferCount,
                const qint64& bufferSize)
    {
        this->bufferSize = bufferSize;
        this->bufferCount = bufferCount;
        totalSize = 0;
    }

    int getFreeBufferCount() const
    {
        return bufferCount - queue.count();
    }

    bool enqueue(const QByteArray& data)
    {
        mutex.lock();
        if (data.length() > bufferSize ||    // beyond the size of a data
                getFreeBufferCount() == 0)  // buffer is full
        {
            PCMPlayerDebug("beyond the size of a data or buffer is full");
            mutex.unlock();
            return false;
        }
        queue.append(data);
        totalSize += data.length();
        mutex.unlock();
        return true;
    }

    QByteArray dequeue()
    {
        mutex.lock();
        QByteArray data;
        data.clear();
        if (getFreeBufferCount() == bufferCount)    //buffer is empty
        {
            PCMPlayerDebug("buffer is empty");
            mutex.unlock();
            return data;
        }
        data = queue.takeFirst();
        totalSize -= data.length();
        mutex.unlock();
        return data;
    }

    qint64 getBufferSize() const
    {
        return bufferSize;
    }

    bool isQueueEmpty() const
    {
        return (queue.count() == 0);
    }

    void clear()
    {
        queue.clear();
        totalSize = 0;
    }

    qint64 getCurrentBufferSize() const
    {
        if (!queue.isEmpty())
        {
            return queue.first().length();
        }
        return 0;
    }
    qint64 getTotalSize() const
    {
        return totalSize;
    }
};

enum PCMPlayerState
{
    ePcmPlayer_playingState = 0,
    ePcmPlayer_pausingState = 1,
    ePcmPlayer_stopingState = 2
};

PCMPlayer::PCMPlayer(qint64 samplesPerSec,
                     qint64 channels,
                     qint64 bitNumPerSample,
                     qint64 bitRate,
                     qint64 bufferCount,
                     QObject *parent) :
    QObject(parent),
    audioOutput(NULL),
    pcmFormat(NULL),
    audioOutputIODevice(NULL),
    isPaused(true)
{
    writeTotalSize = 0;
    playerVolume = 1;
    this->samplesPerSec = samplesPerSec;
    this->channels = channels;
    this->bitNumPerSample = bitNumPerSample;
    this->bitRate = bitRate;
    this->bufferCount = bufferCount;
    //100 ms
    qint64 bufferSize = 125 * (samplesPerSec * channels * bitNumPerSample) / 8 / 1000;
//    PCMPlayerDebug("buffer size is %d", bufferSize);
    dataQueue = new PcmDataQueue(bufferCount, bufferSize);
    pcmPlayerState = ePcmPlayer_pausingState;

    //default normal soundTrack play
    soundTrack = eNormalSoundTrack;
}

PCMPlayer::~PCMPlayer()
{
}

QByteArray PCMPlayer::disposeSoundTrack(QByteArray originalData)
{
    
    QByteArray data;
    data.clear();
    if (originalData.isEmpty())
    {
        PCMPlayerDebug("originalData isEmpty");
        return data;
    }
    qint64 originalDataLen = originalData.length();
    if ((originalDataLen % 4) != 0)
    {
        originalData = originalData.left((originalDataLen / 4) * 4);
        originalDataLen -= originalDataLen % 4;
    }
    if (soundTrack == eLeftSoundTrack)
    {
        for (qint64 i=0; i<originalDataLen; i+=4)
        {
            QByteArray bTemp = originalData.mid(i, 2);
            data.append(bTemp);
            data.append(bTemp);
        }
        PCMPlayerDebug("soundTrack eLeftSoundTrack");
        return data;
    }
    if (soundTrack == eRightSoundTrack)
    {
        for (qint64 i=0; i<originalDataLen; i+=4)
        {
            QByteArray bTemp = originalData.mid(i+2, 2);
            data.append(bTemp);
            data.append(bTemp);
        }
        PCMPlayerDebug("soundTrack eRightSoundTrack");
        return data;
    }
    return originalData;
}

QByteArray PCMPlayer::disposeVolume(QByteArray originalData)
{
    QByteArray data;
    data.clear();
    if (originalData.isEmpty())
    {
        PCMPlayerDebug("originalData isEmpty");
        return data;
    }
    qint64 originalDataLen = originalData.length();
    if ((originalDataLen % 2) != 0)
    {
        originalData = originalData.left((originalDataLen / 2) * 2);
        originalDataLen -= originalDataLen % 2;
    }
    //set volume
    short *sTemp = (short *)originalData.data();
    for (qint64 i=0; i<originalDataLen/2; i++)
    {
        sTemp[i] = sTemp[i] * playerVolume;
    }
    char *cTemp = (char *)sTemp;
    data.append(cTemp, originalDataLen);
    return data;
}

void PCMPlayer::handleThreadOnStarted()
{
    PCMPlayerDebug("current thread:%x",QThread::currentThreadId());
    thread->setObjectName(QString("PCMPlayerThread-")+ QString::number((int)QThread::currentThreadId()));
    PCMPlayerDebug("prepare pcmPlayer!");
    pcmFormat  = new QAudioFormat();
    pcmFormat->setSampleRate(samplesPerSec);
    pcmFormat->setChannels(channels);
    pcmFormat->setSampleSize(bitNumPerSample);
    if (bitNumPerSample == 8)
    {
        pcmFormat->setSampleType(QAudioFormat::SignedInt);
    }
    else if (bitNumPerSample == 16)
    {
        pcmFormat->setSampleType(QAudioFormat::UnSignedInt);
    }
    pcmFormat->setCodec("audio/pcm");

    QAudioDeviceInfo devInfo = QAudioDeviceInfo::defaultOutputDevice();
    if (!devInfo.isFormatSupported(*pcmFormat))
    {
        *pcmFormat = devInfo.nearestFormat(*pcmFormat);
        PCMPlayerDebug("pcmPlayer prepare failed!");
        return;
    }
    audioOutput = new QAudioOutput(*pcmFormat);
    audioOutput->reset();
    audioOutput->setBufferSize(dataQueue->getBufferSize() * 2);
    PCMPlayerDebug("AudioOutput BufferSize = %d", audioOutput->bufferSize());

    QObject::connect(QThread::currentThread(), SIGNAL(finished()),
            audioOutput, SLOT(deleteLater()));
    connect(QThread::currentThread() , SIGNAL(finished()) , this , SLOT(handleThreadOnFinished()));
    audioOutputIODevice = audioOutput->start();

    timerID = startTimer(20);
    PCMPlayerDebug("pcmPlayer prepare successed!");
}

void PCMPlayer::handleThreadOnFinished()
{
    PCMPlayerDebug("thread finished");
    thread->deleteLater();
    thread = NULL;
    dataQueue->clear();
    audioOutput = NULL;
}

void PCMPlayer::timerEvent(QTimerEvent *)
{
    if (isPaused)
    {
        return;
    }
    if (pcmPlayerState == ePcmPlayer_pausingState)
    {
        audioOutput->suspend();
    }
    if (pcmPlayerState == ePcmPlayer_playingState)
    {
        audioOutput->resume();
    }
    if (pcmPlayerState == ePcmPlayer_stopingState)
    {
        PCMPlayerDebug("stoping!");
        audioOutput->stop();
        PCMPlayerDebug("stoping finished!");
    }

    if (dataQueue->isQueueEmpty() && audioOutput->state() == QAudio::IdleState)
    {
        return;
    }

    qint64 dataFree = audioOutput->bytesFree();
    if (dataFree >= dataQueue->getCurrentBufferSize()
            && !dataQueue->isQueueEmpty())
    {
        QByteArray disposeData = dataQueue->dequeue();

        //soundTrack dispose and volume dispose
        disposeData = disposeSoundTrack(disposeData);
        disposeData = disposeVolume(disposeData);

        qint64 writeSize = audioOutputIODevice->write(disposeData);
        writeTotalSize += writeSize;

        emit onBufferUsed();
//        int *a = 0;
//        *a = 1;
    }

    float musicTime = getMusicTime();
    PCMPlayerDebug("musicTime is: %f", musicTime);
    emit onMusicTime(musicTime);
}

bool PCMPlayer::addPcmDataToDataQueue(const QByteArray &data)
{
    if (dataQueue->enqueue(data))
    {
        PCMPlayerDebug("add pcmData to queue success!");
        return true;
    }
    PCMPlayerDebug("add pcmData to queue failed!");
    return false;
}

void PCMPlayer::start()
{
    PCMPlayerDebug("add pcmData to queue start!");
    thread = new QThread;

    this->moveToThread(thread);
    QObject::connect(thread, SIGNAL(started()),
            this, SLOT(handleThreadOnStarted()), Qt::DirectConnection);
    thread->start();
    PCMPlayerDebug("add pcmData to queue started!");
}

void PCMPlayer::resume()
{
    PCMPlayerDebug("resume!");
    if (audioOutput == NULL)
    {
        return;
    }
    isPaused = false;
    pcmPlayerState = ePcmPlayer_playingState;
//    audioOutput->resume();
    PCMPlayerDebug("resumed!");
}

void PCMPlayer::pause()
{
    PCMPlayerDebug("pause!");
    if (audioOutput == NULL)
    {
        return;
    }
    isPaused = true;
    pcmPlayerState = ePcmPlayer_pausingState;
//    audioOutput->suspend();
    PCMPlayerDebug("paused!");
}

void PCMPlayer::stop()
{
    mutex.lock();

    if (audioOutput == NULL || thread == NULL)
    {
        mutex.unlock();
        return;
    }
    PCMPlayerDebug("stoped!");
    isPaused = true;
    pcmPlayerState = ePcmPlayer_stopingState;


    if (thread->isRunning())
    {
        thread->quit();
    }


    mutex.unlock();
    PCMPlayerDebug("stoped!");
}

float PCMPlayer::getMusicTime()
{
    mutex.lock();
    if (audioOutput == NULL || thread == NULL)
    {
        PCMPlayerDebug("audioOutput:%d,thread:%d",audioOutput,thread);
        mutex.unlock();
        return 0.0;
    }

    qint64 bytesInBuffer = audioOutput->bufferSize() - audioOutput->bytesFree();
    mutex.unlock();

//        PCMPlayerDebug("bytesInBuffer is: %d", bytesInBuffer);
    float realTime = (qint64)(1000) * (float)(writeTotalSize - bytesInBuffer) /
            (float)(channels * bitNumPerSample * samplesPerSec / 8);

     PCMPlayerDebug("writeTotalSize:%d ,bytesInBuffer:%d ,realTime :%f",writeTotalSize,bytesInBuffer,realTime);
    if (realTime < 0)
    {
        PCMPlayerDebug("realTime < 0");
        return 0.0;
    }

    return realTime;

}

bool PCMPlayer::isPlaying()
{
    if (audioOutput == NULL)
    {
        PCMPlayerDebug("isPlaying false");
        return false;
    }
    return (!isPaused && audioOutput->state() == QAudio::ActiveState);
}

bool PCMPlayer::setSoundTrack(PCMPlayer::eSoundTrack soundTrack)
{
    if (channels != 2)
    {
        PCMPlayerDebug("this music only has one soundTrack!");
        return false;
    }
    this->soundTrack = soundTrack;
    return true;
}

float PCMPlayer::getRemainTime()
{
    mutex.lock();
    if (audioOutput == NULL)
    {
        PCMPlayerDebug("audioOutput is NULL!");
        mutex.unlock();
        return -1;
    }
    qint64 bytesInBuffer = audioOutput->bufferSize() - audioOutput->bytesFree();
    mutex.unlock();

    qint64 queueSize = dataQueue->getTotalSize();
    float time = (qint64)(1000) * (float)(bytesInBuffer + queueSize) /
            (float)(channels * bitNumPerSample * samplesPerSec / 8);
    return time;
}

void PCMPlayer::setVolume(qreal newVolume)
{
    if (newVolume < 0 || newVolume > 1)
    {
        PCMPlayerDebug("setVolume failed!");
        return;
    }
    PCMPlayerDebug("volume change %f", newVolume);
    playerVolume = newVolume;
}

qreal PCMPlayer::volume()
{
    return playerVolume;
}

bool PCMPlayer::isDataQueueFull()
{
    return (dataQueue->getFreeBufferCount() == 0);
}

bool PCMPlayer::isDataQueueEmpty()
{
    return dataQueue->isQueueEmpty();
}

int PCMPlayer::getQueueFreeCount()
{
    return dataQueue->getFreeBufferCount();
}
