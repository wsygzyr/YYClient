#include "SesMarkCaller.h"
#include "SES3DApi.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/LyricParse/LyricParse.h"
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"AdvAPI32.lib")

/*
 *  callbacks from libSES
 */

void handleSESOnError(void* response, void* userData)
{
    SESMarkCaller *caller = (SESMarkCaller *)userData;

    caller->notifyOnError(true);
}

//单句分数发送信号
void handleSESOnScore(void* response, void* userData)
{
    SESMarkCallerDebug("onScore!");

    ScoreResponse *s = (ScoreResponse *)response;
    SESMarkCallerDebug("s->retCode is: %d, s->sentenceIndex is: %d", s->retCode, s->sentenceIndex);

    SESMarkCaller *caller = (SESMarkCaller *)userData;

    Score score(s->pitchScore,
                s->lyricScore,
                s->rhythmScore,
                s->sentenceScore,
                s->sentenceIndex);

    if (s->retCode != 0)
    {
        caller->notifyOnScore(true, score);
        return;
    }

    caller->notifyOnScore(false, score);
}


void SESMarkCaller::notifyOnError(bool error)
{
    SESMarkCallerDebug("onReady! error is: %s", error ? "true" : "false");

    if (error)
    {
        emit onError();
        return;
    }
}

void SESMarkCaller::notifyOnScore(bool error, Score score)
{
    SESMarkCallerDebug("onScore! error is: %s, score.getSentenceIndex() is: %d",
                       error ? "true" : "false",
                       score.getSentenceIndex());
    if (error)
    {
        emit onError();
        return;
    }

    calcPropScore(score);

    scoreList.append(score);


    Score sumScore = calcSumScore();
    emit onScore(score, sumScore);

}


SESMarkCaller::SESMarkCaller(QObject *parent) :
    QObject(parent)
{    
    ConfigHelper *cfg = ConfigHelper::getInstance();

    lyricParse = LyricParse::getInstance();

    info = PkInfoManager::getInstance();
}

SESMarkCaller::~SESMarkCaller()
{
}

QAtomicPointer<SESMarkCaller> SESMarkCaller::instance;
QMutex SESMarkCaller::instanceMutex;
SESMarkCaller* SESMarkCaller::getInstance()
{
    SESMarkCallerDebug("getInstance!");
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            SESMarkCallerDebug("create instance!");
            instance = new SESMarkCaller();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void SESMarkCaller::destroy()
{
    SESMarkCallerDebug("SESMarkCaller destroy");
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = NULL;
    }
    instanceMutex.unlock();
}

void SESMarkCaller::calcPropScore(Score &score)
{
    float propLuckyWingScore = (info->getLuckyWingProp().getIsActive() ? 1.0 : 0) * score.getTotalScore() * 0.1;
    float propPigScore = 0 - info->getPigProp().getUseNumber();
    float propKissScore = 0 - info->getKissProp().getUseNumber() * 2;
    float propFrogScore = 0 - info->getFrogProp().getUseNumber();

    Prop pigProp = info->getPigProp();
    pigProp.setUseNumber(0);
    info->setPigProp(pigProp);
    Prop kissProp = info->getKissProp();
    kissProp.setUseNumber(0);
    info->setKissProp(kissProp);
    Prop frogProp = info->getFrogProp();
    frogProp.setUseNumber(0);
    info->setFrogProp(frogProp);

    SESMarkCallerDebug("propLuckyWingScore is: %f, propPigScore is: %f, propKissScore is: %f",
                       propLuckyWingScore, propPigScore, propKissScore);

    float propScore = propLuckyWingScore + (((propPigScore + propKissScore + propFrogScore) < -20) ? -20 : (propPigScore + propKissScore + propFrogScore));

    SESMarkCallerDebug("propScore is: %f", propScore);
    score.setPropScore(propScore);
}

Score SESMarkCaller::calcSumScore()
{
    float pitchScore    = 0.f;
    float lyricScore    = 0.f;
    float rhythmScore   = 0.f;
    float propScore     = 0.f;
    float totalScore    = 0.f;

    for(int i = 0; i < scoreList.size(); i++)
    {
        Score score = scoreList.at(i);
        pitchScore  += score.getPitchScore();
        lyricScore  += score.getLyricScore();
        rhythmScore += score.getRhythmScore();
        propScore   += score.getPropScore();
        totalScore  += score.getTotalScore();
    }

    pitchScore = pitchScore / (lyricParse->getSectionLastSentenceIndex(sectionIndex) - lyricParse->getSectionFirstSentenceIndex(sectionIndex) );
    lyricScore = lyricScore / (lyricParse->getSectionLastSentenceIndex(sectionIndex) - lyricParse->getSectionFirstSentenceIndex(sectionIndex) );
    rhythmScore = rhythmScore / (lyricParse->getSectionLastSentenceIndex(sectionIndex) - lyricParse->getSectionFirstSentenceIndex(sectionIndex) );
    totalScore = (totalScore + propScore) / (lyricParse->getSectionLastSentenceIndex(sectionIndex) - lyricParse->getSectionFirstSentenceIndex(sectionIndex) );

    Score sumScore(pitchScore, lyricScore, rhythmScore, totalScore, -1);
    sumScore.setPropScore(propScore);

    return sumScore;
}

bool SESMarkCaller::start(int sectionIndex, int samplerate)
{
    SESMarkCallerDebug("start! sectionIndex is: %d", sectionIndex);

    ConfigHelper *cfg = ConfigHelper::getInstance();

    SetGetSTScoreErrorCallBack(handleSESOnError);
    SetGet3DOnlineScoreCallBack(handleSESOnScore);


    if (scoreList.count() > 0)
    {
        scoreList.clear();
    }

    recordTimeOffset = lyricParse->getSectionBeginTime(sectionIndex);
    pcmBytesCount = 0;
    this->sectionIndex = sectionIndex;
    sentenceIndex = lyricParse->getSectionFirstSentenceIndex(sectionIndex);

    SESMarkCallerDebug(" path is %s , sampleRate is %d , offset is %f , index is %d",
                       QString(cfg->getPluginPath() + "SES/").toUtf8().data(),
                       samplerate,
                       recordTimeOffset / 1000 ,
                       lyricParse->getRealSentenceIndex( sentenceIndex ));
    int ret = SESInit(lyricParse->getLyricXmlData().toLocal8Bit().data(),
            lyricParse->getLyricXmlData().length() ,
            QString(cfg->getPluginPath() + "SES/").toLocal8Bit().data(),
            samplerate,
            recordTimeOffset / 1000.0 ,
            lyricParse->getRealSentenceIndex( sentenceIndex ),
            QString::number(PkInfoManager::getInstance()->getMe().getID()).toUtf8().data()
            );
    SESMarkCallerDebug("ret is %d" , ret);
    return (ret == 0) ? true : false ;
}

void SESMarkCaller::stop()
{
    SESMarkCallerDebug("stop!");
    SESUnInit();
}

void SESMarkCaller::sendAudio(QByteArray pcmData)
{
//    SESMarkCallerDebug("pcmdata length is:%d",pcmData.length()/2);

    if (0 != SESInsertPcm((short*)pcmData.data(), pcmData.length() / 2 , this))
    {
        SESMarkCallerDebug("send audio data failed!");
        return;
    }
//    SESMarkCallerDebug("send audio data succeed!");

    pcmBytesCount += pcmData.length();
//    SESMarkCallerDebug("pcmBytesCount is: %d", pcmBytesCount);
    float recorderTime = recordTimeOffset + (float)pcmBytesCount / (float)22050 / (float)(16/8) / 1 * 1000;

//    SESMarkCallerDebug("recorderTime is: %fms", recorderTime);

}











