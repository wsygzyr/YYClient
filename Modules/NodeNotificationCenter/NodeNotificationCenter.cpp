#include "NodeNotificationCenter.h"
#include "Modules/JsonPrase/cJSON.h"
#include <QCryptographicHash>

QAtomicPointer<NodeNotificationCenter> NodeNotificationCenter::instance;
QMutex NodeNotificationCenter::instanceMutex;

NodeNotificationCenter::NodeNotificationCenter()
{
    qRegisterMetaType<Player>("Player");
    qRegisterMetaType<Song>("Song");
    qRegisterMetaType<Score>("Score");
    qRegisterMetaType<Prop>("Prop");
    qRegisterMetaType<VictoryReward>("VictoryReward");
    qRegisterMetaType<QList<Player>>("QList<Player>");
    qRegisterMetaType<QList<Prop>>("QList<Prop>");
    qRegisterMetaType<QList<Score>>("QList<Score>");
    qRegisterMetaType<QList<VictoryReward>>("QList<VictoryReward>");
    qRegisterMetaType<NodeNotificationCenter::error>("NodeNotificationCenter::error");
    qRegisterMetaType<NodeNotificationCenter::eConnectErrorCode>("NodeNotificationCenter::eConnectErrorCode");
    qRegisterMetaType<NodeNotificationCenter::eServerType>("NodeNotificationCenter::eServerType");
    qRegisterMetaType<NodeNotificationCenter::eRoomState>("NodeNotificationCenter::eRoomState");
    qRegisterMetaType<QList<int>>("QList<int>");
    qRegisterMetaType<QList<SingReadyResult>>("QList<SingReadyResult>");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<ExceptionWidget::eButtonType>("ExceptionWidget::eButtonType");

    cfg = ConfigHelper::getInstance();
    info = PkInfoManager::getInstance();
    //stateCtrl = StateController::getInstance();

    sendMsgList = new QLinkedList<QByteArray>;

    thread = new QThread();

    this->moveToThread(thread);
    QObject::connect(thread, SIGNAL(started()), this, SLOT(handleThreadOnStarted()));
    QObject::connect(this, SIGNAL(onLogin(int)), this, SLOT(handleExceptionOnLogin(int)));
    QObject::connect(this, SIGNAL(onSwitchServer(NodeNotificationCenter::eServerType, NodeNotificationCenter::eServerType)), this, SLOT(handleSwitchServer(NodeNotificationCenter::eServerType, NodeNotificationCenter::eServerType)));

    nodeServer = NULL;
    statusServer = NULL;

    timerId = -1;

}

NodeNotificationCenter::~NodeNotificationCenter()
{
    thread->deleteLater();

    delete sendMsgList;
}

NodeNotificationCenter* NodeNotificationCenter::getInstance()
{
    //    NodeNotificationCenterDebug("getInstance!");
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            NodeNotificationCenterDebug("create instance!");
            instance = new NodeNotificationCenter();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void NodeNotificationCenter::destory()
{
    NodeNotificationCenterDebug("destroy!");
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}


bool NodeNotificationCenter::sendReconnServer(eServerType serType)
{
    NodeNotificationCenterDebug("Reconnecting server start ... !!!");
    bool ret = false;

    sendMsgList->clear();
    info->setIsSyncToServer(false);

    //reconnect operator
    if(statusServerType == serType)
    {
        this->switchServer(statusServerType, nodeServerType);
    }
    else if(nodeServerType == serType)
    {
        this->switchServer(nodeServerType, statusServerType);
    }

    return ret;
}
bool NodeNotificationCenter::start(const QString nodeServerIP, int nodeServerPort,
                                   const QString statusServerIP, int statusServerPort,
                                   int channelID, Player me)
{
    NodeNotificationCenterDebug("start!");

    if (!thread || thread->isRunning())
    {
        NodeNotificationCenterDebug("return cause thread is already running!");
        return false;
    }

    this->nodeServerIP      = nodeServerIP;
    this->nodeServerPort    = nodeServerPort;
    this->statusServerIP    = statusServerIP;
    this->statusServerPort  = statusServerPort;
    this->channelID         = channelID;
    this->me                = me;


    sendMsgList->clear();

    this->nodeState = NodeConnecting;
    thread->start(QThread::HighestPriority);

    while (this->nodeState == NodeConnecting)
    {
         QCoreApplication::processEvents();
    }

    if (this->nodeState != NodeConnected)
    {
        NodeNotificationCenterDebug("start failed!");
        return false;
    }

    NodeNotificationCenterDebug("start succeed!");
    return true;
}

bool NodeNotificationCenter::stop()
{
    NodeNotificationCenterDebug("stop!");

    if (!thread->isRunning())
    {
        NodeNotificationCenterDebug("thread is not runnint!");
        return false;
    }

    NodeNotificationCenterDebug("quiting thread ... ");

    thread->quit();
    thread->wait(100);

    info->setIsSyncToServer(false);
    return true;
}

int NodeNotificationCenter::sendMsgCount()
{
    return sendMsgList->count();
}

void NodeNotificationCenter::handleExceptionOnLogin(int code)
{
    switch(code)
    {
    case -6:  //game over,login failed, connect status server,only
        this->switchServer(statusServerType, nodeServerType);
        break;
    default:
        break;
    }
}

void NodeNotificationCenter::handleThreadOnStarted()
{
    thread->setObjectName("NodeNotificationCenterThread!-"+ QString::number((int)QThread::currentThreadId()));
    startTimer(10);

    if (this->me.getIsHost())
    {
        NodeNotificationCenterDebug("connect to NodeServer!");
        this->switchServer(nodeServerType, statusServerType);
    }
    else
    {
        NodeNotificationCenterDebug("connect to StatusServer !");
        this->switchServer(statusServerType, nodeServerType);
    }
}


void NodeNotificationCenter::handleNodeOnDisconnected()
{
    NodeNotificationCenterDebug("onError! node disconnected");

    //set state machine
    Node *sender = (Node *)this->sender();
    if (statusServer == sender)
    {
         emit onConnectServerResult(statusServerType, connectFailed);
    }
    else
    {
         emit onConnectServerResult(nodeServerType, connectFailed);
    }
}

void NodeNotificationCenter::handleNodeOnRecvMsg(const QByteArray &msg)
{ 
    NodeNotificationCenterDebug("onRecvMsg!");

    Node *sender = (Node *)this->sender();

    if (sender == nodeServer)
    {
        NodeNotificationCenterDebug("sender is NodeServer!");

        cJSON *jsRoot = cJSON_Parse(msg.data());
        for (int i = 0; i < cJSON_GetArraySize(jsRoot); i++)
        {
            cJSON *jsMsg = cJSON_GetArrayItem(jsRoot, i);
            char *strMsg = cJSON_PrintUnformatted(jsMsg);
            QByteArray msg(strMsg, strlen(strMsg));
            free(strMsg);

            cJSON *jsMsgName = cJSON_GetObjectItem(jsMsg, "S2C");
            if (!jsMsgName)
            {
                NodeNotificationCenterDebug("parse message failed!");
                emit onError(recvMessageFailed);
                return;
            }

            QString msgName(jsMsgName->valuestring);
            NodeNotificationCenterDebug("msgName is: %s", msgName.toUtf8().data());
            if ("SyncTime" == msgName)
            {
                NodeNotificationCenterDebug("find SyncTime!");
                parseSyncTime(msg);
            }
            else if ("Login" == msgName)
            {
                NodeNotificationCenterDebug("find Login!");
                parseLogin(msg);
            }
            else if("RoomInfo" == msgName)
            {
                NodeNotificationCenterDebug("find RoomInfo!");
                parseRoomInfo(msg);
            }
            else if ("BidInfo" == msgName)
            {
                NodeNotificationCenterDebug("find BidInfo!");
                parseBidInfo(msg);
            }
            else if ("BidStop" == msgName)
            {
                NodeNotificationCenterDebug("find BidStop!");
                parseBidStop(msg);
            }
            else if ("BidConfirm" == msgName)
            {
                NodeNotificationCenterDebug("find BidConfirm!");
                parseBidConfirm(msg);
            }
            else if ("BidSettlement" == msgName)
            {
                NodeNotificationCenterDebug("find BidSettlement!");
                parseBidSettlement(msg);
            }
            else if ("SingReady" == msgName)
            {
                NodeNotificationCenterDebug("find SingReady!");
                parseSingReady(msg);
            }
            else if ("PKStart" == msgName)
            {
                NodeNotificationCenterDebug("find PKStart!");
                parsePKStart(msg);
            }
            else if ("SingPrepare" == msgName)
            {
                NodeNotificationCenterDebug("find SingPrepare");
                parseSingPrepare(msg);
            }
            else if ("SingChange" == msgName)
            {
                NodeNotificationCenterDebug("find SingChange!");
                parseSingChange(msg);
            }
            else if ("SingTimeOffset" == msgName)
            {
                NodeNotificationCenterDebug("find SingTimeOffset!");
                parseSingTimeOffset(msg);
            }
            else if ("PKResult" == msgName)
            {
                NodeNotificationCenterDebug("find PKResult!");
                parsePKResult(msg);
            }
            else if ("Score" == msgName)
            {
                NodeNotificationCenterDebug("find Score!");
                parseScore(msg);
            }
            else if ("PropConfirm" == msgName)
            {
                NodeNotificationCenterDebug("find PropConfirm!");
                parsePropConfirm(msg);
            }
            else if ("PropActive" == msgName)
            {
                NodeNotificationCenterDebug("find PropActive!");
                parsePropActive(msg);
            }
            else if ("UpdateHeat" == msgName)
            {
                NodeNotificationCenterDebug("find UpdateHeat!");
                parseUpdateHeat(msg);
            }
            else if ("SelectReward" == msgName)
            {
                NodeNotificationCenterDebug("find SelectReward!");
                parseSelectReward(msg);
            }
            else if ("Tick" == msgName)
            {
                NodeNotificationCenterDebug("find Tick!");
                parseTick(msg);
            }
            else if ("ErrorEvent" == msgName)
            {
                NodeNotificationCenterDebug("find ErrorEvent!");
                parseErrorEvent(msg);
            }
            else if ("UploadLogs" == msgName)
            {
                NodeNotificationCenterDebug("find UploadLogs");
                parseUploadLogs(msg);

            }
            else if ("SingTick" == msgName)
            {
                parseSingTick(msg);
                emit onSingTick();

            }
            else if("BidEnd" == msgName)
            {
                parseBidEnd(msg);
            }
            else
            {
                NodeNotificationCenterDebug("find test string, msg is: %s", msg.data());
            }
        }
        cJSON_Delete(jsRoot);
        return;
    }

    if (sender == statusServer)
    {
        NodeNotificationCenterDebug("sender is StatusServer!");

        cJSON *jsRoot = cJSON_Parse(msg.data());
        for (int i = 0; i < cJSON_GetArraySize(jsRoot); i++)
        {
            cJSON *jsMsg = cJSON_GetArrayItem(jsRoot, i);
            char *strMsg = cJSON_PrintUnformatted(jsMsg);
            QByteArray msg(strMsg, strlen(strMsg));
            free(strMsg);

            cJSON *jsMsgName = cJSON_GetObjectItem(jsMsg, "S2C");
            if (!jsMsgName)
            {
                NodeNotificationCenterDebug("parse message failed!");
                emit onError(recvMessageFailed);
                return;
            }

            QString msgName(jsMsgName->valuestring);
            NodeNotificationCenterDebug("msgName is: %s", msgName.toUtf8().data());
            if ("SyncTime" == msgName)
            {
                NodeNotificationCenterDebug("find SyncTime!");
                parseSyncTime4StatusServer(msg);
            }
            else if ("Login" == msgName)
            {
                NodeNotificationCenterDebug("find Login!");
                parseLogin4StatusServer(msg);
            }
            else if ("Tick" == msgName)
            {
                NodeNotificationCenterDebug("find Tick!");
                parseTick4StatusServer(msg);
            }
            else if ("PKInvite" == msgName)
            {
                NodeNotificationCenterDebug("find PKInvite!");
                parsePKInvite4StatusServer(msg);
            }
            else
            {
                NodeNotificationCenterDebug("find test string, msg is: %s", msg.data());
            }
        }
        cJSON_Delete(jsRoot);
        return;
    }



}

void NodeNotificationCenter::handleThreadOnFinished()
{
    NodeNotificationCenterDebug("NodeNotificationCenter handleThreadOnFinished");
    if(nodeServer)
    {
        nodeServer->disconnectFromServer();
        nodeServer->deleteLater();
        nodeServer = NULL;
    }
    if(statusServer)
    {
        statusServer->disconnectFromServer();
        statusServer->deleteLater();
        statusServer = NULL;
    }
    if(timerId != -1)
        this->killTimer(timerId);
}

#include "Modules/ihouAuth/des/DES.h"
void NodeNotificationCenter::parseSyncTime(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseSyncTime");
    qint64 syncTime;
    qint32 nonce;
    /*
    {
        "S2C": "SyncTime",
        "time": 123123123 	//	1970 ->
    }
    */

    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        syncTime = (qint64)(cJSON_GetObjectItem(jsMsg, "time")->valuedouble);
        NodeNotificationCenterDebug("syncTime is: %lld", syncTime);
        qint64 localTime = QDateTime::currentMSecsSinceEpoch();
        NodeNotificationCenterDebug("localTime is: %lld", localTime);
        info->setMillisecondsDiff(syncTime - localTime);
        nonce = (qint32)(cJSON_GetObjectItem(jsMsg, "nonce")->valueint);
        NodeNotificationCenterDebug("nonce is: %d", nonce);
        info->setIsSyncToServer(true);
        NodeNotificationCenterDebug("syncTime");
        //stateCtrl->sendUploadDumpFile();
        emit onUploadDumpFile();
    }
    cJSON_Delete(jsMsg);

    //send event to state machine
    //stateCtrl->sendNodeSyncTimeEvent(nonce);
    emit onLoginNodeServer((nonce));
}

void NodeNotificationCenter::parseLogin(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseLogin");
    int retcode;
    /*
    {
        "S2C": "Login",
        "retcode": 0	//	0 - //auth succeed
                        //	-1 - //auth failed

    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        retcode = cJSON_GetObjectItem(jsMsg, "retcode")->valueint;
        NodeNotificationCenterDebug("nodeServer login retcode is: %d", retcode);
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendloginNodeResultEvent(retcode);
    emit onLoginNodeResult(retcode);
}

void NodeNotificationCenter::parsePKInvite4StatusServer(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parsePKInvite");
    Player host;
    Song song;
    Prop bidProp;
    QList<VictoryReward> vrList;
    int acceptedPlayer = 0;
    quint32 curGameId = 0;
    /*
    {
        "S2C": "PKInvite",
        "host":
        {
            "ID": 13123121,
            "name": "主播昵称",
        },
        "song":
        {
            "name": "爱我别走",
            "singerName": "张震岳",
            "sectionCount": 4
        },
        "bidProp":
        {
            "ID": 7000,
            "useNumber": 300
        },
        "rewardList":
        [
            {
                "ID": 0,
                "name":	"吻",
                "picUrl": "http://wen.png",
                "desp": "描述"
            },
            {
                "ID": 1,
                "name":	"黄马",
                "picUrl": "http://wen.png",
                "desp": "描述"
            },
            {
                "ID": 2,
                "name":	"吻",
                "picUrl": "http://wen.png",
                "desp": "描述"
            },
            {
                "ID": 100,
                "name": "自定义",
                "picUrl": "http://wen.png",
                "desp": "描述"
            }
        ],
        "acceptedPlayer": 1000,
        "game":{"ID":100517}
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        cJSON *jsHost = cJSON_GetObjectItem(jsMsg, "host");
        {
            host.setID(cJSON_GetObjectItem(jsHost, "ID")->valueint);
            host.setName(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "name")->valuestring));
            NodeNotificationCenterDebug("host.getName() is: %s", host.getName().toUtf8().data());
        }

        cJSON *jsSong = cJSON_GetObjectItem(jsMsg, "song");
        {
            song.setName(QString::fromUtf8(cJSON_GetObjectItem(jsSong, "name")->valuestring));
            song.setSinger(QString::fromUtf8(cJSON_GetObjectItem(jsSong, "singerName")->valuestring));
            song.setSectionCount(cJSON_GetObjectItem(jsSong, "sectionCount")->valueint);
            NodeNotificationCenterDebug("song.getName() is: %s", song.getName().toUtf8().data());
            NodeNotificationCenterDebug("song.getSinger() is: %s", song.getSinger().toUtf8().data());
        }

        cJSON *jsBidProp = cJSON_GetObjectItem(jsMsg, "bidProp");
        {
            bidProp.setID(cJSON_GetObjectItem(jsBidProp, "ID")->valueint);
            bidProp.setUseNumber(cJSON_GetObjectItem(jsBidProp, "useNumber")->valueint);

            NodeNotificationCenterDebug("bidProp.getID() is: %d", bidProp.getID());
            NodeNotificationCenterDebug("bidProp.getUseNumber() is: %d", bidProp.getUseNumber());
        }

        cJSON *jsVRList = cJSON_GetObjectItem(jsMsg, "rewardList");
        for (int i = 0; i < cJSON_GetArraySize(jsVRList); i++)
        {
            cJSON *jsVR = cJSON_GetArrayItem(jsVRList, i);
            {
                VictoryReward vr;
                vr.setID(cJSON_GetObjectItem(jsVR, "ID")->valueint);
                NodeNotificationCenterDebug("victoryReward%i.getID() is: %d", i, vr.getID());
                if (vr.getID() >= 100)
                {
                    vr.setName(QString::fromUtf8(cJSON_GetObjectItem(jsVR, "name")->valuestring));
                    //                    vr.setPicUrl(QUrl(QString::fromUtf8(cJSON_GetObjectItem(jsVR, "picUrl")->valuestring)));
                    vr.setDesp(QString::fromUtf8(cJSON_GetObjectItem(jsVR, "desp")->valuestring));

                    NodeNotificationCenterDebug("victoryReward%d.getName() is: %s", i, vr.getName().toUtf8().data());
                    //                    NodeNotificationCenterDebug("victoryReward%d.getPicUrl() is: %d", i, vr.getPicUrl().toString().toUtf8().data());
                    NodeNotificationCenterDebug("victoryReward%d.getDesp() is: %s", i, vr.getDesp().toUtf8().data());
                }
                vrList.append(vr);
            }
        }

        acceptedPlayer = cJSON_GetObjectItem(jsMsg, "acceptedPlayer")->valueint;
        cJSON *jsGame = cJSON_GetObjectItem(jsMsg, "game");
        {
            curGameId = cJSON_GetObjectItem(jsGame, "ID")->valueint;
            NodeNotificationCenterDebug("last game id:%u, current game id is is: %u", info->getGameId(), curGameId);
        }
        NodeNotificationCenterDebug("acceptedPlayer is: %d", acceptedPlayer);
    }
    cJSON_Delete(jsMsg);

    NodeNotificationCenterDebug("parse PKInvite succeed!");
	if(curGameId != info->getGameId())
    {
    	//send to state machine check
    	//stateCtrl->sendPkInviteEvent(host, song, bidProp, vrList, acceptedPlayer);
		emit onPKInvite(host, song, bidProp, vrList, acceptedPlayer);
        info->setGameId(curGameId);
    }
}

void NodeNotificationCenter::parseBidInfo(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseBidInfo");
    int biderNumber;
    Prop leastProp;
    Prop expectIncome;
    /*
    {
        "S2C": "BidInfo",
        "biderNumber": 32,
        "leastProp":
        {
            "ID": 7000,
            "useNumber": 500
        },
        "expectIncome":
        {
            "ID": 7000,
            "useNumber": 1200
        }
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        biderNumber = cJSON_GetObjectItem(jsMsg, "biderNumber")->valueint;
        NodeNotificationCenterDebug("biderNumber is: %d", biderNumber);

        cJSON *jsLeastProp = cJSON_GetObjectItem(jsMsg, "leastProp");
        {
            leastProp.setID(cJSON_GetObjectItem(jsLeastProp, "ID")->valueint);
            leastProp.setUseNumber(cJSON_GetObjectItem(jsLeastProp, "useNumber")->valueint);

            NodeNotificationCenterDebug("leastProp.getID() is: %d", leastProp.getID());
            NodeNotificationCenterDebug("leastProp.getUseNumber() is: %d", leastProp.getUseNumber());
        }

        cJSON *jsExpectIncome = cJSON_GetObjectItem(jsMsg, "expectIncome");
        {
            expectIncome.setID(cJSON_GetObjectItem(jsExpectIncome, "ID")->valueint);
            expectIncome.setUseNumber(cJSON_GetObjectItem(jsExpectIncome, "useNumber")->valueint);

            NodeNotificationCenterDebug("expectIncome.getID() is: %d", expectIncome.getID());
            NodeNotificationCenterDebug("expectIncome.getUseNumber() is: %d", expectIncome.getUseNumber());
        }
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendBidInfo(biderNumber, leastProp, expectIncome);
    emit onBidInfo(biderNumber, leastProp, expectIncome);

}

void NodeNotificationCenter::parseBidStop(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseBidStop");
    int countdown;

    /*
    {
        "S2C": "BidStop",
        "countDown": 5
    }
    */

    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        countdown = cJSON_GetObjectItem(jsMsg, "countDown")->valueint;
    }
    cJSON_Delete(jsMsg);
	
    //stateCtrl->sendBidStopEvent(countdown);
    emit onBidStop(countdown);
}

void NodeNotificationCenter::parseBidConfirm(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseBidConfirm");
    Prop prop;
    /*
    {
        "S2C": "BidConfirm",
        "prop":
        {
            "ID": 7000,
            "useNumber": 12000
        }
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        cJSON *jsProp = cJSON_GetObjectItem(jsMsg, "prop");
        {
            prop.setID(cJSON_GetObjectItem(jsProp, "ID")->valueint);
            prop.setUseNumber(cJSON_GetObjectItem(jsProp, "useNumber")->valueint);

            NodeNotificationCenterDebug("prop.getID() is: %d", prop.getID());
            NodeNotificationCenterDebug("prop.getUseNumber() is: %d", prop.getUseNumber());
        }
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendBidConfirm(prop);
    emit onBidConfirm(prop);
}

void NodeNotificationCenter::parseSingReady(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseSingReady");

    QList<SingReadyResult> resultList;

    /*
    {
        "S2C": "SingReady",
        "resultList":
        [
         { "sectionIndex": 0
                "retcode"：0   //-> 1: not completed
         }                          0: succceed
        ]                           -1: failed
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        cJSON *jsResultList = cJSON_GetObjectItem(jsMsg, "resultList");
        for (int i = 0; i < cJSON_GetArraySize(jsResultList); i++)
        {
            SingReadyResult retInfo;
            cJSON *jsResult = cJSON_GetArrayItem(jsResultList, i);
            {
                retInfo.sectionIndex = cJSON_GetObjectItem(jsResult, "sectionIndex")->valueint;
                retInfo.retcode  = cJSON_GetObjectItem(jsResult, "retcode")->valueint;
            }
            resultList.append(retInfo);
        }
    }
    cJSON_Delete(jsMsg);

    emit onSingReady(resultList);
}

void NodeNotificationCenter::parsePKStart(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parsePKStart");
    QList<Player> playerList;
    /*
    {
        "S2C": "StartPK",
        "playerList":
        [
            {
                "ID": 1231312,
                "name": "user1",
                "isHost": true,
                "bid":
                {
                    "ID": 7000,
                    "useNumber": 3000
                },
                "sumScore":		// 之前累积的得分
                {
                    "sentenceIndex": 9,	// 比如第一段是0~9句，此处为9表明该玩家已经唱完
                    "pitchScore": 80.5,
                    "lyricScore": 90.5,
                    "rhythmScore": 60.5,
                    "totalScore": 70.0,
                }
            },
            {
                "ID": 12311,
                "name": "user2",
                "isHost": false,
                "bid":
                {
                    "ID": 7000,
                    "useNumber": 3000
                },
                "sumScore":
                {
                    "sentenceIndex": 11,	// 11 表示演唱到第11句
                    "pitchScore": 0,
                    "lyricScore": 0,
                    "rhythmScore": 0,
                    "totalScore": 0,
                }
            },
            {
                "ID": 131321,
                "name": "user3",
                "isHost": false,
                "bid":
                {
                    "ID": 7000,
                    "useNumber": 3000
                },
                "sumScore":
                {
                    "sentenceIndex": -1,	// -1 表示演唱未开始
                    "pitchScore": 0,
                    "lyricScore": 0,
                    "rhythmScore": 0,
                    "totalScore": 0,
                }
            }
        ]
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {      
        cJSON *jsPlayerList = cJSON_GetObjectItem(jsMsg, "playerList");
        {
            for (int i = 0; i < cJSON_GetArraySize(jsPlayerList); i++)
            {
                Player player;
                cJSON *jsPlayer = cJSON_GetArrayItem(jsPlayerList, i);
                {
                    player.setID(cJSON_GetObjectItem(jsPlayer, "ID")->valueint);
                    player.setName(QString::fromUtf8(cJSON_GetObjectItem(jsPlayer, "name")->valuestring));
                    player.setIsHost(cJSON_GetObjectItem(jsPlayer, "isHost")->type == cJSON_True);
                    NodeNotificationCenterDebug("player%d.getID() is: %d", i, player.getID());
                    NodeNotificationCenterDebug("player%d.getName() is: %s", i, player.getName().toUtf8().data());
                    NodeNotificationCenterDebug("player%d.getIsHost() is: %s", i, player.getIsHost() ? "true" : "false");

                    Prop prop;
                    cJSON *jsProp = cJSON_GetObjectItem(jsPlayer, "bid");
                    {
                        prop.setID(cJSON_GetObjectItem(jsProp, "ID")->valueint);
                        prop.setUseNumber(cJSON_GetObjectItem(jsProp, "useNumber")->valueint);
                        NodeNotificationCenterDebug("prop%d.getID() is: %d", i, prop.getID());
                        NodeNotificationCenterDebug("prop%d.getUseNumber() is: %d", i, prop.getUseNumber());
                    }
                    player.setProp(prop);

                    Score sumScore;
                    cJSON *jsSumScore = cJSON_GetObjectItem(jsPlayer, "sumScore");
                    {
                        sumScore.setSentenceIndex(cJSON_GetObjectItem(jsSumScore, "sentenceIndex")->valueint);
                        sumScore.setPitchScore(cJSON_GetObjectItem(jsSumScore, "pitchScore")->valuedouble);
                        sumScore.setLyricScore(cJSON_GetObjectItem(jsSumScore, "lyricScore")->valuedouble);
                        sumScore.setRhythmScore(cJSON_GetObjectItem(jsSumScore, "rhythmScore")->valuedouble);
                        sumScore.setTotalScore(cJSON_GetObjectItem(jsSumScore, "totalScore")->valuedouble);
                    }
                    player.setSumScore(sumScore);
                    NodeNotificationCenterDebug("score%d.getTotalScore() is: %f", i, sumScore.getTotalScore());
                }
                playerList.append(player);
            }
        }
    }
    cJSON_Delete(jsMsg);

    //send to state machine
    emit onPKStart(playerList);
}

void NodeNotificationCenter::parseSingPrepare(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseSingPrepare");
    NodeNotificationCenterDebug("Ihou recvive <SingPrepare> notify from nodeNC succeed!");
    int sectionIndex;

    /*
    {
        "S2C": "SingPrepare",
        "sectionIndex": 0	// 如一首歌曲共4段，sectionCount = 4，则 0 <= sectionIndex < 4
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        sectionIndex = cJSON_GetObjectItem(jsMsg, "sectionIndex")->valueint;
        NodeNotificationCenterDebug("sectionIndex is: %d", sectionIndex);
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendSingPrepared(sectionIndex);
    emit onSingPrepare(sectionIndex);
}

void NodeNotificationCenter::parseSingChange(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseSingChange");
    int sectionIndex;
    int retcode;
    QList<int> reCodeList;
    /*
    {
        "S2C": "SingChange",
        "sectionIndex": 0,	// 如一首歌曲共4段，sectionCount = 4，则 0 <= sectionIndex < 4
        "retcode ": [0]  //-> array, indicate that the Pk result code front.
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        sectionIndex = cJSON_GetObjectItem(jsMsg, "sectionIndex")->valueint;
        NodeNotificationCenterDebug("sectionIndex is: %d", sectionIndex);
        cJSON *jsreCodeList = cJSON_GetObjectItem(jsMsg, "retcode");
        {
            for (int i = 0; i < cJSON_GetArraySize(jsreCodeList); i++)
            {
                retcode = cJSON_GetArrayItem(jsreCodeList, i)->valueint;
                reCodeList.append(retcode);
                NodeNotificationCenterDebug("retcode is: %d,", retcode);
            }
        }
    }
    cJSON_Delete(jsMsg);
    info->setCurrentPkPlayerIndex(sectionIndex);
    info->setLastSingEndRetcode(reCodeList);

    //send to state machine
    emit onSingChange(sectionIndex, reCodeList);
}

void NodeNotificationCenter::parseSingTimeOffset(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseSingTimeOffset");
    float timeOffset;
    /*
    {
        "S2C": "SingTimeOffset",
        "timeOffset": 12345.678
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        timeOffset = cJSON_GetObjectItem(jsMsg, "timeOffset")->valuedouble;
        NodeNotificationCenterDebug("timeOffset is: %f", timeOffset);
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendSingTimeOffset(timeOffset);
    emit onSingTimeOffset(timeOffset);
}

void NodeNotificationCenter::parsePKResult(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parsePKResult");
    QList<Score> sumScoreList;
    int heat;
    Prop propIncome;
    QList<int> lastRetcodeList;
    /*
    {
        "S2C": "PKResult",
        "lastRetcodes":[0],   //-> array, indicate that the Pk result code front.
        "sumScoreList":
        [
            {
                "sentenceIndex": -1,
                "pitchScore": 800,
                "lyricScore": 900,
                "rhythmScore": 600,
                "totalScore": 700
            },
            {
                "sentenceIndex": -1,
                "pitchScore": 800.5,
                "lyricScore": 900.5,
                "rhythmScore": 600.5,
                "totalScore": 70.0

            },
            {
                "sentenceIndex": -1,
                "pitchScore": 800.5,
                "lyricScore": 900.5,
                "rhythmScore": 600.5,
                "totalScore": 740.0
            },
            {
                "sentenceIndex": -1,
                "pitchScore": 800.5,
                "lyricScore": 900.5,
                "rhythmScore": 600.5,
                "totalScore": 750.0
            }
        ],
        "heat": 10000,

    }

    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        cJSON *jsLastRecodeList = cJSON_GetObjectItem(jsMsg, "lastRetcodes");
        {
            int retcode;
            for (int i = 0; i < cJSON_GetArraySize(jsLastRecodeList); i++)
            {
                retcode = cJSON_GetArrayItem(jsLastRecodeList, i)->valueint;
                lastRetcodeList.append(retcode);\
                NodeNotificationCenterDebug("lastretcode is: %d,", retcode);
            }
        }

        cJSON *jsSumScoreList = cJSON_GetObjectItem(jsMsg, "sumScoreList");
        for (int i = 0; i < cJSON_GetArraySize(jsSumScoreList); i++)
        {
            cJSON *jsSumScore = cJSON_GetArrayItem(jsSumScoreList, i);
            {
                Score sumScore;
                sumScore.setSentenceIndex(cJSON_GetObjectItem(jsSumScore, "sentenceIndex")->valueint);
                sumScore.setPitchScore(cJSON_GetObjectItem(jsSumScore, "pitchScore")->valuedouble);
                sumScore.setLyricScore(cJSON_GetObjectItem(jsSumScore, "lyricScore")->valuedouble);
                sumScore.setRhythmScore(cJSON_GetObjectItem(jsSumScore, "rhythmScore")->valuedouble);
                sumScore.setTotalScore(cJSON_GetObjectItem(jsSumScore, "totalScore")->valuedouble);
                sumScoreList.append(sumScore);
                NodeNotificationCenterDebug("sumScore%d.getTotalScore() is: %f", i, sumScore.getTotalScore());
            }
        }

        heat = cJSON_GetObjectItem(jsMsg, "heat")->valueint;

    }
    cJSON_Delete(jsMsg);

    //send to state machine
    //stateCtrl->sendPkResult(sumScoreList, heat, propIncome, lastRetcodeList);
    emit onPKResult(sumScoreList, heat, propIncome, lastRetcodeList);
}

void NodeNotificationCenter::parseScore(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseScore");
    Score sentenceScore;
    Score sumScore;
    /*
    {
        "S2C": "Score",
        "sentenceScore":
         {
                    "sentenceIndex": 1,
                    "pitchScore": 80.5,
                    "lyricScore": 90.5,
                    "rhythmScore": 60.5,
                    "propScore": -10,
                    "totalScore": 70.0,
            },
        "sumScore":
             {
                    "sentenceIndex": -1,
                    "pitchScore": 80.5,
                    "lyricScore": 90.5,
                    "rhythmScore": 60.5,
                    "propScore": -50,
                    "totalScore": 70.0,
         }
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        cJSON *jsSentenceScore = cJSON_GetObjectItem(jsMsg, "sentenceScore");
        {
            sentenceScore.setSentenceIndex(cJSON_GetObjectItem(jsSentenceScore, "sentenceIndex")->valueint);
            sentenceScore.setPitchScore(cJSON_GetObjectItem(jsSentenceScore, "pitchScore")->valuedouble);
            sentenceScore.setLyricScore(cJSON_GetObjectItem(jsSentenceScore, "lyricScore")->valuedouble);
            sentenceScore.setRhythmScore(cJSON_GetObjectItem(jsSentenceScore, "rhythmScore")->valuedouble);
            sentenceScore.setPropScore(cJSON_GetObjectItem(jsSentenceScore, "propScore")->valuedouble);
            sentenceScore.setTotalScore(cJSON_GetObjectItem(jsSentenceScore, "totalScore")->valuedouble);
            NodeNotificationCenterDebug("sentenceScore.getTotalScore() is: %f", sentenceScore.getTotalScore());
        }

        cJSON *jsSumScore = cJSON_GetObjectItem(jsMsg, "sumScore");
        {
            sumScore.setSentenceIndex(cJSON_GetObjectItem(jsSumScore, "sentenceIndex")->valueint);
            sumScore.setPitchScore(cJSON_GetObjectItem(jsSumScore, "pitchScore")->valuedouble);
            sumScore.setLyricScore(cJSON_GetObjectItem(jsSumScore, "lyricScore")->valuedouble);
            sumScore.setRhythmScore(cJSON_GetObjectItem(jsSumScore, "rhythmScore")->valuedouble);
            sumScore.setPropScore(cJSON_GetObjectItem(jsSumScore, "propScore")->valuedouble);
            sumScore.setTotalScore(cJSON_GetObjectItem(jsSumScore, "totalScore")->valuedouble);
            NodeNotificationCenterDebug("sumScore.getTotalScore() is: %f", sumScore.getTotalScore());
        }

    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendScore(sentenceScore, sumScore);
    emit onScore(sentenceScore, sumScore);
}

void NodeNotificationCenter::parsePropConfirm(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parsePropConfirm");
    Prop prop;
    /*
    {
        "S2C": "PropConfirm",
        "prop":
        {
            "ID": 7001,
            "useNumber": 1
        }
    }
    */

    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        cJSON *jsProp = cJSON_GetObjectItem(jsMsg, "prop");
        {
            prop.setID(cJSON_GetObjectItem(jsProp, "ID")->valueint);
            prop.setUseNumber(cJSON_GetObjectItem(jsProp, "useNumber")->valueint);

            NodeNotificationCenterDebug("prop.getID() is: %d", prop.getID());
            NodeNotificationCenterDebug("prop.getUseNumber() is: %d", prop.getUseNumber());
        }
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendPropConfirm(prop);
    emit onPropConfirm(prop);
}

void NodeNotificationCenter::parsePropActive(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parsePropActive");
    Player player;
    Prop prop;
    /*
    {
        "S2C": "PropActive",
        "player":
        {
            "name": "Sheldon",
            "ID": 1231312,
        },
        "prop":
        {
            "ID": 7001,
            "useNumber": 1
        }
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        cJSON *jsPlayer = cJSON_GetObjectItem(jsMsg, "player");
        {
            player.setName(QString::fromUtf8(cJSON_GetObjectItem(jsPlayer, "name")->valuestring));
            player.setID(cJSON_GetObjectItem(jsPlayer, "ID")->valueint);
            NodeNotificationCenterDebug("player.getName() is: %s", player.getName().toUtf8().data());
            NodeNotificationCenterDebug("player.getID() is: %d", player.getID());
        }
        cJSON *jsProp = cJSON_GetObjectItem(jsMsg, "prop");
        {
            prop.setID(cJSON_GetObjectItem(jsProp, "ID")->valueint);
            prop.setUseNumber(cJSON_GetObjectItem(jsProp, "useNumber")->valueint);
            NodeNotificationCenterDebug("prop.getID() is: %d", prop.getID());
            NodeNotificationCenterDebug("prop.getUseNumber() is: %d", prop.getUseNumber());
        }
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendPropActive(player, prop);
    emit onPropActive(player, prop);
}

void NodeNotificationCenter::parseUpdateHeat(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseUpdateHeat");
    int heat;
    /*
    {
        "S2C": "UpdateHeat",
        "heat": 10000
    }
    */

    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        heat = cJSON_GetObjectItem(jsMsg, "heat")->valueint;
        NodeNotificationCenterDebug("heat is: %d", heat);
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendUpdateHeartBeat(heat);
    emit onUpdateHeat(heat);
}

void NodeNotificationCenter::parseSelectReward(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseSelectReward");
    VictoryReward vr;
    /*
    {
        "S2C": "SelectReward",
        "reward":
        {
            "ID": 1
        }
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        cJSON *jsVR = cJSON_GetObjectItem(jsMsg, "reward");
        {
            vr.setID(cJSON_GetObjectItem(jsVR, "ID")->valueint);
            NodeNotificationCenterDebug("victoryReward.getID() is: %d", vr.getID());
        }
    }
    cJSON_Delete(jsMsg);

   //stateCtrl->sendSelectVictoryReward(vr);
   emit onSelectReward(vr);

}

void NodeNotificationCenter::parseTick(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseTick");
    /*
    {
        "S2C": "Tick",
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "Tock");
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray sendMsg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(sendMsg);
}

void NodeNotificationCenter::parseErrorEvent(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseErrorEvent");
    int error;
    /*
    {
        "S2C": "ErrorEvent",
        "error": "-1",
    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        error = cJSON_GetObjectItem(jsMsg, "error")->valueint;
        NodeNotificationCenterDebug("error is: %d", error);
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendOnErrorEvent(error);
    emit onErrorEvent(error);
}

void NodeNotificationCenter::parseSingTick(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseSingTick");
    /*
    {
        "S2C": "SingTick",
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "SingTock");
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray sendMsg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(sendMsg);
}

void NodeNotificationCenter::parseBidEnd(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseBidEnd");
    emit onBidEnd();
}

void NodeNotificationCenter::parseBidSettlement(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseBidSettlement");
    /*
    {
        "S2C": "BidSettlement",
        completed: 1   //settlement completed user number
    }
    */
    int userNum = 0;
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        userNum = cJSON_GetObjectItem(jsMsg, "completed")->valueint;
        NodeNotificationCenterDebug("settlement completed userNum is: %d", userNum);
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendSettlement(userNum);
    emit onBidSettlement(userNum);
}

void NodeNotificationCenter::parseRoomInfo(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseRoomInfo");
    /*
    {
        "S2C": "RoomInfo",
        "status": 0,     //room status
                         //0: Spare time,host can initiate "Pkinvite"
                           1: Biding
                           2: BidSettlement
                           3: Pking
        "song": {
            "ID": "123",
            "name": "爱我别走",
            "sectionCount": 4,
            "lyricUrl": "xxx.xml",
            "accomanyUrl": "xxx.mp3",
            "sectionUrl": "xxx.section.xml"
        },
        "bid":
        {
            "biderNumber": 1,
            "leastProp":
            {
                "ID": 7000,
                "useNumber": 500
            },
            "expectIncome":
            {
                "ID": 7000,
                "useNumber": 1200
            }
        },
        "settlement":
        {
            "completed": 1,
            "singReady": 0
        },
    pk:
    {
        "heat":1,
        "playerList": [
             {
            "ID": 1231312,
            "name": "user1",
            "isHost": true,
            "bid": {
                "ID": 7000,
                "useNumber": 3000
             },
            "sumScore": {
                "sentenceIndex": 9,
                "pitchScore": 80.5,
                "lyricScore": 90.5,
                "rhythmScore": 60.5,
                "totalScore": 70.0,
            }
            },
            {
            "ID": 12311,
            "name": "user2",
            "isHost": false,
            "bid": {
                "ID": 7000,
                "useNumber": 3000
            },
            "sumScore": {
                "sentenceIndex": 11,
                "pitchScore": 0,
                "lyricScore": 0,
                "rhythmScore": 0,
                "totalScore": 0,
            }
            },
            {
            "ID": 131321,
            "name": "user3",
            "isHost": false,
            "bid": {
                 "ID": 7000,
                 "useNumber": 3000
             },
            "sumScore": {
                "sentenceIndex": -1,
                "pitchScore": 0,
                "lyricScore": 0,
                "rhythmScore": 0,
                "totalScore": 0,
             }
            }
        ],
        "currentSectionIndex":0,
        "currentTimeOffset":0,
        "singReadyList":[0,0,0,0]
        }
    }
    */
    //parse RoomInfo
    int status = -1;
    Song song;
    int biderNumber;
    Prop leastProp;
    Prop expectIncome;
    int completed = 0;
    int firstSingReady = -2;
    int heat = 0;
    int currentSectionIndex = -1;
    double timeOffset = 0;
    QString singername = info->getCurrentSong().getSinger();
    QList<SingReadyResult>singReadyList;
    QList<Player> playerList;
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        status = cJSON_GetObjectItem(jsMsg, "status")->valueint;
        setRoomCurState(static_cast<eRoomState>(status));
        if(SpareTimeState < status)
        {
            cJSON *jsSong = cJSON_GetObjectItem(jsMsg, "song");
            {
                song.setSongID(QString::fromUtf8(cJSON_GetObjectItem(jsSong, "ID")->valuestring));
                song.setName(QString::fromUtf8(cJSON_GetObjectItem(jsSong, "name")->valuestring));
                song.setSectionCount(cJSON_GetObjectItem(jsSong, "sectionCount")->valueint);
                song.setLyricUrl(QUrl(cfg->getHttpDownloadPrefix() + QString::fromUtf8(cJSON_GetObjectItem(jsSong, "lyricUrl")->valuestring)));
                song.setAccompanyUrl(QUrl(cfg->getHttpDownloadPrefix() + QString::fromUtf8(cJSON_GetObjectItem(jsSong, "accompanyUrl")->valuestring)));
                song.setSectionUrl(QUrl(cfg->getHttpDownloadPrefix() + QString::fromUtf8(cJSON_GetObjectItem(jsSong, "sectionUrl")->valuestring)));
                song.setSinger(singername);
               // NodeNotificationCenterDebug("song.getName() is: %s", song.getName().toUtf8().data());
                info->setCurrentSong(song);
            }

            if(BidState == status)
            {
                cJSON *jsBid = cJSON_GetObjectItem(jsMsg, "bid");
                {
                    biderNumber = cJSON_GetObjectItem(jsBid, "biderNumber")->valueint;
                    NodeNotificationCenterDebug("biderNumber is: %d", biderNumber);

                    cJSON *jsLeastProp = cJSON_GetObjectItem(jsBid, "leastProp");
                    {
                        leastProp.setID(cJSON_GetObjectItem(jsLeastProp, "ID")->valueint);
                        leastProp.setUseNumber(cJSON_GetObjectItem(jsLeastProp, "useNumber")->valueint);

                        NodeNotificationCenterDebug("leastProp.getID() is: %d", leastProp.getID());
                        NodeNotificationCenterDebug("leastProp.getUseNumber() is: %d", leastProp.getUseNumber());
                        info->setLeastBidProp(leastProp);
                    }

                    cJSON *jsExpectIncome = cJSON_GetObjectItem(jsBid, "expectIncome");
                    {
                        expectIncome.setID(cJSON_GetObjectItem(jsExpectIncome, "ID")->valueint);
                        expectIncome.setUseNumber(cJSON_GetObjectItem(jsExpectIncome, "useNumber")->valueint);

                        NodeNotificationCenterDebug("expectIncome.getID() is: %d", expectIncome.getID());
                        NodeNotificationCenterDebug("expectIncome.getUseNumber() is: %d", expectIncome.getUseNumber());
                    }
                }
            }

            if(SettlementState == status)
            {
                cJSON *jsSettlement = cJSON_GetObjectItem(jsMsg, "settlement");
                {
                    completed = cJSON_GetObjectItem(jsSettlement, "completed")->valueint;
                    firstSingReady = cJSON_GetObjectItem(jsSettlement, "singReady")->valueint;
                    NodeNotificationCenterDebug("completed is: %d, first sing ready is:%d", completed, firstSingReady);
                }
                SingReadyResult settleSingReady;
                settleSingReady.sectionIndex = 0;
                settleSingReady.retcode = firstSingReady;
                singReadyList.append(settleSingReady);
                info->setResDownloadRetcode(singReadyList);
            }

            if(PkState == status)
            {
                cJSON *jsPK = cJSON_GetObjectItem(jsMsg, "pk");
                {
                    heat =  cJSON_GetObjectItem(jsPK, "heat")->valueint;
                    cJSON *jsPlayerList = cJSON_GetObjectItem(jsPK, "playerList");
                    {
                        for (int i = 0; i < cJSON_GetArraySize(jsPlayerList); i++)
                        {
                            Player player;
                            cJSON *jsPlayer = cJSON_GetArrayItem(jsPlayerList, i);
                            {
                                player.setID(cJSON_GetObjectItem(jsPlayer, "ID")->valueint);
                                player.setName(QString::fromUtf8(cJSON_GetObjectItem(jsPlayer, "name")->valuestring));
                                player.setIsHost(cJSON_GetObjectItem(jsPlayer, "isHost")->type == cJSON_True);
                                NodeNotificationCenterDebug("player%d.getID() is: %d", i, player.getID());
                                NodeNotificationCenterDebug("player%d.getName() is: %s", i, player.getName().toUtf8().data());
                                NodeNotificationCenterDebug("player%d.getIsHost() is: %s", i, player.getIsHost() ? "true" : "false");
                                Score sumScore;
                                cJSON *jsSumScore = cJSON_GetObjectItem(jsPlayer, "sumScore");
                                {
                                    sumScore.setSentenceIndex(cJSON_GetObjectItem(jsSumScore, "sentenceIndex")->valueint);
                                    sumScore.setPitchScore(cJSON_GetObjectItem(jsSumScore, "pitchScore")->valuedouble);
                                    sumScore.setLyricScore(cJSON_GetObjectItem(jsSumScore, "lyricScore")->valuedouble);
                                    sumScore.setRhythmScore(cJSON_GetObjectItem(jsSumScore, "rhythmScore")->valuedouble);
                                    sumScore.setTotalScore(cJSON_GetObjectItem(jsSumScore, "totalScore")->valuedouble);
                                }
                                player.setSumScore(sumScore);
                                NodeNotificationCenterDebug("score%d.getTotalScore() is: %f", i, sumScore.getTotalScore());

                            }
                            playerList.append(player);
                        }
                    }
                    info->setPkPlayerList(playerList);

                    currentSectionIndex = cJSON_GetObjectItem(jsPK, "currentSectionIndex")->valueint;
                    timeOffset = cJSON_GetObjectItem(jsPK, "currentTimeOffset")->valuedouble;
                    info->setTimeOffset((float)timeOffset);
                    cJSON *jsSingReadyList = cJSON_GetObjectItem(jsPK, "singReadyList");
                    {
                        SingReadyResult singReady;
                        for (int j = 0; j < cJSON_GetArraySize(jsSingReadyList); j++)
                        {
                            cJSON *jsReadyRsult = cJSON_GetArrayItem(jsSingReadyList, j);
                            singReady.sectionIndex = j;
                            singReady.retcode = jsReadyRsult->valueint;
                            singReadyList.append(singReady);
                        }
                    }
                    info->setResDownloadRetcode(singReadyList);
                    info->setCurrentPkPlayerIndex(currentSectionIndex);
                }
            }
        }
    }
    cJSON_Delete(jsMsg);


    //send event by server current state
    emit onRoomInfo(status, biderNumber, leastProp, expectIncome,
                    completed, heat);
}

void NodeNotificationCenter::parseSyncTime4StatusServer(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseSyncTime4StatusServer");
    qint64 syncTime;
    qint32 nonce;
    /*
    {
        "S2C": "SyncTime",
        "time": 123123123 	//	1970 ->
        "nonce": 1       //random number
    }
    */

    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        syncTime = (qint64)(cJSON_GetObjectItem(jsMsg, "time")->valuedouble);
        NodeNotificationCenterDebug("syncTime is: %lld", syncTime);
        qint64 localTime = QDateTime::currentMSecsSinceEpoch();
        NodeNotificationCenterDebug("localTime is: %lld", localTime);
        info->setMillisecondsDiff(syncTime - localTime);
        nonce = (qint32)(cJSON_GetObjectItem(jsMsg, "nonce")->valueint);
        NodeNotificationCenterDebug("nonce is: %d", nonce);
        info->setIsSyncToServer(true);
    }
    cJSON_Delete(jsMsg);

    //stateCtrl->sendStatusSyncTimeEvent(nonce);
    emit onLoginStatusServer(nonce);
}

void NodeNotificationCenter::sendOnLoginStatusServer(int nonce)
{
     NodeNotificationCenterDebug("NodeNotificationCenter LogingStatusServer");

    /*
     *  cipher = Base64(MD5(nonce=1&password=iHouPlugin!))
     */
      QString strPassword = "password=iHouPlugin!";
      QString strNonce = "nonce=" + QString("%1").arg(nonce);
      QString plain = QString("%1&%2").arg(strNonce).arg(strPassword);
      NodeNotificationCenterDebug("plain is: %s", plain.toUtf8().data());
      // MD5/bas64 encode
      QByteArray md5array;
      QString cipher;
      md5array = QCryptographicHash::hash(plain.toAscii(),QCryptographicHash::Md5);
      cipher.append(md5array.toBase64());
      NodeNotificationCenterDebug("cipher is: %s", cipher.toUtf8().data());

    /*
    {
        "C2S": "Login",
        "channel":
        {
            "ID": 12312321,
        },
        "auth":
        {
            "cipher": "sdfsfsf"
        }
    }
    */
    cJSON *jsSendMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsSendMsg, "C2S", "Login");

        cJSON *jsChannel = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsChannel, "ID", channelID);
        }
        cJSON_AddItemToObject(jsSendMsg, "channel", jsChannel);

        cJSON *jsAuth = cJSON_CreateObject();
        {
            cJSON_AddStringToObject(jsAuth, "cipher", cipher.toUtf8().data());
        }
        cJSON_AddItemToObject(jsSendMsg, "auth", jsAuth);
    }
    char *str = cJSON_PrintUnformatted(jsSendMsg);
    QByteArray sendMsg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsSendMsg);

    statusServer->sendMsg(sendMsg);
}

void NodeNotificationCenter::sendOnLoginNodeServer(int nonce)
{
    NodeNotificationCenterDebug("NodeNotificationCenter LogingNodeServer");

    /*
     *  cipher = Base64(MD5(nonce=1&password=iHouPlugin!))
     */
    QString strPassword = "password=iHouPlugin!";
    QString strNonce = "nonce=" + QString("%1").arg(nonce);
    QString plain = QString("%1&%2").arg(strNonce).arg(strPassword);
    NodeNotificationCenterDebug("plain is: %s", plain.toUtf8().data());
    // MD5/base64 encode
    QByteArray md5array;
    QString cipher;
    md5array = QCryptographicHash::hash(plain.toAscii(),QCryptographicHash::Md5);
    cipher.append(md5array.toBase64());
    NodeNotificationCenterDebug("cipher is: %s", cipher.toUtf8().data());

    /*
    {
        "C2S": "Login",
        "channel":
        {
            "ID": 12312321,
        },
        "player":
        {
            "ID": 312312132,
            "name": "sheldon",
            "isHost": false,
        },
        "auth":
        {
            "cipher": "sdfsfsf"
        }
    }
    */
    cJSON *jsSendMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsSendMsg, "C2S", "Login");

        cJSON *jsChannel = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsChannel, "ID", channelID);
        }
        cJSON_AddItemToObject(jsSendMsg, "channel", jsChannel);

        cJSON *jsPlayer = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsPlayer, "ID", me.getID());
            cJSON_AddStringToObject(jsPlayer, "name", me.getName().toUtf8().data());
            cJSON_AddBoolToObject(jsPlayer, "isHost", me.getIsHost());
        }
        cJSON_AddItemToObject(jsSendMsg, "player", jsPlayer);

        cJSON *jsAuth = cJSON_CreateObject();
        {
            cJSON_AddStringToObject(jsAuth, "cipher", cipher.toUtf8().data());
        }
        cJSON_AddItemToObject(jsSendMsg, "auth", jsAuth);
    }
    char *str = cJSON_PrintUnformatted(jsSendMsg);
    QByteArray sendMsg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsSendMsg);

    sendMsgList->append(sendMsg);
}

void NodeNotificationCenter::parseUploadLogs(QByteArray msg)
{
    //stateCtrl->sendUploadLog();
    emit onUploadLog();
}

void NodeNotificationCenter::parseLogin4StatusServer(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseLogin4StatusServer");
    int retcode;
    /*
    {
        "S2C": "Login",
        "retcode": 0	//	0 - //auth succeed
                        //	-1 - //auth failed

    }
    */
    cJSON *jsMsg = cJSON_Parse(msg.data());
    {
        retcode = cJSON_GetObjectItem(jsMsg, "retcode")->valueint;
        NodeNotificationCenterDebug("Login status Server retcode is: %d", retcode);
    }
    cJSON_Delete(jsMsg);

    emit onLoginStatusResult(retcode);
}

void NodeNotificationCenter::parseTick4StatusServer(QByteArray msg)
{
    NodeNotificationCenterDebug("NodeNotificationCenter parseTick4StatusServer");
    /*
    {
        "S2C": "Tick",
    }*/

    //receive "Tick" heartbeat pkg, then send "Tock" to status server
    /*
    {
        "S2C": "Tock",
    }*/
    cJSON *jsSendMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsSendMsg, "C2S", "Tock");
    }
    char *str = cJSON_PrintUnformatted(jsSendMsg);
    QByteArray sendMsg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsSendMsg);

    statusServer->sendMsg(sendMsg);
    return;
}

void NodeNotificationCenter::timerEvent(QTimerEvent *event)
{
    //    NodeNotificationCenterDebug("onTimerEvent! thread name is: %s", QThread::currentThread()->objectName().toUtf8().data());

    // send message
    if (sendMsgList->count() <= 0)
    {
        return;
    }

    NodeNotificationCenterDebug("sendMsgList->count() is: %d", sendMsgList->count());

    QByteArray msg = sendMsgList->takeFirst();

    if(nodeServer)
    {
        if (!nodeServer->sendMsg(msg.data()))
        {
            NodeNotificationCenterDebug("sendMsg failed! msg is: %s", msg.data());
            nodeServer->disconnectFromServer();
            emit onError(sendMessageFailed);
        }

        NodeNotificationCenterDebug("sendMsg succeed! ");

    }
}
void NodeNotificationCenter::sendPKInvite(Song song, Prop bidProp, QList<VictoryReward> vrList)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendPKInvite");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }

    /*
    {
        "C2S": "PKInvite",
        "song":
        {
            "ID": "1234",
        },
        "bidProp":
        {
            "ID": 7000,
            "useNumber": 300
        },
        "rewardList":
        [
            {
                "ID": 0,
            },
            {
                "ID": 1,
            },
            {
                "ID": 2,
            },
            {
                "ID": 100,
                "name": "自定义",
                "desp": "描述"
            }
        ]
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "PKInvite");

        cJSON *jsSong = cJSON_CreateObject();
        {
            cJSON_AddStringToObject(jsSong, "ID", song.getSongID().toUtf8().data());
        }
        cJSON_AddItemToObject(jsMsg, "song", jsSong);

        cJSON *jsBidProp = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsBidProp, "ID", bidProp.getID());
            cJSON_AddNumberToObject(jsBidProp, "useNumber", bidProp.getUseNumber());
        }
        cJSON_AddItemToObject(jsMsg, "bidProp", jsBidProp);

        cJSON *jsRewardList = cJSON_CreateArray();
        {
            for (int i = 0; i < vrList.size(); i++)
            {
                VictoryReward vr = vrList.at(i);
                cJSON *jsVR = cJSON_CreateObject();
                {
                    cJSON_AddNumberToObject(jsVR, "ID", vr.getID());
                    if (vr.getID() >= 100)
                    {
                        cJSON_AddStringToObject(jsVR, "name", vr.getName().toUtf8().data());
                        cJSON_AddStringToObject(jsVR, "desp", vr.getDesp().toUtf8().data());
                    }
                }
                cJSON_AddItemToArray(jsRewardList, jsVR);
            }
        }
        cJSON_AddItemToObject(jsMsg, "rewardList", jsRewardList);
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}


void NodeNotificationCenter::sendBid(Prop prop)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendBid");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }

    /*
    {
        "C2S": "Bid",
        "prop":
        {
            "ID": 7000,
            "useNumber": 100
        }
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "Bid");

        cJSON *jsProp = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsProp, "ID", prop.getID());
            cJSON_AddNumberToObject(jsProp, "useNumber", prop.getUseNumber());
        }
        cJSON_AddItemToObject(jsMsg, "prop", jsProp);
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}

void NodeNotificationCenter::sendBidStop()
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendBidStop");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }

    /*
    {
        "C2S": "BidStop"
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "BidStop");
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}

void NodeNotificationCenter::sendBidConfirm(Prop prop, int retcode, QString orderNumber, QString signature)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendBidConfirm");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }

    /*
    {
        "C2S": "BidConfirm",
        "prop":
        {
            "ID": 7001,
            "useNumber": 12000
        },
        "retcode": 0,
        "orderNumber": "12313121",
        "signature": "sfssfsfs"
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "BidConfirm");

        cJSON *jsProp = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsProp, "ID", prop.getID());
            cJSON_AddNumberToObject(jsProp, "useNumber", prop.getUseNumber());
        }
        cJSON_AddItemToObject(jsMsg, "prop", jsProp);

        cJSON_AddNumberToObject(jsMsg, "retcode", retcode);
        cJSON_AddStringToObject(jsMsg, "orderNumber", orderNumber.toUtf8().data());
        cJSON_AddStringToObject(jsMsg, "signature", signature.toUtf8().data());
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}

void NodeNotificationCenter::sendSingReady(int reCode)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendSingReady");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }

    /*
    {
        "C2S": "SingReady"
        "retcode":0     //-> resource download result code.
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "SingReady");
        cJSON_AddNumberToObject(jsMsg, "retcode", reCode);
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}

void NodeNotificationCenter::sendSingPrepare(int sectionIndex, int retcode)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendSingPrepare");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }

    NodeNotificationCenterDebug("Ihou send <SingPrepare> result to nodeNC, resultCode=%d",retcode);

    /*
    {
        "C2S": "SingPrepare",
        "sectionIndex": 0,
        "retcode": 0
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "SingPrepare");
        cJSON_AddNumberToObject(jsMsg, "sectionIndex", sectionIndex);
        cJSON_AddNumberToObject(jsMsg, "retcode", retcode);
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);

}

void NodeNotificationCenter::sendSingTimeOffset(float timeOffset)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendSingTimeOffset");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }

    /*
    {
        "C2S": "SingTimeOffset",
        "timeOffset": 12345.678
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "SingTimeOffset");
        cJSON_AddNumberToObject(jsMsg, "timeOffset", timeOffset);
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}

void NodeNotificationCenter::sendSingEnd(int errorCode)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendSingEnd");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }

    /*
    {
        "C2S": "SingEnd"
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "SingEnd");
        cJSON_AddNumberToObject(jsMsg , "retcode" , errorCode);
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}

void NodeNotificationCenter::sendScore(Score sentenceScore, Score sumScore)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendScore");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }

    /*
    {
        "C2S": "Score",
        "sentenceScore":
        {
            "sentenceIndex": 1,
            "pitchScore": 80.5,
            "lyricScore": 90.5,
            "rhythmScore": 60.5,
            "propScore": -10,
            "totalScore": 70.0,
        },
        "sumScore":
        {
            "sentenceIndex": -1,
            "pitchScore": 80.5,
            "lyricScore": 90.5,
            "rhythmScore": 60.5,
            "propScore": -50,
            "totalScore": 70.0,
        }
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "Score");

        cJSON *jsSentenceScore = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsSentenceScore, "sentenceIndex", sentenceScore.getSentenceIndex());
            cJSON_AddNumberToObject(jsSentenceScore, "pitchScore", sentenceScore.getPitchScore());
            cJSON_AddNumberToObject(jsSentenceScore, "lyricScore", sentenceScore.getLyricScore());
            cJSON_AddNumberToObject(jsSentenceScore, "rhythmScore", sentenceScore.getRhythmScore());
            cJSON_AddNumberToObject(jsSentenceScore, "propScore", sentenceScore.getPropScore());
            cJSON_AddNumberToObject(jsSentenceScore, "totalScore", sentenceScore.getTotalScore());
        }
        cJSON_AddItemToObject(jsMsg, "sentenceScore", jsSentenceScore);

        cJSON *jsSumScore = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsSumScore, "sentenceIndex", sumScore.getSentenceIndex());
            cJSON_AddNumberToObject(jsSumScore, "pitchScore", sumScore.getPitchScore());
            cJSON_AddNumberToObject(jsSumScore, "lyricScore", sumScore.getLyricScore());
            cJSON_AddNumberToObject(jsSumScore, "rhythmScore", sumScore.getRhythmScore());
            cJSON_AddNumberToObject(jsSumScore, "propScore", sumScore.getPropScore());
            cJSON_AddNumberToObject(jsSumScore, "totalScore", sumScore.getTotalScore());
        }
        cJSON_AddItemToObject(jsMsg, "sumScore", jsSumScore);
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}

void NodeNotificationCenter::sendProp(Prop prop)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendProp");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }


    /*
    {
        "C2S": "Prop",
        "prop":
        {
            "ID": 7001,
            "useNumber": 1
        }
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "Prop");

        cJSON *jsProp = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsProp, "ID", prop.getID());
            cJSON_AddNumberToObject(jsProp, "useNumber", prop.getUseNumber());
        }
        cJSON_AddItemToObject(jsMsg, "prop", jsProp);
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}

void NodeNotificationCenter::sendPropConfirm(Prop prop, int retcode, QString orderNumber, QString signature)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendPropConfirm");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }


    /*
    {
        "C2S": "PropConfirm",
        "prop":
        {
            "ID": 7001,
            "useNumber": 1
        },
        "retcode": 0,
        "orderNumber": "12313121",
        "signature": "sfssfsfs"
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "PropConfirm");

        cJSON *jsProp = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsProp, "ID", prop.getID());
            cJSON_AddNumberToObject(jsProp, "useNumber", prop.getUseNumber());
        }
        cJSON_AddItemToObject(jsMsg, "prop", jsProp);

        cJSON_AddNumberToObject(jsMsg, "retcode", retcode);
        cJSON_AddStringToObject(jsMsg, "orderNumber", orderNumber.toUtf8().data());
        cJSON_AddStringToObject(jsMsg, "signature", signature.toUtf8().data());
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);

}

void NodeNotificationCenter::sendSelectReward(VictoryReward vr)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendSelectReward");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }

    /*
    {
        "C2S": "SelectReward",
        "reward":
        {
            "ID": 1
        }
    }
    */
    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", "SelectReward");

        cJSON *jsReward = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsReward, "ID", vr.getID());
        }
        cJSON_AddItemToObject(jsMsg, "reward", jsReward);
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}


void NodeNotificationCenter::sendTestMsg(const QString &msgName, const QByteArray &msgData)
{
    NodeNotificationCenterDebug("NodeNotificationCenter sendTestMsg");
    if (NULL == sendMsgList)
    {
        NodeNotificationCenterDebug("return cause sendMsgList == NULL!");
        return;
    }


    cJSON *jsMsg = cJSON_CreateObject();
    {
        cJSON_AddStringToObject(jsMsg, "C2S", msgName.toUtf8().data());
        cJSON_AddStringToObject(jsMsg, "test", msgData.data());
    }
    char *str = cJSON_PrintUnformatted(jsMsg);
    QByteArray msg = QByteArray(str, strlen(str));
    free(str);
    cJSON_Delete(jsMsg);

    sendMsgList->append(msg);
}

void NodeNotificationCenter::handleNodeConnectServerResult(int connectResult)
{
    NodeNotificationCenterDebug("NodeNotificationCenter handleNodeConnectServerResult");
    //set state machine
    Node *sender = (Node *)this->sender();
    if (statusServer == sender)
    {
        if(0 == connectResult)
        {
            emit onConnectServerResult(statusServerType, connectSucceed);
        }
        else
        {
            emit onConnectServerResult(statusServerType, connectFailed);
        }
    }
    else
    {
        if(0 == connectResult)
        {
            emit onConnectServerResult(nodeServerType, connectSucceed);
        }
        else
        {
            emit onConnectServerResult(nodeServerType, connectFailed);
        }
    }
}

bool NodeNotificationCenter::switchServer(eServerType startServer, eServerType stopServer)
{
    //stop server
    switch(stopServer)
    {
        case statusServerType:
        {
            if(NULL != statusServer)
            {
                NodeNotificationCenterDebug("disconnect to StatusServer...!");
                QObject::disconnect(thread, SIGNAL(finished()), statusServer, SLOT(disconnectFromServer()));
                QObject::disconnect(thread, SIGNAL(finished()), statusServer, SLOT(deleteLater()));
                QObject::disconnect(statusServer, SIGNAL(onRecvMsg(QByteArray)), this, SLOT(handleNodeOnRecvMsg(QByteArray)));
                QObject::disconnect(statusServer, SIGNAL(onDisconnected()), this, SLOT(handleNodeOnDisconnected()));
                QObject::disconnect(statusServer, SIGNAL(onConnectResult(int)), this, SLOT(handleNodeConnectServerResult(int)));
                statusServer->disconnectFromServer();
                statusServer->deleteLater();
                statusServer = NULL;
            }
            break;
        }
        case nodeServerType:
        {
            if(NULL != nodeServer)
            {
                NodeNotificationCenterDebug("disconnect to NodeServer...!");
                QObject::disconnect(thread, SIGNAL(finished()), nodeServer, SLOT(disconnectFromServer()));
                QObject::disconnect(thread, SIGNAL(finished()), nodeServer, SLOT(deleteLater()));
                QObject::disconnect(nodeServer, SIGNAL(onRecvMsg(QByteArray)), this, SLOT(handleNodeOnRecvMsg(QByteArray)));
                QObject::disconnect(nodeServer, SIGNAL(onDisconnected()), this, SLOT(handleNodeOnDisconnected()));
                QObject::disconnect(nodeServer, SIGNAL(onConnectResult(int)), this, SLOT(handleNodeConnectServerResult(int)));
                nodeServer->disconnectFromServer();
                nodeServer->deleteLater();
                nodeServer = NULL;
            }
            break;
        }
    default:
        break;
    }

    //start server
    switch(startServer)
    {
        case statusServerType:
        {
            if(NULL == statusServer)
            {
                statusServer = new Node;
                if(NULL == statusServer)
                    return false;
            }
            NodeNotificationCenterDebug("connect to StatusServer...!");
            statusServer->setObjectName("statusServer");
            QObject::connect(statusServer, SIGNAL(onRecvMsg(QByteArray)), this, SLOT(handleNodeOnRecvMsg(QByteArray)));
            QObject::connect(statusServer, SIGNAL(onDisconnected()), this, SLOT(handleNodeOnDisconnected()));
            QObject::connect(statusServer, SIGNAL(onConnectResult(int)), this, SLOT(handleNodeConnectServerResult(int)));

            this->nodeState = NodeConnecting;
            if (!statusServer->connectToServer(statusServerIP, statusServerPort, true, 3))
            {
                this->nodeState = NodeError;
            }
            else
            {
                this->nodeState = NodeConnected;
            }
            break;
        }
        case nodeServerType:
        {
            if(NULL == nodeServer)
            { 
                nodeServer = new Node;
                if(NULL == nodeServer)
                    return false;
            }
            NodeNotificationCenterDebug("connect to Node Server...!");
            nodeServer->setObjectName("nodeServer");
            QObject::connect(nodeServer, SIGNAL(onRecvMsg(QByteArray)), this, SLOT(handleNodeOnRecvMsg(QByteArray)));
            QObject::connect(nodeServer, SIGNAL(onDisconnected()), this, SLOT(handleNodeOnDisconnected()));
            QObject::connect(nodeServer, SIGNAL(onConnectResult(int)), this, SLOT(handleNodeConnectServerResult(int)));

            this->nodeState = NodeConnecting;
            if (!nodeServer->connectToServer(nodeServerIP, nodeServerPort, true, 3))
            {
                this->nodeState = NodeError;
            }
            else
            {
                this->nodeState = NodeConnected;
            }
            break;
        }
    default:
        break;
    }

    return true;
}


void NodeNotificationCenter::sendSwitchServer(eServerType startServer, eServerType stopServer)
{
    emit onSwitchServer(startServer, stopServer);
}


void NodeNotificationCenter::handleSwitchServer(eServerType startServer, eServerType stopServer)
{
    switchServer(startServer, stopServer);
}
