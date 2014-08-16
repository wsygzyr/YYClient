#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <QObject>
#include <QList>
#include <QMutex>

#include "DataType/Player/Player.h"
#include "DataType/Song/Song.h"
#include "DataType/VictoryReward/VictoryReward.h"
#include "DataType/Score/Score.h"
#include "DataType/Prop/Prop.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/ResManager/ResManager.h"

#define DEBUG_EVENT_HANDLER     1

#if DEBUG_EVENT_HANDLER
#define EventHandlerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#include <QDebug>
#else
#define EventHandlerDebug(format,...)
#endif

class EventHandler : public QObject
{
    Q_OBJECT
public:
    explicit EventHandler(QObject *parent = 0);
    ~EventHandler();

public:
    //event type
    typedef enum
    {
        StatusSyncTimeEvent = 0,
        LoginStatusResultEvent = 1,
        PkInviteEvent = 2,
        AcceptInviteEvent = 3,
        NodeSyncTimeEvent = 4,
        LoginNodeResultEvent = 5,
        RoomInfoEvent = 6,
        BidStopEvent = 7,
        PkStartEvent = 8,
        PkResultEvent = 9,
        StatusNetworkDisconnEvent = 10,
        NodeNetWorkDisconnEvent = 11
    }EventType;

    EventType getCurEvent() const { return curEvent; }
    void setCurEvent(EventType event) { curEvent = event; }

private:
    EventType curEvent;
    PkInfoManager *pkInfo;
    ResManager *resManager;
public:
    void downloadRes4PkState();
    int checkResDownloadResult(QString keyWord, bool error);
};

#endif // EVENTHANDLER_H
