#include "ResManager.h"
#include "Modules/DecodeAudioFile/DecodeAudioFile.h"

QAtomicPointer<ResManager> ResManager::instance;
QMutex ResManager::instanceMutex;
ResManager::ResManager(QObject *parent) :
    QObject(parent)
{
    httpDownload = HttpDownload::getInstance();
    pkInfo = PkInfoManager::getInstance();
    cfg = ConfigHelper::getInstance();
    thread = new QThread;
    this->moveToThread(thread);

    thread->start();
    connect(thread , SIGNAL(started()) , this , SLOT(handleThreadOnStart()));

}

ResManager::~ResManager()
{
    mp3Data.clear();
    thread->quit();
    thread->deleteLater();
    thread = NULL;
}

ResManager *ResManager::getInstance()
{
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            instance = new ResManager();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void ResManager::destory()
{
    instanceMutex.lock();
    ResManagerDebug("ResManager destory");
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}
//cfg->getPluginPath()+cfg->getDownloadFileSavePath()+"/song.pcm"
//cfg->getPluginPath()+cfg->getDownloadFileSavePath()+"/songcut.pcm"
void ResManager::cutPcm(QString inFileName, QString outFileName, int bits, int channel, int rate, int startTime, int endTime)
{
    ResManagerDebug("cutPcm");
    fileCut = new FileCut();
    fileCut->setFormat(rate, channel, bits);
    fileCut->splitFile(inFileName, outFileName, startTime, endTime);
    delete fileCut;
    fileCut = NULL;
}


void ResManager::reset()
{
    ResManagerDebug("ResManager reset");
    dlResState.clear();

    mp3Data.clear();
}

void ResManager::addDownLoadResRequest(QString fileName, QUrl url, QString filePath)
{
    ResManagerDebug("addDownLoadResRequest");
    if(!dlResState.contains(fileName))
    {
        QDir dir;
        if(dir.exists(cfg->getPluginPath()+cfg->getDownloadFileSavePath() + "/"+ QString::number(pkInfo->getMe().getID()) + "_song.pcm"))
        {
            dir.remove(cfg->getPluginPath()+cfg->getDownloadFileSavePath()+"/"+ QString::number(pkInfo->getMe().getID()) + "_song.pcm");
        }
        dlResState.insert(fileName , NOT_DL);
        httpDownload->addRequest(fileName , url , filePath);
    }
    else
    {
        if(dlResState[fileName] != NOT_DL)
        {
            handleHttpDownloadOnDownloadFinished(fileName , (dlResState[fileName]== DL_FAIL) ? true : false , -1);
        }
    }
}
int ResManager::getSampleRate() const
{
    return sampleRate;
}

void ResManager::setSampleRate(int value)
{
    sampleRate = value;
}
int ResManager::getChannels() const
{
    return channels;
}

void ResManager::setChannels(int value)
{
    channels = value;
}
int ResManager::getBits() const
{
    return bits;
}

void ResManager::setBits(int value)
{
    bits = value;
}

void ResManager::handleThreadOnStart()
{
    thread->setObjectName("resManagerThread-"+ QString::number((int)QThread::currentThreadId()));
    connect(httpDownload , SIGNAL(onFinish(QString,bool,qint64)) , this , SLOT(handleHttpDownloadOnDownloadFinished(QString,bool,qint64)));
    connect(httpDownload , SIGNAL(onProgress(QString,QByteArray,qint64)) , this , SLOT(handleHttpDownloadOnProgress(QString , QByteArray , qint64 )));
    connect(httpDownload, SIGNAL(onClearData(QString)), this , SLOT(handleClearErrorData(QString)));
}

void ResManager::handleHttpDownloadOnDownloadFinished(QString keyword, bool isError ,qint64 allByteRead)
{
    ResManagerDebug("ResManager handleHttpDownloadOnDownloadFinished");
    dlResState[keyword] = isError ? DL_FAIL : DL_OK;
    if(!isError && keyword == pkInfo->getCurrentSong().getAccompanyFileName() && allByteRead != -1)
    {
        DecodeAudioFile dec;
        if (!dec.decode(mp3Data,cfg->getPluginPath()+cfg->getDownloadFileSavePath()+ "/"+ QString::number(pkInfo->getMe().getID()) +"_song.pcm"))
        {
            emit onDownloadResPrepared(keyword , true);
            return;
        }
        this->sampleRate = dec.getSampleRate();
        this->channels = dec.getChannel();
        this->bits = dec.getSampleSize();

//        writeWavHeader(pcmFile ,allByteRead);
    }
    emit onDownloadResPrepared(keyword , isError);

}

void ResManager::handleHttpDownloadOnProgress(QString keyword, QByteArray data, qint64 allByteRead )
{ 
    if(keyword == pkInfo->getCurrentSong().getLyricFileName())
    {
        Song song = pkInfo->getCurrentSong();
        song.addLyricXmlData(data);
        //ResManagerDebug("resmanger 2222 data.size=%d, songid=%s", data.size(), song.getSongID().toUtf8().data());
        pkInfo->setCurrentSong(song);
    }
    if(keyword == pkInfo->getCurrentSong().getSectionFileName())
    {
        Song song = pkInfo->getCurrentSong();
        song.addSectionXmlData(data);
        pkInfo->setCurrentSong(song);
    }
    if(keyword == pkInfo->getCurrentSong().getAccompanyFileName())
    {
        mp3Data.append(data);
    }
}

void ResManager::handleClearErrorData(QString keyword )
{
    if(keyword == pkInfo->getCurrentSong().getSectionFileName())
    {
        ResManagerDebug("clear SectionXml");
        Song song = pkInfo->getCurrentSong();
        song.clearSectionXmlData();
        pkInfo->setCurrentSong(song);
    }
    if(keyword == pkInfo->getCurrentSong().getLyricFileName())
    {
        ResManagerDebug("clear LyricXml");
        Song song = pkInfo->getCurrentSong();
        song.clearLyricXmlData();
        pkInfo->setCurrentSong(song);
    }
    if(keyword == pkInfo->getCurrentSong().getAccompanyFileName())
    {
        ResManagerDebug("clear mp3");
        mp3Data.clear();
    }
}
