#ifndef NODENOTIFICATIONCENTER_H
#define NODENOTIFICATIONCENTER_H

#include <QtCore>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QList>


#include "node.h"

#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/PkInfoManager/PkInfoManager.h"

#include "DataType/Player/Player.h"
#include "DataType/Song/Song.h"
#include "DataType/VictoryReward/VictoryReward.h"
#include "DataType/Score/Score.h"
#include "DataType/Prop/Prop.h"

#include "UI/ExceptionWidget/ExceptionWidget.h"


#define NODENOTIFICATION_DEBUG   1
#if NODENOTIFICATION_DEBUG
#include <QDebug>
#define NodeNotificationCenterDebug(format, ...) qDebug("%s, LINE: %d --->"format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define NodeNotificationCenterDebug  /\
/
#endif

class NodeNotificationCenter : public QObject
{
    Q_OBJECT

    NodeNotificationCenter();
    ~NodeNotificationCenter();


/*
 *  singleton
 */

private:
    static QAtomicPointer<NodeNotificationCenter> instance;
    static QMutex instanceMutex;
public:
    static NodeNotificationCenter *getInstance();
    static void destory();

/*
 *  thread
 */
private:
    QThread *thread;
signals:
    void onStop();
private slots:
    void handleThreadOnStarted();
/*
 *  modules
 */
private:
    ConfigHelper *cfg;
    PkInfoManager *info;    

/*
 *  Node
 */
private:
    Node *nodeServer;
    Node *statusServer;

    QString     nodeServerIP;
    int         nodeServerPort;

    QString     statusServerIP;
    int         statusServerPort;

    int         channelID;
    Player      me;

    int         timerId;

    enum eNodeState
    {
        NodeUnconnected = 0,
        NodeConnecting,
        NodeConnected,
        NodeError
    };

    // add by xianli, volatile may solve the "while(1)" bug in method "start"
    volatile eNodeState nodeState;

    QLinkedList<QByteArray> *sendMsgList;

public:
    bool start(const QString nodeServerIP, int nodeServerPort,
               const QString statusServerIP, int statusServerPort,
               int channelID, Player me);

    bool stop();

    enum error
    {
        sendMessageFailed,
        recvMessageFailed,
        connectionBroken
    };

    //server type
    enum eServerType
    {
        statusServerType,
        nodeServerType
    };

    //singend error code
    enum eSingEndErrorCode
    {
        resDownloadError = -1,
        resInitError = -2,
        resOpenError = -3
    };

    enum eSingChangeErrorCode
    {
        changeSingNormal = 0,
        lastSingDownloadFailed = 1,
        lastSingExit = 2,
        lastSingChangeSpeechException = 3,
        lastSingHeartbeatTimeout = 4,
        noSingendChangeAuto = 5,
        deaultErrorCode = -1
    };

    //socket connect error code
    enum eConnectErrorCode
    {
        connectSucceed,
        connectFailed
    };


    //room current state
    enum eRoomState
    {
        SpareTimeState,
        BidState,
        SettlementState,
        PkState
    };

private slots:
    void handleNodeOnDisconnected();
signals:
    void onError(NodeNotificationCenter::error error);

/*
 *  received and decode message
 */
private slots:
    void handleNodeOnRecvMsg(const QByteArray &msg);
    void handleThreadOnFinished();
    void handleExceptionOnLogin(int);  
private:
    // node server msg
    void parseSyncTime(QByteArray);
    void parseLogin(QByteArray);
    void parseRoomInfo(QByteArray);
    void parseBidInfo(QByteArray);
    void parseBidStop(QByteArray);
    void parseBidConfirm(QByteArray);
    void parseBidSettlement(QByteArray);
    void parsePKStart(QByteArray);
    void parseSingPrepare(QByteArray);
    void parseSingChange(QByteArray);
    void parseSingTimeOffset(QByteArray);
    void parsePKResult(QByteArray);
    void parseScore(QByteArray);
    void parsePropConfirm(QByteArray);
    void parsePropActive(QByteArray);
    void parseUpdateHeat(QByteArray);
    void parseSelectReward(QByteArray);
    void parseTick(QByteArray);
    void parseErrorEvent(QByteArray);
    void parseSingTick(QByteArray);
	void parseSingReady(QByteArray);
    void parseUploadLogs(QByteArray);
    void parseBidEnd(QByteArray);

    // status server msg
    void parseSyncTime4StatusServer(QByteArray);
    void parseLogin4StatusServer(QByteArray);
    void parseTick4StatusServer(QByteArray);
    void parsePKInvite4StatusServer(QByteArray);

signals:
    void onLogin(int retcode);
    void onBidEnd();
    void onReconnNotify(int cnt);

    void onLoginStatusServer(int);
    void onLoginNodeServer(int);
    void onLoginStatusResult(int);
    void onLoginNodeResult(int);
    void onRoomInfo(int status, int bidnum, Prop leastProp,
                    Prop expIncome, int completed, int heat);
    void onPKInvite(Player, Song, Prop, QList<VictoryReward>, int);
    //void onAcceptInviteResult(bool);
    void onBidStart();
    void onBidInfo(int biderNumber, Prop leastProp, Prop expectIncome);
    void onBidStop(int);
    void onBidConfirm(Prop);
    void onBidSettlement(int);
    void onSingReady(QList<SingReadyResult>);
    void onPKStart(QList<Player>);
    void onSingPrepare(int);
    void onSingChange(int, QList<int>);
    void onSingTick();
    void onPKResult(QList<Score>, int, Prop, QList<int>);
    void onSelectReward(VictoryReward);
    void onUpdateHeat(int);
    void onSingTimeOffset(float);
    void onScore(Score, Score);
    void onPropConfirm(Prop);
    void onPropActive(Player, Prop);
    void onErrorEvent(int);
    void onUploadLog();
    void onUploadDumpFile();
    void onConnectServerResult(NodeNotificationCenter::eServerType, NodeNotificationCenter::eConnectErrorCode);
    void onSwitchServer(NodeNotificationCenter::eServerType, NodeNotificationCenter::eServerType);

/*
 *  send message
 */
protected:
    void timerEvent(QTimerEvent *);

public:
    int sendMsgCount();
public:
    void sendPKInvite(Song, Prop, QList<VictoryReward>);
    void sendBid(Prop);
    void sendBidStop();
    void sendBidConfirm(Prop prop, int retcode, QString orderNumber, QString signature);
    void sendSingReady(int reCode);
    void sendSingPrepare(int sectionIndex, int retcode);
    void sendSingTimeOffset(float timeOffset);
    void sendSingEnd(int errorCode);
    void sendScore(Score sentenceScore, Score sumScore);
    void sendProp(Prop);
    void sendPropConfirm(Prop prop, int retcode, QString orderNumber, QString signature);
    void sendSelectReward(VictoryReward);

    void sendTestMsg(const QString &msgName, const QByteArray &msgData);
    void sendOnLoginStatusServer(int nonce);
    void sendOnLoginNodeServer(int nonce);
    bool sendReconnServer(eServerType);
private:
    //reconnect server when socket disconnect

    void setRoomCurState(eRoomState state) { roomState = state; }
    eRoomState getRoomCurState() const { return roomState; }
    volatile eRoomState roomState;
public:
    // switch to the specified server type
    bool switchServer(eServerType startServer, eServerType stopServer);
    void sendSwitchServer(eServerType startServer, eServerType stopServer);

private slots:
    void handleNodeConnectServerResult(int connectResult);
    void handleSwitchServer(NodeNotificationCenter::eServerType, NodeNotificationCenter::eServerType);
};

#endif // NODENOTIFICATIONCENTER_H
