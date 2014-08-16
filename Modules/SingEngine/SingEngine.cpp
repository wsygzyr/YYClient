#include "SingEngine.h"

QAtomicPointer<SingEngine> SingEngine::instance;
QMutex SingEngine::instanceMutex;

SingEngine::SingEngine()
{
    // ADD
    pFormat = new QAudioFormat();
    pFormat->setFrequency(22050);
    pFormat->setChannels(1);
    pFormat->setSampleSize(16);
    pFormat->setCodec("audio/pcm");
    pFormat->setByteOrder(QAudioFormat::LittleEndian);
    pFormat->setSampleType(QAudioFormat::UnSignedInt);

    pAudioDeviceInfo = new QAudioDeviceInfo(QAudioDeviceInfo::defaultOutputDevice());
    if (!pAudioDeviceInfo->isFormatSupported(*pFormat))
    {
        *pFormat = pAudioDeviceInfo->nearestFormat(*pFormat);
    }

    pAudioOutput = new QAudioOutput(*pAudioDeviceInfo, *pFormat, this);
    pAudioOutput->setBufferSize(4410 * 2);
    pOutputIODevice = pAudioOutput->start();
    // ADDED

    recordTimer = new QTimer();
    connect(recordTimer , SIGNAL(timeout()), this, SLOT(handleRecordTimerOnTimeOut()));

    yync    = YYNotificationCenter::getInstance();
    nc      = NodeNotificationCenter::getInstance();
    cfg     = ConfigHelper::getInstance();
    info    = PkInfoManager::getInstance();
    speechChange = ChangeSpeech::getInstance();

    audioRecord = NULL;
    audioPlayer = NULL;

    ses = SESMarkCaller::getInstance();
    QObject::connect(ses ,SIGNAL(onScore(Score,Score)) ,this ,SIGNAL(onScore(Score,Score)));

    lyricParse = LyricParse::getInstance();

    thread = new QThread();


    this->moveToThread(thread);
    connect(thread , SIGNAL(started()) , this , SLOT(handleThreadOnStart()));

    thread->start();
    fadeType = NONE;
    deltaVol = 0;
    volume = 1;

    recordTime = 0;
}

SingEngine::~SingEngine()
{
    speechChange->destroy();

    // ADD
    if (pAudioOutput) {
        delete pAudioOutput;
        pAudioOutput = NULL;
    }
    if (pAudioDeviceInfo) {
        delete pAudioDeviceInfo;
        pAudioDeviceInfo = NULL;
    }
    if (pFormat) {
        delete pFormat;
        pFormat = NULL;
    }
    // ADDED

    if(volTimer)
        volTimer->deleteLater();
    thread->quit();
    thread->wait(100);
    thread->deleteLater();
    if(recordTimer)
        recordTimer->deleteLater();
}

void SingEngine::init(QString mp3FilePath, QString outFilePath, int sectionIndex, int sampleRate, int bits, int channels)
{
    mutex.lock();
    SingEngineDebug("SingEngine init()... sectionIndex is: %d", sectionIndex);
    // decode, MP3 -> WAV
//    if (!decodeAudio.decode(mp3FilePath, outFilePath))
//    {
//        isError = true;
//        SingEngineDebug("error: decode failed!");
//    }
    this->pcmFilePath = outFilePath;

    recordCountNum = 0;
    recordCountFlags = 0;

    // deactive speech change prop
    curProp.setIsActive(false);

    // init AudioRecord
    audioRecord = new AudioRecord();
    QObject::connect(audioRecord, SIGNAL(onRecordData(QByteArray)), this, SLOT(handleAudioRecordOnRecordData(QByteArray)));
    QObject::connect(this, SIGNAL(onAudioRecordInit())  ,   audioRecord, SLOT(init()));
    QObject::connect(this, SIGNAL(onAudioRecordStart()) ,   audioRecord, SLOT(start()));
    QObject::connect(this, SIGNAL(onAudioRecordStop())  ,   audioRecord, SLOT(stop()));
    QObject::connect(this, SIGNAL(onAudioRecordUnInit()),   audioRecord, SLOT(unInit()));

    audioRecord->setDataFormat(22050, 16, 1);
    audioRecord->init();

    // init audioPlayer, to set sound track
    float startTime = lyricParse->getSectionBeginTime(sectionIndex);
    float endTime = lyricParse->getSectionEndTime(sectionIndex);
    factor = channels*sampleRate*bits
            /audioRecord->getChannel()/audioRecord->getSampleRate()/audioRecord->getSampleSize();
    sectionBeginBytesNum = startTime * channels * sampleRate * bits / BITS_PER_BYTE / MS_PER_SECOND;
    sectionEndBytesNum = endTime * channels * sampleRate * bits / BITS_PER_BYTE/MS_PER_SECOND;
    if (sectionBeginBytesNum % 4 != 0)
    {
        sectionBeginBytesNum = sectionBeginBytesNum - sectionBeginBytesNum % 4;
    }
    if (sectionEndBytesNum % 4 != 0)
    {
        sectionEndBytesNum = sectionEndBytesNum - sectionEndBytesNum % 4;
    }
    recordBytesNum = sectionBeginBytesNum;
    SingEngineDebug("singEngine start | starttime:%f,endtime:%f,recordBytesNum:%d(bytes)",
                    startTime, endTime, recordBytesNum);

    audioPlayer = new FilePlayer(cfg->getPluginPath()+cfg->getDownloadFileSavePath()+ "/"+ QString::number(info->getMe().getID()) +"_songcut.pcm" , false);
    connect(audioPlayer , SIGNAL(onMusicTime(float)) , this , SLOT(handleAudioPlayerOnMusicTime(float)));
    connect(this , SIGNAL(onStopAudioPlayer()) , audioPlayer , SLOT(stop()));
    connect(this , SIGNAL(onAudioRecordStop()) , audioRecord , SLOT(stop()));
    connect(this , SIGNAL(onAudioRecordUnInit()) , audioRecord , SLOT(unInit()));

    mutex.unlock();
    return ;
}


void SingEngine::unInit()
{        
    mutex.lock();
    SingEngineDebug("SingEngine uninit()...");
    if (audioRecord)
    {
        SingEngineDebug("SingEngine AudioRecord Uninit...");
        emit onAudioRecordUnInit();
        audioRecord->deleteLater();
        audioRecord = NULL;
    }
    if (audioPlayer)
    {
//        audioPlayer->stop();
        SingEngineDebug("SingEngine AudioPlayer Uninit...");
        emit onStopAudioPlayer();
        audioPlayer->deleteLater();
        audioPlayer = NULL;
    }
    mutex.unlock();
}

#include <Windows.h>
bool SingEngine::start(int sectionIndex)
{
    SingEngineDebug("SingEngine start()...");
    bool retcode = true;
    mutex.lock();
    retcode = ses->start(sectionIndex, audioRecord->getSampleRate());

    SingEngineDebug("ses start over");

    audioRecord->start();
    audioPlayer->start();

    yync->sendPCMBegin(audioRecord->getSampleRate(),
                       audioRecord->getChannel(),
                       audioRecord->getSampleSize());


    float tickTime = (float)GetTickCount();
    SingEngineDebug("tickTime is: %f", tickTime);
    nc->sendSingTimeOffset(tickTime);

    // check if audiorecord failed in 3 second later
    QTimer::singleShot(3000, this, SLOT(handleTimerOnCheckAudioInput()));

    recordTimer->start(2000);

    startFadeIn(11000);
    mutex.unlock();
    return retcode;
}


void SingEngine::stop()
{
    SingEngineDebug("SingEngine stop()...");
    mutex.lock();

    recordTimer->stop();

    if (audioRecord)
    {
        emit onAudioRecordStop();
        emit onAudioRecordUnInit();
        audioRecord->deleteLater();
        audioRecord = NULL;
    }
    if (audioPlayer)
    {
        emit onStopAudioPlayer();
        audioPlayer->deleteLater();
        audioPlayer = NULL;
    }
    ses->stop();

    speechChange->unInit();
    curProp.setIsActive(false);

    if(fadeType != NONE)
    {
        volTimer->deleteLater();
        volTimer = NULL;
        fadeType = NONE;
    }
    yync->sendPCMEnd();
    mutex.unlock();
}

SingEngine *SingEngine::getInstance()
{
    SingEngineDebug("singEngine getInstance thread:%s",QThread::currentThread()->objectName().toUtf8().data());
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            SingEngineDebug("create instance!");
            instance = new SingEngine();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void SingEngine::destroy()
{
    instanceMutex.lock();
    SingEngineDebug("SingEngine destroy");
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}

int SingEngine::getRecordChannel() const
{
    SingEngineDebug("SingEngine getRecordChannel()...");
    return audioRecord->getChannel();
}

int SingEngine::getRecordSampleSize() const
{
    SingEngineDebug("SingEngine getRecordSampleSize()...");
    return audioRecord->getSampleSize();
}

int SingEngine::getRecordSampleRate() const
{
    SingEngineDebug("SingEngine getRecordSampleRate()...");
    return audioRecord->getSampleRate();
}

void SingEngine::propActive(Prop prop)
{
    SingEngineDebug("prop.getID() is: %d", prop.getID());

    if (curProp.getIsActive())
    {
        SingEngineDebug("curProp is active! curProp.getID() is: %d", curProp.getID());

        curProp.setIsActive(false);
        speechChange->unInit();

    }

    curProp = prop;
    curProp.setIsActive(true);

    ChangeSpeech::EChangeType eChangeType;
    switch (prop.getID())
    {
    case PROP_ID_TRANSFORMER:
        eChangeType = ChangeSpeech::CHANGE_TYPE_TRANSFORMER;
        break;
    case PROP_ID_TOMCAT:
        eChangeType = ChangeSpeech::CHANGE_TYPE_TOMCAT;
        break;
    default:
        break;
    }

    if (!speechChange->init(eChangeType, ChangeSpeech::AudioSampleRate_22k))
    {
        SingEngineDebug("changeSpeech dll init failed!");
        return;
    }
}

void SingEngine::propDeactive()
{
    if (!curProp.getIsActive())
    {
        SingEngineDebug("curProp is already deactived!");
        return;
    }
    curProp.setIsActive(false);
    speechChange->unInit();
}

float SingEngine::getRecordTime()
{
    return recordTime;
}

void SingEngine::startFadeIn(float msec)
{
    if(msec == 0)
    {
        SingEngineDebug("must specify a msec larger than zero");
        return;
    }
    if(fadeType == NONE)
    {
        volume = 0.5;
        fadeType = FADE_IN;
        deltaVol = 1.0 / (msec / 100);
        SingEngineDebug("deltaVol is %f" ,deltaVol);
    }
    else
    {
        SingEngineDebug("current state is not FADE_NONE");
    }
}

void SingEngine::startFadeOut(float msec)
{
    if(msec == 0)
    {
        SingEngineDebug("must specify a msec larger than zero");
        return;
    }
    if(fadeType == NONE)
    {
        volume = 1;
        fadeType = FADE_OUT;
        deltaVol = 1.0 / (msec / 100);
        SingEngineDebug("deltaVol is %f" ,deltaVol);
    }
    else
    {
        SingEngineDebug("current state is not FADE_NONE");
    }
}

void SingEngine::checkFadeOut(float musicTime)
{
    int currentSection = info->getCurrentPkPlayerIndex();
    float sectionEndTime = lyricParse->getSectionEndTime(currentSection);
    if(sectionEndTime - musicTime <= 11000 && fadeType == NONE)
    {
        startFadeOut(11000);
        SingEngineDebug("fade out");
    }
}

void SingEngine::handleVolTimerOnTimeOut()
{
    if(fadeType == FADE_IN)
    {
        volume += deltaVol;
        if(volume >= 1.0)
        {
            fadeType = NONE;
            SingEngineDebug("fade_in over ,volume is %f" , volume);
        }

        SingEngineDebug("volume is %f",volume);
    }
    else if(fadeType == FADE_OUT)
    {
        volume -= deltaVol;
        if(volume <= 0.000001)
        {
            fadeType = NONE;
            SingEngineDebug("fade_out over ,volume is %f" , volume);
        }
        SingEngineDebug("volume is %f",volume);
    }
}

void SingEngine::handleRecordTimerOnTimeOut() // cychen3 added
{
    SingEngineDebug("handleRecordTimerOnTimeOut 2s");
    if ((sectionEndBytesNum - recordBytesNum == recordCountNum) && (1 != recordCountFlags))
    {
        SingEngineDebug("audiorecorder cannot read data");
        emit onSingFinish(true);
        yync->sendPCMEnd();
        recordCountFlags = 1;
    }
    recordCountNum = sectionEndBytesNum - recordBytesNum;
}

void SingEngine::handleAudioRecordOnRecordData(QByteArray recordData)
{
    mutex.lock();
    if(audioPlayer)
    {
//        checkFadeOut(audioPlayer->getCurrentMusicTime());
        mixData(recordData);
        ses->sendAudio(recordData);
        recordTime += 100;
        if (recordBytesNum == sectionEndBytesNum)
        {
            emit onSingFinish(false);
            mutex.unlock();
            //this->stop();
            yync->sendPCMEnd();
            return;
        }
//        SingEngineDebug("audioPlayer time:%f", audioPlayer->getCurrentMusicTime());
    }
    mutex.unlock();
}

void SingEngine::handleAudioPlayerOnMusicTime(float f)
{
    f += lyricParse->getSectionBeginTime(info->getCurrentPkPlayerIndex());
    SingEngineDebug("audioPlayer time is %f ",f);
    emit onMusicTime(f);
}

void SingEngine::handleTimerOnCheckAudioInput()
{

    if (recordBytesNum == sectionBeginBytesNum)
    {
        SingEngineDebug("audiorecord failed! recordBytesNum == sectionBeginBytesNum");
        SingEngineDebug("recordBytesNum is %d , sectionBeginBytesNum is %d" ,recordBytesNum ,sectionBeginBytesNum );
        emit onSingFinish(true);
        yync->sendPCMEnd();
    }
}

void SingEngine::mixData(QByteArray& recordData)
{
    SingEngineDebug("mix data ,recordBytesNum is %d ,sectionEndBytesNum is %d" , recordBytesNum,sectionEndBytesNum);
    if (recordBytesNum > sectionEndBytesNum)
    {
        return;
    }
//    SingEngineDebug("mix data ,recordBytesNum is %d ,sectionEndBytesNum is %d" , recordBytesNum,sectionEndBytesNum);
    pcmFile.setFileName(this->pcmFilePath);
    if (!pcmFile.open(QIODevice::ReadOnly))
    {
        SingEngineDebug("open pcmfile failed:%s",pcmFile.fileName().toUtf8().data());
        emit onError(SingEngine::error::openFileFailed);
        return ;
    }


    if(!pcmFile.seek(WAV_HEAD_SIZE + recordBytesNum))
    {
        pcmFile.close();
        SingEngineDebug("can't seek to pcmfile pos: %d", WAV_HEAD_SIZE + recordBytesNum);
        return;
    }

    qint64 pcmDataBytesNum = recordData.size()*factor;

    if (recordBytesNum + pcmDataBytesNum >= sectionEndBytesNum)
    {
        SingEngineDebug("recordBytesNum + pcmDataBytesNum >= sectionEndBytesNum");
        pcmDataBytesNum = sectionEndBytesNum - recordBytesNum;
        recordBytesNum = sectionEndBytesNum;
    }
    else
    {
        recordBytesNum = recordBytesNum + pcmDataBytesNum;
 //       SingEngineDebug("mix data ,recordBytesNum is %d ,sectionEndBytesNum is %d" , recordBytesNum,sectionEndBytesNum);

    }

    QByteArray mp3CacheData = pcmFile.read(pcmDataBytesNum);
    pcmFile.close();

    qint64 recordLen = pcmDataBytesNum/factor;

    if (curProp.getIsActive())
    {
        recordData = speechChange->process(recordData);

        pOutputIODevice->write(recordData);
    }

    short * shortRecordData = (short*)recordData.data();
    short * shortMp3Data = (short*)mp3CacheData.data();

    //--------to mix record data and mp3 data-------------
    int nSum,pNSum,nj,cj,dj;
    QByteArray outArray;
    int K[5] = {0,28672,32256,32704,32760};
    for(int i = 0 ,j = 0; i < recordLen/2; i++,j+=factor)
    {
        short out;
        shortMp3Data[j] *= volume;
        nSum = shortRecordData[i] + shortMp3Data[j];
        pNSum = abs(nSum);
        nj = qMin(pNSum >> 15, 4);
        cj = pNSum & 32767;
        dj = (cj << 2) + (cj << 1) + cj;
        out = K[nj] + (dj >> ((nj << 1) + nj + 3));
        if(nSum < 0)
        {
            out = -out;
        }

        outArray.append((char*)&out,2);

    }
    //--------to mix record data and mp3 data-----------------


    SingEngineDebug("send to YY mix data");
    yync->sendPCMData(outArray);


    return;
}


void SingEngine::handleThreadOnStart()
{
    thread->setObjectName("SingEngineThread-"+ QString::number((int)QThread::currentThreadId()));
    SingEngineDebug("current thread:%x",QThread::currentThreadId());
    //    qDebug()<<"SingEngine current thread id:"<<QThread::currentThreadId();
    volTimer = new QTimer;
    connect(volTimer , SIGNAL(timeout()) , this , SLOT(handleVolTimerOnTimeOut()));
    volTimer->start(100);
}
