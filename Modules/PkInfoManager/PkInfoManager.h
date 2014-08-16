#ifndef PKINFOMANAGER_H
#define PKINFOMANAGER_H
#include "Player/Player.h"
#include "Song/Song.h"
#include "DataType/Prop/Prop.h"
#include "VictoryReward/VictoryReward.h"
#include <QList>
#include <QMutex>
#include <Qtime>

#define DEBUG_PKINFO_MANAGER  1
#if DEBUG_PKINFO_MANAGER
#include <QDebug>
#define PKInfoManagerDebug(format,...) qDebug("%s,LINE: %d -->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define PKInfoManagerDebug(format,...)
#endif

typedef struct
{
    int sectionIndex;
    int retcode;
}SingReadyResult;

class PkInfoManager
{
    /*
 *  singleton
 */
private:
    PkInfoManager();
    ~PkInfoManager();
private:
    static QAtomicPointer<PkInfoManager> instance;
    static QMutex instanceMutex;
public:
    static PkInfoManager *getInstance();
    static void destory();

public:
    Player                   getMe() const;
    void                     setMe(const Player &value);

    Player                   getHostPlayer() const;
    void                     setHostPlayer(const Player &value);

    QList<Player>            getPkPlayerList() const;
    void                     setPkPlayerList(const QList<Player> &value);

    Player                   getPkPlayer(const int &index);
    void                     setPkPlayer(Player &, const int &index);

    Player                   getPkPlayer(const qint64 &playerId);
    void                     setPkPlayer(Player &,const qint64 &playerId);

    Song                     getCurrentSong() const;
    void                     setCurrentSong(const Song &value);

    QList<VictoryReward>     getVictoryRewardList() const;
    void                     setVictoryRewardList(const QList<VictoryReward> &value);

    VictoryReward            getVictoryReward(const int &index) const;
    VictoryReward            getVictoryReward(const QString &rewardName) const;

    int                      getPopulateCount() const;
    void                     setPopulateCount(int value);

    Player                   getCurrentPkPlayer();
    int                      getCurrentPkPlayerIndex() const;
    void                     setCurrentPkPlayerIndex(int value);

    int                      getMeSectionIndex(); 

    float getTimeOffset() const;
    void setTimeOffset(float value);

    QList<Player> getYYonlineUserList() const;
    void setYYonlineUserList(const QList<Player> &value);

    Player getPrepareSinger() const;
    void setPrepareSinger(const Player &);

    qint64 getChannelID() const;
    void setChannelID(qint64 value);

    QList<Prop> getPKRoomPropList() const;
    void setPKRoomPropList(const QList<Prop> &value);

    Prop getLeastBidProp() const;
    void setLeastBidProp(const Prop &value);

    Prop getChangeSpeechProp() const;
    void setChangeSpeechProp(const Prop &value);

    Prop getLuckyWingProp() const;
    void setLuckyWingProp(const Prop &value);

    Prop getPigProp() const;
    void setPigProp(const Prop &value);

    Prop getKissProp() const;
    void setKissProp(const Prop &value);

    Prop getScrawlProp() const;
    void setScrawlProp(const Prop &value);

    Prop getFrogProp() const;
    void setFrogProp(const Prop &value);

    void resetProp();

    int getNextPkPlayerIndex() const;
    void setNextPkPlayerIndex(int value);

    int getPrepareSectionIndex() const;
    void setPrepareSectionIndex(int value);


    Prop getBidUsedProp() const;
    void setBidUsedProp(const Prop &value);

    int getMillisecondsDiff() const;
    void setMillisecondsDiff(int value);

    bool getIsSyncToServer() const;
    void setIsSyncToServer(bool value);

	quint32 getGameId() const;
    void setGameId(quint32 id);
    //sunly
    QList<int> getLastSingEndRetcode() const;
    void setLastSingEndRetcode(QList<int> singEndCodeList);

    QList<SingReadyResult> getResDownloadRetcode() const;
    void setResDownloadRetcode(QList<SingReadyResult> downloadCodeList);

    int getCurPkPlayerDownloadState(int singerIndex);

private:
    qint64                   channelID;
    Player                   Me;
    Player                   hostPlayer;
    QList<Player>            PkPlayerList;
    QList<Player>            YYonlineUserList;
    Song                     currentSong;
    QList<VictoryReward>     victoryRewardList;
    int                      populateCount;
    int                      currentPkPlayerIndex;
    int                      nextPkPlayerIndex;

    Player                   prepareSinger;
    int                      prepareSectionIndex;
    float                    timeOffset;
    Prop                     leastBidProp;  // keep the least bid prop hint received from NodeServer
    Prop                     bidUsedProp;

    Prop                     luckyWingProp;
    Prop                     changeSpeechProp;
    Prop                     pigProp;
    Prop                     kissProp;
    Prop                     scrawlProp;
    Prop                     frogProp;

    int                      millisecondsDiff;
    bool                     isSyncToServer;
	quint32                  gameId;
	
    QList<int>               lastSingEndRetcode;  //sunly
    QList<SingReadyResult>   resDownloadRetcode;
};




struct P2Y_PKInfo
{
    /*
 *  singleton
 */
private:
    P2Y_PKInfo();
    ~P2Y_PKInfo();
private:
    static QAtomicPointer<P2Y_PKInfo> instance;
    static QMutex instanceMutex;
public:
    static P2Y_PKInfo *getInstance();
    static void destory();

public:
    int startPKCount;
    int startPKSuccessCount;
    int joinPKUserCount;
    bool bZeroStart;
    QDateTime pKStartTime;
    QDateTime pKEndTime;
    int pKDuration;
    bool bRecvPlayPK;
    bool bAcceptPlayPK;
};


#endif // PKINFOMANAGER_H
