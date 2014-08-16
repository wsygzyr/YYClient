#include "PkInfoManager.h"

QAtomicPointer<PkInfoManager> PkInfoManager::instance;
QMutex PkInfoManager::instanceMutex;

PkInfoManager::PkInfoManager()
{
    currentPkPlayerIndex = -1;
    nextPkPlayerIndex = -1;

    luckyWingProp.setID(PROP_ID_LUCKYWING);
    luckyWingProp.setIsActive(false);

    changeSpeechProp.setID(PROP_ID_TRANSFORMER);
    changeSpeechProp.setIsActive(false);

    pigProp.setID(PROP_ID_PIG);
    pigProp.setIsActive(false);
    pigProp.setUseNumber(0);

    kissProp.setID(PROP_ID_KISS);
    kissProp.setIsActive(false);
    kissProp.setUseNumber(0);

    scrawlProp.setID(PROP_ID_SCRAWL);
    scrawlProp.setIsActive(false);
    scrawlProp.setUseNumber(0);

    frogProp.setID(PROP_ID_FROG);
    frogProp.setIsActive(false);
    frogProp.setUseNumber(0);

    isSyncToServer = false;
}

PkInfoManager::~PkInfoManager()
{
    victoryRewardList.clear();
    PkPlayerList.clear();
}

PkInfoManager *PkInfoManager::getInstance()
{
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            instance = new PkInfoManager();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void PkInfoManager::destory()
{
    instanceMutex.lock();
    PKInfoManagerDebug("PkInfoManager destory");
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}


Player PkInfoManager::getMe() const
{
    return Me;
}

void PkInfoManager::setMe(const Player &value)
{
    instanceMutex.lock();
    Me = value;
    instanceMutex.unlock();
}

QList<Player> PkInfoManager::getPkPlayerList() const
{
    return PkPlayerList;
}

void PkInfoManager::setPkPlayerList(const QList<Player> &value)
{
    instanceMutex.lock();
    PkPlayerList = value;
    currentPkPlayerIndex = -1;
    instanceMutex.unlock();
}

Player PkInfoManager::getPkPlayer(const int &index)
{
    if(index >=0 && index < PkPlayerList.size())
        return PkPlayerList[index];
    else
        return  Player();
}

void PkInfoManager::setPkPlayer(Player &player, const int &index)
{
    instanceMutex.lock();
    PkPlayerList[index] = player;
    instanceMutex.unlock();
}

Player PkInfoManager::getPkPlayer(const qint64 &playerId)
{
    for(int i = 0 ;i < PkPlayerList.size(); i++)
    {
        if(PkPlayerList[i].getID() == playerId)
            return PkPlayerList[i];
    }
    return Player();
}

void PkInfoManager::setPkPlayer(Player &player, const qint64 &playerId)
{
    instanceMutex.lock();
    for(int i = 0 ; i < PkPlayerList.size() ; i++)
    {
        if(PkPlayerList[i].getID() == playerId)
        {
            PkPlayerList[i] = player;
            break;
        }
    }
    instanceMutex.unlock();
}

Song PkInfoManager::getCurrentSong() const
{
    PKInfoManagerDebug("getCurrentSong");
    return currentSong;
}

void PkInfoManager::setCurrentSong(const Song &value)
{
    instanceMutex.lock();
    currentSong = value;
    instanceMutex.unlock();
}

QList<VictoryReward> PkInfoManager::getVictoryRewardList() const
{
    return victoryRewardList;
}

void PkInfoManager::setVictoryRewardList(const QList<VictoryReward> &value)
{
    instanceMutex.lock();
    victoryRewardList = value;
    instanceMutex.unlock();
}

VictoryReward PkInfoManager::getVictoryReward(const int &index) const
{
    if (index >= 0 && index < victoryRewardList.size())
        return victoryRewardList[index];
    else
        return VictoryReward();
}

VictoryReward PkInfoManager::getVictoryReward(const QString &rewardName) const
{
    for(int i = 0 ;i < victoryRewardList.size() ;i++)
    {
        if(victoryRewardList[i].getName() == rewardName)
            return victoryRewardList[i];
    }
    return VictoryReward();
}

int PkInfoManager::getPopulateCount() const
{
    return populateCount;
}

void PkInfoManager::setPopulateCount(int value)
{
    instanceMutex.lock();
    populateCount = value;
    instanceMutex.unlock();
}

Player PkInfoManager::getCurrentPkPlayer()
{
    if(currentPkPlayerIndex == -1)
    {
        Player player;
        player.setID(-1);
        return player;
    }
    PKInfoManagerDebug("getCurrentPkPlayer %d %d", currentPkPlayerIndex, PkPlayerList.size());
    Q_ASSERT(PkPlayerList.size() >= currentPkPlayerIndex && "currentPkPlayerIndex cannot greater than PkPlayerList.size()");
    return PkPlayerList[currentPkPlayerIndex];
}

int PkInfoManager::getCurrentPkPlayerIndex() const
{
    return currentPkPlayerIndex;
}

void PkInfoManager::setCurrentPkPlayerIndex(int value)
{
    instanceMutex.lock();
    currentPkPlayerIndex = value;
    if (currentPkPlayerIndex + 1 < PkPlayerList.size())
        nextPkPlayerIndex = value + 1;
    else
        nextPkPlayerIndex = -1;
    instanceMutex.unlock();
}

int PkInfoManager::getMeSectionIndex()
{
    PKInfoManagerDebug("getMeSectionIndex");
    int meSectionIndex = -1;
    for(int i = 0; i < PkPlayerList.size(); i++)
    {
        if (PkPlayerList[i].getID() == Me.getID())
        {
            meSectionIndex = i;
            break;
        }
    }
    PKInfoManagerDebug("meSectionIndex is: %d", meSectionIndex);
    return meSectionIndex;
}

float PkInfoManager::getTimeOffset() const
{
    return timeOffset;
}

void PkInfoManager::setTimeOffset(float value)
{
    instanceMutex.lock();
    timeOffset = value;
    instanceMutex.unlock();
}

QList<Player> PkInfoManager::getYYonlineUserList() const
{
    return YYonlineUserList;
}

void PkInfoManager::setYYonlineUserList(const QList<Player> &value)
{
    instanceMutex.lock();
    YYonlineUserList = value;
    instanceMutex.unlock();
}

Player PkInfoManager::getPrepareSinger() const
{
    return prepareSinger;
}

void PkInfoManager::setPrepareSinger(const Player& player)
{
    instanceMutex.lock();
    prepareSinger = player;
    instanceMutex.unlock();
}

qint64 PkInfoManager::getChannelID() const
{
    return channelID;
}

void PkInfoManager::setChannelID(qint64 value)
{
    instanceMutex.lock();
    channelID = value;
    instanceMutex.unlock();
}

Player PkInfoManager::getHostPlayer() const
{
    return hostPlayer;
}

void PkInfoManager::setHostPlayer(const Player &value)
{
    instanceMutex.lock();
    hostPlayer = value;
    instanceMutex.unlock();
}

Prop PkInfoManager::getLeastBidProp() const
{
    return leastBidProp;
}

void PkInfoManager::setLeastBidProp(const Prop &value)
{
    instanceMutex.lock();
    leastBidProp = value;
    instanceMutex.unlock();
}
Prop PkInfoManager::getChangeSpeechProp() const
{
    return changeSpeechProp;
}

void PkInfoManager::setChangeSpeechProp(const Prop &value)
{
    instanceMutex.lock();
    changeSpeechProp = value;
    instanceMutex.unlock();
}

Prop PkInfoManager::getLuckyWingProp() const
{
    return luckyWingProp;
}

void PkInfoManager::setLuckyWingProp(const Prop &value)
{
    instanceMutex.lock();
    luckyWingProp = value;
    instanceMutex.unlock();
}

Prop PkInfoManager::getPigProp() const
{
    return pigProp;
}

void PkInfoManager::setPigProp(const Prop &value)
{
    instanceMutex.lock();
    pigProp = value;
    instanceMutex.unlock();
}

Prop PkInfoManager::getKissProp() const
{
    return kissProp;
}

void PkInfoManager::setKissProp(const Prop &value)
{
    instanceMutex.lock();
    kissProp = value;
    instanceMutex.unlock();
}

Prop PkInfoManager::getScrawlProp() const
{
    return scrawlProp;
}

void PkInfoManager::setScrawlProp(const Prop &value)
{
    instanceMutex.lock();
    scrawlProp = value;
    instanceMutex.unlock();
}

Prop PkInfoManager::getFrogProp() const
{
    return frogProp;
}

void PkInfoManager::setFrogProp(const Prop &value)
{
    instanceMutex.lock();
    frogProp = value;
    instanceMutex.unlock();
}

void PkInfoManager::resetProp()
{
    instanceMutex.lock();

    luckyWingProp.setID(PROP_ID_LUCKYWING);
    luckyWingProp.setIsActive(false);

    changeSpeechProp.setID(PROP_ID_TRANSFORMER);
    changeSpeechProp.setIsActive(false);

    pigProp.setID(PROP_ID_PIG);
    pigProp.setIsActive(false);
    pigProp.setUseNumber(0);

    kissProp.setID(PROP_ID_KISS);
    kissProp.setIsActive(false);
    kissProp.setUseNumber(0);

    scrawlProp.setID(PROP_ID_SCRAWL);
    scrawlProp.setIsActive(false);
    scrawlProp.setUseNumber(0);

    frogProp.setID(PROP_ID_FROG);
    frogProp.setIsActive(false);
    frogProp.setUseNumber(0);

    instanceMutex.unlock();
}
int PkInfoManager::getNextPkPlayerIndex() const
{
    return nextPkPlayerIndex;
}

void PkInfoManager::setNextPkPlayerIndex(int value)
{
    nextPkPlayerIndex = value;
}
int PkInfoManager::getPrepareSectionIndex() const
{
    return prepareSectionIndex;
}

void PkInfoManager::setPrepareSectionIndex(int value)
{
    instanceMutex.lock();
    prepareSectionIndex = value;
    instanceMutex.unlock();
}

Prop PkInfoManager::getBidUsedProp() const
{
    return bidUsedProp;
}

void PkInfoManager::setBidUsedProp(const Prop &value)
{
    bidUsedProp = value;
}

int PkInfoManager::getMillisecondsDiff() const
{
    return millisecondsDiff;
}

void PkInfoManager::setMillisecondsDiff(int value)
{
    instanceMutex.lock();
    millisecondsDiff = value;
    instanceMutex.unlock();
}
bool PkInfoManager::getIsSyncToServer() const
{
    return isSyncToServer;
}

void PkInfoManager::setIsSyncToServer(bool value)
{
    isSyncToServer = value;
}

void PkInfoManager::setGameId(quint32 id)
{
    gameId = id;
}

quint32 PkInfoManager::getGameId() const
{
    return gameId;
}
QList<int> PkInfoManager::getLastSingEndRetcode() const
{
     return lastSingEndRetcode;
}

void PkInfoManager::setLastSingEndRetcode(QList<int> singEndCodeList)
{
    lastSingEndRetcode = singEndCodeList;
}

QList<SingReadyResult> PkInfoManager::getResDownloadRetcode() const
{
     return resDownloadRetcode;
}

void PkInfoManager::setResDownloadRetcode(QList<SingReadyResult> downloadCodeList)
{
    //resDownloadRetcode = downloadCodeList;
    QList<int> tmp;

    for(int i = 0; i <downloadCodeList.size(); i++ )
    {
        bool exsit = false;
        for(int j = 0; j <resDownloadRetcode.size(); j++ )
        {
            PKInfoManagerDebug("resDownloadRetcode index1=%d, code2=%d",resDownloadRetcode[j].sectionIndex, resDownloadRetcode[j].retcode);
            if(downloadCodeList[i].sectionIndex == resDownloadRetcode[j].sectionIndex)
            {
                resDownloadRetcode[j].retcode = downloadCodeList[i].retcode;
                exsit = true;
                continue;
            }
        }
        if(!exsit)
        {
            tmp.append(i);
        }
    }

    for(int n=0; n < tmp.size(); n++)
    {
        int k = tmp[n];
        resDownloadRetcode.push_back(downloadCodeList[k]);
    }

    for(int k=0; k<resDownloadRetcode.size(); k++)
    {
        PKInfoManagerDebug("current resource download state index=%d, retcode=%d", resDownloadRetcode[k].sectionIndex, resDownloadRetcode[k].retcode);
    }
}

int PkInfoManager::getCurPkPlayerDownloadState(int singerIndex)
{
   QList<SingReadyResult> downloadList = getResDownloadRetcode();
   int singerDownloadState = 1;
   for(int i = 0; i < downloadList.size(); i++)
   {
       if(downloadList[i].sectionIndex == singerIndex)
       {
           singerDownloadState = downloadList[i].retcode;
           break;
       }
   }

   return singerDownloadState;
}

QAtomicPointer<P2Y_PKInfo> P2Y_PKInfo::instance;
QMutex P2Y_PKInfo::instanceMutex;

P2Y_PKInfo::P2Y_PKInfo() :
    startPKCount(1),
    joinPKUserCount(0),
    bRecvPlayPK(true)
{
}

P2Y_PKInfo::~P2Y_PKInfo()
{
}


P2Y_PKInfo *P2Y_PKInfo::getInstance()
{
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            instance = new P2Y_PKInfo();
        }
        instanceMutex.unlock();
    }
    return instance;
}


void P2Y_PKInfo::destory()
{
    instanceMutex.lock();
    PKInfoManagerDebug("P2Y_PKInfo destory");
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}
