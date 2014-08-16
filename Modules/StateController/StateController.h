#ifndef STATECONTROLLER_H
#define STATECONTROLLER_H

#include <QObject>
#include <QMutex>
#include <QList>
#include <QTimer>

#define DEBUG_STATE_CONTROLLER     1

#if DEBUG_STATE_CONTROLLER
#define StateControllerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#include <QDebug>
#else
#define StateControllerDebug(format,...)
#endif

#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include "Modules/StateController/EventHandler.h"
#include "Modules/ResManager/ResManager.h"


#define  RECV_SING_CHANGE   0x00000001
#define  RECV_XML_OK        0x00000010
#define  RECV_SECTION_OK    0x00000100
#define  RECV_MP3_OK        0x00001000
#define  RECV_XML_ING       0x00010000
#define  RECV_SECTION_ING   0x00100000
#define  RECV_MP3_ING       0x01000000
#define  RECV_ERROR         0x10000000

//define resource state
typedef enum
{
    resDownloadFailed = -1,
    resDownloadSucceed = 0,
    resDownloadUncomplete = 1
}eResDownloadState;

class AudienceMainWidget;

class StateController : public QObject
{
    Q_OBJECT
private:
    StateController();
    ~StateController();

/*
 *  singleton
 */
private:
    static QAtomicPointer<StateController> instance;
    static QMutex instanceMutex;

public:
    static StateController *getInstance();
    static void destory();

public:
    /*
     * Defined  states that it's exsit all possible.
    */
    //state type
    typedef enum
    {
        NoneState = 0,
        ConnectStatusState = 1,
        LoginStatusState = 2,
        LoginedStatusState = 3,
        InviteWaitState = 4,
        UnconnectNodeState = 5,
        ConnectNodeState = 6,
        LoginNodeState = 7,
        LoginedNodeState = 8,
        BidState = 9,
        SettleState = 10,
        PkState = 11,
        PkEndState = 12
    }StateType;

    //login status/node server result code
    enum LoginResCode{
        LoginSucceed = 0,
        LoginFailed = -1,
        GameNoStart = -6,
        otherLoginError = -7
    };

    /*
     * interface
     */
    void startStateMachine();
    void stopStateMachine();
    StateType getState() const { return curPluginState; }
    void setNextState(StateType state) { curPluginState = state; }
    bool needProcessEvent(EventHandler::EventType event);
    void initConnectInterface();

private:
    StateType              curPluginState;
    PkInfoManager          *pkinfo;
    EventHandler           *eventHandler;
    NodeNotificationCenter *nodeNC;
    ResManager             *resManager;
    quint32                resState; //ressource download result

signals:
    void onStateLoginStatusServer(int);
    void onStateLoginNodeServer(int);
    void onStateLoginResult(int);
    void onStatePKInvite(Player, Song, Prop, QList<VictoryReward>, int acceptedPlayer);
    //void onStateAcceptInviteResult(bool);
    void onStateBidStart();
    void onStateBidInfo(int biderNumber, Prop leastProp, Prop expectIncome);
    void onStateBidStop(int);
    void onStateBidConfirm(Prop);
    void onStateBidSettlement(int);
    void onStateFirstSingReady(int);
    void onStateSingReady(QList<SingReadyResult>);
    void onStatePKStart(QList<Player>);
    void onStateSingPrepare(int);
    //void onStateSingChange(int, QList<int>);
    void onStateSingChange(int, QList<int>, int, quint32);
    void onStatePKResult(QList<Score>, int, Prop, QList<int>);
    void onStateSelectReward(VictoryReward);
    void onStateUpdateHeat(int);
    void onStateSingTimeOffset(float);
    void onStateScore(Score, Score);
    void onStatePropConfirm(Prop);
    void onStatePropActive(Player, Prop);
    void onStateErrorEvent(int);
    void onStateUploadLog();
    void onStateUploadDumpFile();
    void onStateReconnectServer();
    void onStateBidFailed();

//public:
private slots:
    //for NodeNotificationCenter and upper layer call
    void handleNodeNotificationCenterOnLoginStatusServer(int);
    void handleNodeNotificationCenterOnLoginNodeServer(int);
    void handleNodeNotificationCenterOnLoginStatusResult(int);
    void handleNodeNotificationCenterOnLoginNodeResult(int);
    void handleNodeNotificationCenterOnPKInvite(Player, Song,
                                                Prop, QList<VictoryReward>,
                                                int acceptedPlayer);
    void handleNodeNotificationCenterOnBidInfo(int biderNumber, Prop leastProp,
                                               Prop expectIncome);
    void handleNodeNotificationCenterOnBidStop(int);
    void handleNodeNotificationCenterOnBidConfirm(Prop);
    void handleNodeNotificationCenterOnBidSettlement(int);
    void handleNodeNotificationCenterOnSingReady(QList<SingReadyResult>);
    void handleNodeNotificationCenterOnPKStart(QList<Player>);\
    void handleNodeNotificationCenterOnSingPrepare(int);
    void handleNodeNotificationCenterOnSingChange(int, QList<int>);
    void handleNodeNotificationCenterOnPKResult(QList<Score>, int, Prop, QList<int>);
    void handleNodeNotificationCenterOnSelectReward(VictoryReward);
    void handleNodeNotificationCenterOnUpdateHeat(int);
    void handleNodeNotificationCenterOnSingTimeOffset(float);
    void handleNodeNotificationCenterOnScore(Score, Score);
    void handleNodeNotificationCenterOnPropConfirm(Prop);
    void handleNodeNotificationCenterOnPropActive(Player, Prop);
    void handleNodeNotificationCenterOnErrorEvent(int);
    void handleNodeNotificationCenterOnUploadLog();
    void handleNodeNotificationCenterOnUploadDumpFile();
    void handleNodeNotificationCenterOnConnectServerResult(NodeNotificationCenter::eServerType, NodeNotificationCenter::eConnectErrorCode);
    void handleNodeNotificationCenterOnRoominfoPk(int status, int bidnum, Prop leastProp,
                          Prop expIncome, int completed, int heat);

    //resmanager
    void handleResManageronDownloadResPrepared(QString,bool);


public:
    bool sendStateAcceptInvite();
    void sendStatePkInvite(Song song, Prop bidProp, QList<VictoryReward> vrList);
    void sendStateBidStop();
    void sendStateBidConfirm(Prop prop, int retcode, QString orderNumber, QString signature);
    void sendStateSingEnd(int errorcode);
    void sendPkEnd();
private:
    void timerEvent(QTimerEvent *downloadTimer); //download timer
    //void handleNoneState();
    void resetResState();
    int downLoadResTimer;
    int totalTime;
};

#endif // STATECONTROLLER_H
