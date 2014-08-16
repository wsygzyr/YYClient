#ifndef PLAYER_H
#define PLAYER_H

#include <QUrl>
#include "Prop/Prop.h"
#include "VictoryReward/VictoryReward.h"
#include "Score/Score.h"
#define PLAYER_DEBUG    1

#if PLAYER_DEBUG
#include <QDebug>
#define PlayerDebug(format, ...) qDebug("%s, LINE: %d --->"format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define PlayerDebug(format, ...)
#endif

class Player
{

public:
    Player();
private:
    QString       name;
    int           ID;
    bool          isHost;
    int           sectionIndex;
    VictoryReward victoryDemand;
    //QList<Prop>   propList;

    Score         sumScore;
    QString       picFileSvPath;
    QString       picFileName;
    Prop          propInfo;

public:
    QString     getName() const;
    void        setName(const QString &name);

    bool        getIsHost() const;
    void        setIsHost(bool isHost);

//    QUrl        getHeadPicUrl() const;
//    void        setHeadPicUrl(const QUrl &headPicUrl);

    QString     getPicFileSvPath() const;
    void        setPicFileSvPath(const QString &value);

    VictoryReward getVictoryDemand() const;
    void          setVictoryDemand(const VictoryReward &value);

    int         getID() const;
    void        setID(const int &value);

    QString     getPicFileName() const;
    void        setPicFileName(const QString &value);

    Score       getSumScore() const;
    void        setSumScore(const Score &value);

    int         getSectionIndex() const;
    void        setSectionIndex(int value);

//    QList<Prop> getPropList() const;
//    void        setPropList(const QList<Prop> &value);
    void        updatePropList(const Prop usedProp);

    QString     getShortCutName(QFont , int &) const;

    Prop       getProp() const;
    void       setProp(const Prop &value);
};

#endif // PLAYER_H
