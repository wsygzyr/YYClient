#include "AudioPlayer.h"

#define PARSED_NEED_FILESIZE              1024  //parsed audioFormat need read file length
#define PCMPLAYER_BUFFER_COUNT            10    //set pcmPlayer buffer count
#define WAV_FILE_HEAD                     44


//#define FILE_READ_SIZE                    1024
//#define FILE_HEAD_SIZE                    10*1024



AudioPlayer::AudioPlayer(QObject *parent) :
    QObject(parent)
{

    //    AudioPlayerDebug("this thread = %s",QThread::currentThread()->objectName().toLocal8Bit().data());
}

AudioPlayer::~AudioPlayer()
{
}

void AudioPlayer::init()
{
    AudioPlayerDebug("init!");
    this->reset();

    thread = new QThread;

    QObject::connect(thread, SIGNAL(started()), this, SLOT(handleThreadOnStarted()));
    this->moveToThread(thread);
    thread->start();
}

void AudioPlayer::uninit()
{
    AudioPlayerDebug("uninit!");
    this->stop();
    thread->quit();
    thread->wait(200);
    thread->deleteLater();
    thread = NULL;
}

void AudioPlayer::reset()
{
    reserveTime    = 0;

    startTime       = 0;
    endTime        = 0;
    endMusicTime = 0;

    sourceDataType  = eNoSetSType;
    decoderType     = eNoSetDType;

    sourceData      = NULL;
    decoder         = NULL;
    pcmPlayer       = NULL;

    isAudioFormatParsed     = false;
    sourceDataReadLength    = 0;
    startReadPos    = 0;
    endReadPos      = 0;
    indexReadPos    = 0;
    beUsedtoDecodeReadPos         = 0;
    beUsedtoDecodeStopPos         = 0;

    playState       = eIdleState;
    playError       = eNoError;
}

void AudioPlayer::addPcmDataToPcmPlayer()
{
//    AudioPlayerDebug("startReadPos = %d, endReadPos = %d", startReadPos, endReadPos);
    if (isPlayWholeSong && decoder->temporaryPcmData.count() > 0)
    {
        AudioPlayerDebug("temporaryPcmData isn't finished!");
        QByteArray pcmData = decoder->temporaryPcmData.takeFirst();
        if (pcmData.isEmpty())
        {
            return;
        }
        pcmPlayer->addPcmDataToDataQueue(pcmData);
    }
    //because :
    /* when play whole song, before decode read a part data,
     * and decoded data save decoder->temporaryPcmData
     * when playWholeSong is true ,decoder->temporaryPcmData data read to pcmPlayer,
     * so ,no need to decode this part data,skip this part data
     */
    if (isPlayWholeSong && beUsedtoDecodeReadPos != 0)
    {
        AudioPlayerDebug("play whole song ,set startReadPos = beUsedtoDecodeReadPos");
        indexReadPos = beUsedtoDecodeReadPos;
        beUsedtoDecodeReadPos = 0;
    }

    //because :
    /* when sourceType is RAM, it not have startReadPos,
     * read a part data ,delete a part data,
     * so, every time getData can read head,(startReadPos set 0)
     * though read RAM ,cannot used startReadPos and endReadPos,
     * But, startReadPos all along add, when it > endReadPos, pcmPlayer
     * will stop(user don't want it stop),
     */
    if (sourceDataType == eSourceRAM)
    {
        indexReadPos = 0;
    }

    QByteArray originalData = sourceData->readFileData(sourceDataReadLength,
                                                   indexReadPos,
                                                   endReadPos);
    if (originalData.isEmpty())
    {
        if (sourceData->getErrorCode())
        {
            playError = eFileOpenError;
            emit onError(eFileOpenError);
        }
        return;
    }
    indexReadPos += originalData.length();
//    AudioPlayerDebug("originalData length is %d", originalData.length());
    QByteArray pcmData = decoder->decode(originalData);

//    AudioPlayerDebug("decode data length is %d", pcmData.length());
    if (pcmData.isEmpty())
    {
        return;
    }
    pcmPlayer->addPcmDataToDataQueue(pcmData);
    //    AudioPlayerDebug("queue free count is %d", pcmPlayer->getQueueFreeCount());
}

void AudioPlayer::audioFormatParsed()
{
    //step 2.1. judge musicFile
    QByteArray originalData;
    originalData.clear();

    originalData = sourceData->readFileData(PARSED_NEED_FILESIZE,
                                           beUsedtoDecodeReadPos,
                                           beUsedtoDecodeStopPos);

    if (originalData.isEmpty())
    {
//        AudioPlayerDebug("resource file read failed!");
        if (sourceData->getErrorCode())
        {
            playError = eFileOpenError;
            emit onError(eFileOpenError);
        }
        return;
    }
    beUsedtoDecodeReadPos += originalData.length();
    //step 2.2. decode data
    decoder->decode(originalData);
    if (!decoder->getDecoderState())
    {
        return;
    }
    int sampleRate      = 0;
    int bitsPerSample   = 0;
    int channels        = 0;
    int bitRate         = 0;
    decoder->getAudioFormat(&sampleRate, &bitsPerSample, &channels, &bitRate);
    if (sampleRate <= 0
        || bitsPerSample <= 0
        || channels <= 0
        || bitRate <= 0)
    {
        AudioPlayerDebug("audioFormat parsed data is error!");
        return;
    }
//        AudioPlayerDebug("audioFormat parsed success!");
    AudioPlayerDebug("sampleRate is : %d, bitsPerSample is %d, "
                     "channels is %d, bitRate is %d", sampleRate,
                     bitsPerSample, channels, bitRate);
    //step 2.3. if decode success, create pcmPlayer

    sourceDataReadLength = 100 * bitRate / 8 / 1000;    //each read 100ms data
    AudioPlayerDebug("sourceDataReadLength is %d", sourceDataReadLength);
    if (decoderType == eDecodeWAV && !isPlayWholeSong)
    {
        //calculate startReadPos and endReadPos
        AudioPlayerDebug("start calculate startReadPos and endReadPos!");
        
		startReadPos = (int)(startTime / 1000 * (float)(bitRate / 8)) + WAV_FILE_HEAD;       
		if (startReadPos % (2 * channels) != 0)
        {
            startReadPos = (startReadPos / (2 * channels) + 1) * (2 * channels);
        }
        indexReadPos = startReadPos;

        endReadPos = (int)(endTime / 1000 * (float)(bitRate / 8)) + WAV_FILE_HEAD;
        if (endReadPos % (2 * channels) != 0)
        {
            endReadPos = (endReadPos / (2 * channels) + 1) * (2 * channels);
        }
        AudioPlayerDebug("startReadPos is: %lld, endReadPos is: %lld", startReadPos, endReadPos);   
	}
    pcmPlayer = new PCMPlayer(sampleRate,
                              channels,
                              bitsPerSample,
                              bitRate,
                              PCMPLAYER_BUFFER_COUNT);
    if (pcmPlayer == NULL)
    {
        AudioPlayerDebug("pcmPlayer create failed!");
        return ;
    }
    connect(pcmPlayer, SIGNAL(onMusicTime(float)),
            this, SLOT(handlePCMPlayerOnMusicTime(float)));
    pcmPlayer->start();

    AudioPlayerDebug("current playState is %d", playState);
    AudioPlayerDebug("parsed audioFormat is successed!");
    isAudioFormatParsed = true;
}

void AudioPlayer::handleThreadOnStarted()
{
    AudioPlayerDebug("thread start()...!");
    thread->setObjectName(QString("AudioPlayerThread-") + QString::number((int)QThread::currentThreadId()));
    timerID = startTimer(25);
    AudioPlayerDebug("current thread:%x",QThread::currentThreadId());
    QObject::connect(this, SIGNAL(destroyed()), thread, SLOT(quit()));
    QObject::connect(this, SIGNAL(destroyed()), thread, SLOT(deleteLater()));
    connect(thread , SIGNAL(finished()) , thread , SLOT(deleteLater()));
}

void AudioPlayer::handlePCMPlayerOnMusicTime(float fTime)
{
    float time = startTime + fTime;
    emit onMusicTime(time);
}

bool AudioPlayer::setSourceData(QString fileName,
                                SourceDataType sType,
                                DecoderType dType,
                                float startTime,
                                float endTime)
{
    AudioPlayerDebug("audioplayer setSourceData()...!");
    if (playState != eIdleState)
    {
        AudioPlayerDebug("setSourseData failed! because current state is idleState!");
        return false;
    }
    if (sType == eSourceRAM)
    {
        AudioPlayerDebug("RAM data nonsupport this way!");
        return false;
    }
    reset();
    if (fileName.isEmpty())
    {
        AudioPlayerDebug("sourceDataName is empty!");
        playError = eFileNameIsEmptyError;
        emit onError(eFileNameIsEmptyError);
        return false;
    }
    sourceDataName  = fileName;
    //
    if (startTime < 0
        || endTime < 0
        || startTime >= endTime)
    {
        AudioPlayerDebug("Interval time is error");
        playError = eIntervalTimeError;
        emit onError(eIntervalTimeError);
        return false;
    }
    this->startTime = startTime;
    this->endTime   = endTime;
	AudioPlayerDebug("startTime is: %f, endTime is: %f", startTime, endTime);
    //

    sourceDataType  = sType;
    decoderType     = dType;
    isPlayWholeSong = false;
    return true;
}

bool AudioPlayer::setSourceData(QString fileName,
                                SourceDataType sType,
                                DecoderType dType)
{
    AudioPlayerDebug("fileName is: %s, sType is: %s, dType is: %d",
                     fileName.toUtf8().data(),
                     (sType == eSourceFile) ? "file" : (sType == eSourceRAM) ? "RAM" : "unknown",
                     dType);

    if (playState != eIdleState)
    {
        AudioPlayerDebug("setSourseData failed! because current state is not idleState!");
        return false;
    }
    reset();
    if (fileName.isEmpty() && sType != eSourceRAM)
    {
        AudioPlayerDebug("sourceDataName is empty!");
        playError = eFileNameIsEmptyError;
        emit onError(eFileNameIsEmptyError);
        return false;
    }
    sourceDataName  = fileName;

    sourceDataType  = sType;
    decoderType     = dType;
    isPlayWholeSong = true;
    return true;
}

bool AudioPlayer::start(bool isPaused, bool isWaitForReady)
{
    AudioPlayerDebug("audioplayer start()!");
    if (playState != eIdleState)
    {
        AudioPlayerDebug("current player state isn't idleState!");
        return false;
    }
    //judge sourceDataType
    if (sourceDataType == eSourceFile)
    {
        AudioPlayerDebug("create sourceFile!");
        sourceData = new SourceFile(sourceDataName);
    }
    else if (sourceDataType == eSourceRAM)
    {
        AudioPlayerDebug("create sourceRAM!");
        sourceData = new SourceRAM;
    }
    else
    {
        AudioPlayerDebug("fuck! sourceDataType is: %d", sourceDataType);
    }
    if (sourceData == NULL)
    {
        AudioPlayerDebug("sourceData create failed!");
        return false;
    }

    //judge decoderType
    if (decoderType == eDecodeWAV)
    {
        AudioPlayerDebug("create wav decoder!");
        decoder = new WAVDecoder;

        //init be used to decode data length ,wav from 0 start
        beUsedtoDecodeReadPos = 0;
        beUsedtoDecodeStopPos = 1024;
    }
    else if (decoderType == eDecodeMP3)
    {
        AudioPlayerDebug("create mp3 decoder!");
        qint64 sourceDataLength = sourceData->getSourceFileLength();
        if (sourceDataLength <= 0)
        {
            AudioPlayerDebug("sourceFileLength <= 0! is error");
            return false;
        }
        decoder = new MP3Decoder(100*1024, sourceDataLength);

        //because mp3 only play all file , startReadPos = 0
        beUsedtoDecodeReadPos = startReadPos;  //因为mp3现在只是播放全部文件
        beUsedtoDecodeStopPos = sourceDataLength;
    }
    if (decoder == NULL)
    {
        AudioPlayerDebug("decoder create failed!");
        return false;
    }

    this->isPaused = isPaused;
    if (isPlayWholeSong)
    {
        startReadPos = 0;
        endReadPos = sourceData->getSourceFileLength();
        AudioPlayerDebug("endReadPos is %lld", endReadPos);
    }

    endMusicTime = 0;
    playState  = eInitState;
    indexReadPos = startReadPos;
    if (isWaitForReady)
    {
        AudioPlayerDebug("wait to prepare!");
        while (pcmPlayer == NULL || !pcmPlayer->isDataQueueFull() || playState == eInitState)
        {

            if (playError != eNoError)
            {
                AudioPlayerDebug("current error is %d", playError);
                return false;
            }
        }
		AudioPlayerDebug("ready!");
    }
    return true;
}

bool AudioPlayer::resume()
{
    AudioPlayerDebug("audioplayer resume()!");
    if (playState != ePausedState)
    {
        AudioPlayerDebug("resume failed! because current state isn't PausedState!");
        return false;
    }
    if (NULL == pcmPlayer)
    {
        AudioPlayerDebug("pcmPlayer is NULL");
        return false;
    }
    AudioPlayerDebug("resume audioPlayer!");
    playState = ePlayingState;
    pcmPlayer->resume();

    return true;
}

bool AudioPlayer::pause()
{
    AudioPlayerDebug("audioplayer pause()!");
    if (playState != ePlayingState)
    {
        AudioPlayerDebug("pause failed! because current state isn't playingState!");
        return false;
    }
    if (NULL == pcmPlayer)
    {
        AudioPlayerDebug("pcmPlayer is NULL");
        return false;
    }

    AudioPlayerDebug("pause audioPlayer!");
    playState = ePausedState;
    pcmPlayer->pause();
    return true;
}

bool AudioPlayer::stop()
{
    AudioPlayerDebug("audioplayer stop()!");
    if (playState != ePlayingState && playState != ePausedState)
    {
        AudioPlayerDebug("stop failed! because current state isn't PausedState or playingState!");
        //return false;
    }
    if (NULL == pcmPlayer)
    {
        AudioPlayerDebug("pcmPlayer is NULL");
        //return false;
    }
    AudioPlayerDebug("stop audioPlayer!");
    playState = eStopedState;
    if (pcmPlayer)
    {
        endMusicTime = pcmPlayer->getMusicTime() + startTime;
        pcmPlayer->stop();
        pcmPlayer->deleteLater();
        pcmPlayer = NULL;
    }
    if(sourceData)
    {
        sourceData->deleteLater();
        sourceData = NULL;
    }
    if(decoder)
    {
        decoder->deleteLater();
        decoder = NULL;
    }
    reset();
    AudioPlayerDebug("stop audioPlayer! playState is %d", playState);
    return true;
}

void AudioPlayer::setPlayerSoundTrack(PCMPlayer::eSoundTrack track)
{
    if (pcmPlayer == NULL)
    {
        return ;
    }
    pcmPlayer->setSoundTrack(track);
}

void AudioPlayer::setReserveTime(float time)
{
    if (time <= 0)
    {
        return;
    }
    reserveTime = time;
}

void AudioPlayer::setVolume(qreal newVolume)
{
    if (pcmPlayer == NULL)
    {
        return;
    }
    if (newVolume < 0
        || newVolume > 1)
    {
        return;
    }
    pcmPlayer->setVolume(newVolume);
}

qreal AudioPlayer::volume()
{
    if (pcmPlayer == NULL)
    {
        return 0;
    }
    return pcmPlayer->volume();
}

void AudioPlayer::timerEvent(QTimerEvent *event)
{
    //step 1. state is not init or playing state ,return
    //        (can execute below code only ePlayingState and eInitState)
    if (playState != eInitState && playState != ePlayingState)
    {
//        AudioPlayerDebug("playState is %d", playState);
        return;
    }
    //step 2. judge audioFormat is parsed?
    if (playState == eInitState
        && !isAudioFormatParsed)
    {
        audioFormatParsed();
        return;
    }
    /*
     * Before obtaining AudioFormat, does not execute the following code
     */
    //step 3. judge queue whether init finish
    if (playState == eInitState
        && pcmPlayer != NULL
        && (pcmPlayer->isDataQueueFull() ||
            (reserveTime > 0 && reserveTime < pcmPlayer->getRemainTime())))
    {
        AudioPlayerDebug("pcm data is ready!");
        if (isPaused)       //when pcmplayer ready whether immediately start playing
        {
            playState = ePausedState;
            return;
        }
        else
        {
//            AudioPlayerDebug("start playing!");
            playState = ePlayingState;
            pcmPlayer->resume();
            return;
        }
    }
    //step 4. judge queue is full?
    if (pcmPlayer == NULL || pcmPlayer->isDataQueueFull())
    {
        return;
    }
//    AudioPlayerDebug("start add data!");
    //step 5. judge pcmplayer whether play finish
    if (playState != eInitState
        && pcmPlayer != NULL
        && pcmPlayer->isDataQueueEmpty()
        && !pcmPlayer->isPlaying())
    {
//        AudioPlayerDebug("pcmPlayer is out of data");
        if (indexReadPos >= endReadPos)  //(if source don't finish, return)
        {
            AudioPlayerDebug("play finished!");
            playState = eStopedState;
            endMusicTime = pcmPlayer->getMusicTime() + startTime;

            if (NULL != pcmPlayer)
            {
                pcmPlayer->stop();
                pcmPlayer->deleteLater();
                pcmPlayer = NULL;
            }
            if (NULL != sourceData)
            {
                sourceData->deleteLater();
                sourceData = NULL;
            }
            if (NULL != decoder)
            {
                decoder->deleteLater();
                decoder = NULL;
            }
            reset();
            emit onFinished();
        }
    }
    //step 6. judge resource finished?
    if (indexReadPos >= endReadPos)
    {
        return;
    }
    //step 7. add pcm data to pcmplayer
    addPcmDataToPcmPlayer();
}

float AudioPlayer::getCurrentMusicTime()
{
  //  AudioPlayerDebug("audioplayer getCurrentMusicTime()!");
    if (pcmPlayer == NULL)
    {
        if (endMusicTime == 0)
        {
            return startTime;
        }
        else
        {
            return endMusicTime;
        }
    }
    float time = startTime + pcmPlayer->getMusicTime();
   // AudioPlayerDebug("audioplayer getCurrentMusicTime():%f!",time);
    return time;
}

bool AudioPlayer::appendRAMData(QByteArray data)
{
    if (sourceData == NULL && sourceDataType != eSourceRAM)
    {
        return false;
    }
//    AudioPlayerDebug("setFileData length is %d", data.length());
    ((SourceRAM *)sourceData)->writeFileData(data);
    return true;
}
