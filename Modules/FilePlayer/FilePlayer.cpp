#include "FilePlayer.h"
#include <QSound>
FilePlayer::FilePlayer(QString fileName , bool bSelfDelete)
{
    fileOutPlayer = NULL;
    device = NULL;
    this->bSelfDelete = bSelfDelete;
    this->fileName = fileName;
    thread = new QThread();

    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(handleOnThreadStart()));
    connect(thread, SIGNAL(finished()), this, SLOT(deleteLater()));
    thread->start();
    bStart = false;
    offset = 0;
}

FilePlayer::~FilePlayer()
{
    if(thread)
        thread->quit();
}

void FilePlayer::start()
{  
    QFile file(fileName);

    if(!file.exists())
    {
        FilePlayerDebug("can't find file :%s",fileName.toUtf8().data());
        return ;
    }
    if(!file.open(QIODevice::ReadOnly))
    {
        FilePlayerDebug("can't open file :%s",fileName.toUtf8().data());
        return ;
    }

    QAudioFormat format;
    // Set up the format, eg.
    format.setFrequency(22050);
    format.setChannels(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    buf.clear();
    if(QAudioDeviceInfo::availableDevices(QAudio::AudioOutput).size() == 0)
    {
        FilePlayerDebug("no available devices");
        return;
    }

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format))
    {
        FilePlayerDebug("set PCM format failed!");
        return ;
    }

    fileOutPlayer = new QAudioOutput(format);
    buf = file.readAll();
    fileOutPlayer->setBufferSize(4410 * 5);
    offset = 4410 * 5;
    time = 0.0;
    device = fileOutPlayer->start();
    device->write(buf.data() , 4410 * 5);
    bStart = true;
    elapseTime.start();
    FilePlayerDebug("filePlayer start");
    return ;
}

void FilePlayer::stop()
{
    mutex.lock();
    if(fileOutPlayer)
    {
        if(device)
            device->close();
        fileOutPlayer->stop();
        fileOutPlayer->deleteLater();
        fileOutPlayer = NULL;
        buf.clear();
    }
    if(timer)
        timer->stop();
    time = 0.0;
    mutex.unlock();
}



void FilePlayer::handleTimerOnTimeOut()
{
    if(bStart)
    {
        time += 20;
        if(fileOutPlayer->bytesFree() >= 4410)
        {

            int writeSize = ((buf.size() - offset) >= 4410) ? 4410 : (buf.size() - offset);
            if(writeSize > 0)
            {
                int writeNum = device->write(buf.data() + offset , writeSize);
                offset += writeNum;
            }
        }
        if((int)time % 100 == 0)
            emit onMusicTime(time + 30);
    }
}

void FilePlayer::handleOnThreadStart()
{
    thread->setObjectName("FilePlayerThread" + QString::number((int)QThread::currentThreadId()));
    FilePlayerDebug("FilePlayerThread start");
    time = .0;
    timer = new QTimer(this);
    timer->setInterval(20);
    connect(timer , SIGNAL(timeout()) , this , SLOT(handleTimerOnTimeOut()));
    timer->start();


}



