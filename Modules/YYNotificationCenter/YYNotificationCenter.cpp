#include "YYNotificationCenter.h"
#include "Modules/JsonPrase/cJSON.h"
#include <QMetaType>
#include <QApplication>
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "PkInfoManager/PkInfoManager.h"

QByteArray json2ByteArray(cJSON* root)
{
    char* str = cJSON_PrintUnformatted(root);
    QByteArray msg = QByteArray(str , strlen(str));
    free(str);
    return msg;
}

class YYNotificationCenterMsg
{
public:
    YYNotificationCenterMsg(QString name, QByteArray data)
    {
        this->name = name;
        this->data = data;
    }

    QString name;
    QByteArray data;
};
QAtomicPointer<YYNotificationCenter> YYNotificationCenter::instance;
QMutex YYNotificationCenter::instanceMutex;

YYNotificationCenter::YYNotificationCenter()
{
    sendMsgList = NULL;

    pipe = NULL;
    sendMsgList = NULL;
    timerId = -1;
    qRegisterMetaType<YYNotificationCenter::error>("YYNotificationCenter::error");
}

YYNotificationCenter::~YYNotificationCenter()
{

}

YYNotificationCenter* YYNotificationCenter::getInstance()
{
    //    YYNotificationCenterDebug("getInstance!");
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            YYNotificationCenterDebug("create instance!");
            instance = new YYNotificationCenter();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void YYNotificationCenter::destory()
{
    YYNotificationCenterDebug("YYNotificationCenter destroy!");
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}

void YYNotificationCenter::sendCheckConnection(QString uid)
{
    YYNotificationCenterDebug("YYNotificationCenter sendCheckConnection!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root , "guid" ,uid.toUtf8().data());
    sendMsgList->append(YYNotificationCenterMsg("P2Y_CheckConnection" , json2ByteArray(root)));
    cJSON_Delete(root);
}

void YYNotificationCenter::sendHostCreateGame()
{
    YYNotificationCenterDebug("YYNotificationCenter sendHostCreateGame!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    QByteArray msg;
    msg.clear();
    sendMsgList->append(YYNotificationCenterMsg("P2Y_HostCreateGame" ,msg));
}

void YYNotificationCenter::sendAudienceJoinGame()
{
    YYNotificationCenterDebug("YYNotificationCenter sendAudienceJoinGame!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    QByteArray msg;
    msg.clear();
    sendMsgList->append(YYNotificationCenterMsg("P2Y_AudienceJoinGame" ,msg));
}

void YYNotificationCenter::sendGameOver()
{
    YYNotificationCenterDebug("YYNotificationCenter sendGameOver!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    QByteArray msg;
    msg.clear();
    sendMsgList->append(YYNotificationCenterMsg("P2Y_GameOver" ,msg));
}

void YYNotificationCenter::sendGetUserPicRequest(Player player)
{
    YYNotificationCenterDebug("YYNotificationCenter sendGetUserPicRequest!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    int usrId = player.getID();
    cJSON *root = cJSON_CreateObject();
    cJSON *user = cJSON_CreateObject();
    cJSON_AddNumberToObject(user , "ID" ,usrId);
    cJSON_AddItemToObject(root , "user" ,user);
    sendMsgList->append(YYNotificationCenterMsg("P2Y_GetUserPicRequest" , json2ByteArray(root)));
    cJSON_Delete(root);
}

void YYNotificationCenter::sendUserMoneyReq(Player player , Prop prop)
{
    YYNotificationCenterDebug("YYNotificationCenter sendGetUserPicRequest!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    int usrId = player.getID();
    int goodtype  = prop.getID();
    int goodnum = prop.getUseNumber();
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root , "id" , usrId);
    cJSON_AddNumberToObject(root , "goodtype" , goodtype);
    cJSON_AddNumberToObject(root , "goodnum" , goodnum);
    sendMsgList->append(YYNotificationCenterMsg("P2Y_UserMoneyReq" , json2ByteArray(root)));
    cJSON_Delete(root);

}

void YYNotificationCenter::sendUsePropRequest(Prop prop)
{
    YYNotificationCenterDebug("YYNotificationCenter sendUsePropRequest!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    cJSON *jsRoot = cJSON_CreateObject();
    {
        cJSON *jsProp = cJSON_CreateObject();
        {
            cJSON_AddNumberToObject(jsProp, "ID", prop.getID());
            cJSON_AddNumberToObject(jsProp, "number", prop.getUseNumber());
        }
        cJSON_AddItemToObject(jsRoot, "prop", jsProp);
    }

    sendMsgList->append(YYNotificationCenterMsg("P2Y_UsePropRequest" , json2ByteArray(jsRoot)));
    cJSON_Delete(jsRoot);
}

void YYNotificationCenter::sendSetSingerList(QList<Player> singerList)
{
    YYNotificationCenterDebug("YYNotificationCenter sendSetSingerList!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON *jsSingerList = cJSON_CreateArray();
    for(int i = 0 ; i < singerList.size() ; i++)
    {
        int id = singerList[i].getID();
        cJSON *singer = cJSON_CreateObject();
        cJSON_AddNumberToObject(singer , "ID", id);
        cJSON_AddItemToArray(jsSingerList , singer);
    }
    cJSON_AddItemToObject(root ,"singerList" ,jsSingerList);
    sendMsgList->append(YYNotificationCenterMsg("P2Y_SetSingerList" , json2ByteArray(root)));
    cJSON_Delete(root);
}

void YYNotificationCenter::sendSwitchSinger(Player curPlayer, Player nextPlayer)
{
    YYNotificationCenterDebug("YYNotificationCenter sendSwitchSinger!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    YYNotificationCenterDebug("Ihou send <sendSwitchSinger> notify to yyNC succeed!");

    int curId = curPlayer.getID();
    int nextId = nextPlayer.getID();
    cJSON *root = cJSON_CreateObject();

    cJSON *curUser = cJSON_CreateObject();
    cJSON_AddNumberToObject(curUser , "ID" ,curId);
    cJSON_AddItemToObject(root , "curSinger" ,curUser);

    cJSON *nextUser = cJSON_CreateObject();
    cJSON_AddNumberToObject(nextUser , "ID" ,nextId);
    cJSON_AddItemToObject(root , "nextSinger" ,nextUser);

    sendMsgList->append(YYNotificationCenterMsg("P2Y_SwitchSinger" , json2ByteArray(root)));
    cJSON_Delete(root);
}

void YYNotificationCenter::sendPCMBegin(int sampleRate, int channels, int bits)
{
    YYNotificationCenterDebug("YYNotificationCenter sendPCMBegin!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON *format = cJSON_CreateObject();
    cJSON_AddNumberToObject(format , "sampleRate" ,sampleRate);
    cJSON_AddNumberToObject(format , "channels" ,channels);
    cJSON_AddNumberToObject(format , "bits" ,bits);
    cJSON_AddItemToObject(root , "format" ,format);

    sendMsgList->append(YYNotificationCenterMsg("P2Y_PCMBegin" , json2ByteArray(root)));
    cJSON_Delete(root);
}

void YYNotificationCenter::sendPCMData(QByteArray pcmData)
{
    YYNotificationCenterDebug("YYNotificationCenter sendPCMData! len:%d", pcmData.size());
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

//    YYNotificationCenterDebug("pcmlen:%d" , pcmData.size());
    sendMsgList->append(YYNotificationCenterMsg("P2Y_PCMData" , pcmData));
}

void YYNotificationCenter::sendPCMEnd()
{
    YYNotificationCenterDebug("YYNotificationCenter sendPCMEnd!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    YYNotificationCenterDebug("PCMEnd" );
    QByteArray msg;
    msg.clear();
    sendMsgList->append(YYNotificationCenterMsg("P2Y_PCMEnd" ,msg));
}

void YYNotificationCenter::sendPluginInfo(QString company, QString name, QString version)
{
    YYNotificationCenterDebug("YYNotificationCenter sendPluginInfo!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON *plugin = cJSON_CreateObject();
    cJSON_AddStringToObject(plugin , "company" ,company.toUtf8().data());
    cJSON_AddStringToObject(plugin , "name" ,name.toUtf8().data());
    cJSON_AddStringToObject(plugin , "version" ,version.toUtf8().data());
    cJSON_AddItemToObject(root , "plugin" ,plugin);

    sendMsgList->append(YYNotificationCenterMsg("P2Y_PluginInfo" , json2ByteArray(root)));
    cJSON_Delete(root);
}

void YYNotificationCenter::sendHostPKInfo(P2Y_PKInfo *pKInfo)
{
    YYNotificationCenterDebug("YYNotificationCenter sendHostPKInfo!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    pKInfo->pKEndTime.setDate(QDate::currentDate());
    pKInfo->pKEndTime.setTime(QTime::currentTime());
    pKInfo->pKDuration = pKInfo->pKEndTime.toTime_t() - pKInfo->pKStartTime.toTime_t();

    cJSON *root = cJSON_CreateObject();

    cJSON_AddNumberToObject(root, "StartPKCount", pKInfo->startPKCount);
    cJSON_AddNumberToObject(root, "StartPKSuccessCount", pKInfo->startPKSuccessCount);
    cJSON_AddNumberToObject(root, "JoinPkUserCount", pKInfo->joinPKUserCount);
    cJSON_AddBoolToObject(root, "BZeroStart", pKInfo->bZeroStart);
    cJSON_AddStringToObject(root, "PKStartTime", pKInfo->pKStartTime.toString("yyyy:MM:dd hh:mm:ss").toStdString().c_str());
    cJSON_AddStringToObject(root, "PKEndTime", pKInfo->pKEndTime.toString("yyyy:MM:dd hh:mm:ss").toStdString().c_str());
    cJSON_AddNumberToObject(root, "PKDuration", pKInfo->pKDuration);
//    cJSON_AddBoolToObject(root, "BRecvPlayPK", pKInfo->bRecvPlayPK);
//    cJSON_AddBoolToObject(root, "BAcceptPlayPK", pKInfo->bAcceptPlayPK);

    sendMsgList->append(YYNotificationCenterMsg("P2Y_PKInfo" , json2ByteArray(root)));
    cJSON_Delete(root);
}

void YYNotificationCenter::sendAudiencePKInfo(const P2Y_PKInfo *pKInfo)
{
    YYNotificationCenterDebug("YYNotificationCenter sendAudiencePKInfo!");
    if (NULL == sendMsgList)
    {
        YYNotificationCenterDebug("return cause sendMsgList is NULL!");
        return;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddBoolToObject(root, "BRecvPlayPK", pKInfo->bRecvPlayPK);
    cJSON_AddBoolToObject(root, "BAcceptPlayPK", pKInfo->bAcceptPlayPK);

    sendMsgList->append(YYNotificationCenterMsg("P2Y_PKInfo" , json2ByteArray(root)));
    cJSON_Delete(root);
}

void YYNotificationCenter::handleThreadOnStarted()
{
    YYNotificationCenterDebug("YYNotificationCenter handleThreadOnStarted!");
    timerId = startTimer(5);
    thread->setObjectName("YYNotificationCenterThread-"+ QString::number((int)QThread::currentThreadId()));
    //    pipe = new YYPipe(this->pipeName);
    pipe = new YYTCPPipe;
    QObject::connect(QThread::currentThread(), SIGNAL(finished()), pipe, SLOT(disconnectFromServer()));
    QObject::connect(QThread::currentThread(), SIGNAL(finished()), pipe, SLOT(deleteLater()));
    QObject::connect(pipe, SIGNAL(onRecvMsg(QString,QByteArray)), this, SLOT(hanldePipeOnRecvMsg(QString,QByteArray)));
    QObject::connect(pipe, SIGNAL(onDisconnected()), this, SLOT(handlePipeOnDisconnected()));

    this->pipeState = PipeConnecting;
    if (!pipe->connectToServer(this->port))
    {
        this->pipeState = PipeError;
    }
    else
    {
        this->pipeState = PipeConnected;
    }
}

void YYNotificationCenter::handleThreadOnFinished()
{
    YYNotificationCenterDebug("YYNotificationCenter handleThreadOnFinished!");
    if(timerId != -1)
        this->killTimer(timerId);
    if(pipe)
    {
        pipe->deleteLater();
        pipe = NULL;
    }
}

void YYNotificationCenter::handlePipeOnDisconnected()
{
    YYNotificationCenterDebug("YYNotificationCenter handlePipeOnDisconnected!");
    YYNotificationCenterDebug("onError! pipeBroken");
    emit onError(pipeBroken);
}

void YYNotificationCenter::hanldePipeOnRecvMsg(QString msgName, QByteArray msg)
{

    YYNotificationCenterDebug("hanldePipeOnRecvMsg msgName:%s , msgData is %s" , msgName.toUtf8().data() , msg.data());
    if("Y2P_Show" == msgName)
    {
        parseMsgOnShow(msg);
    }
    else if("Y2P_Move" == msgName)
    {
        parseMsgOnMove(msg);
    }
    else if("Y2P_Quit" == msgName)
    {
        parseMsgOnQuit();
    }
    else if ("Y2P_HostInfo" == msgName)
    {
//        parseMsgOnHostInfo(msg);
    }
    else if("Y2P_GetUserPicResponse" == msgName)
    {
        parseMsgOnGetUserPicResponse(msg);
    }
    else if("Y2P_UsePropResponse" == msgName)
    {
        parseMsgOnUsePropResponse(msg);
    }
    else if("Y2P_UserMoneyRsp" == msgName)
    {
        parseMsgOnUserMoneyResponse(msg);
    }
    else if("Y2P_SwitchSinger" == msgName)
    {
        parseMsgOnSwitchSinger(msg);
    }
    else if("Y2P_TimeStamp" == msgName)
    {
        parseMsgOnTimeStamp(msg);
    }
    else if("Y2P_SetVolume" == msgName)
    {
        parseMsgOnSetVolume(msg);
    }
    else if("Y2P_GetPluginInfo" == msgName)
    {
        parseMsgOnGetPluginInfo();
    }
    else if("Y2P_SomebodyMicDown" == msgName)
    {
        parseMicDown(msg);
    }

}

void YYNotificationCenter::parseMsgOnShow(QByteArray &msg)
{
    YYNotificationCenterDebug("parseMsgOnShow");
    cJSON *root = cJSON_Parse(msg.data());
    bool bShow  = cJSON_GetObjectItem(root, "bShow")->type;
    cJSON_Delete(root);
    YYNotificationCenterDebug("bShow %d" , bShow);
    emit onShow(bShow);
}

void YYNotificationCenter::parseMsgOnMove(QByteArray &msg)
{
    YYNotificationCenterDebug("parseMsgOnMove");
    cJSON *root = cJSON_Parse(msg.data());
    cJSON *geometry = cJSON_GetObjectItem(root, "geometry");
    int x = cJSON_GetObjectItem(geometry, "x")->valueint;
    int y = cJSON_GetObjectItem(geometry, "y")->valueint;
    int width = cJSON_GetObjectItem(geometry, "width")->valueint;
    int height = cJSON_GetObjectItem(geometry, "height")->valueint;

    cJSON_Delete(root);
    YYNotificationCenterDebug("x %d , y %d, width %d ,height %d" ,x,y,width ,height);
    emit onMove(QRect(x, y , width , height));
}

void YYNotificationCenter::parseMsgOnQuit()
{
    YYNotificationCenterDebug("parseMsgOnQuit");
    emit onQuit();
}

void YYNotificationCenter::parseMsgOnHostInfo(QByteArray &msg)
{
    YYNotificationCenterDebug("parseMsgOnHostInfo");

    Player host;

    /*
    {
        "host":
        {
            "host":
            {
                "ID":0,
                "name":"",
                "pic":"",
                "isHost":false
            }
        }
    }
    */

    cJSON *jsRoot = cJSON_Parse(msg);
    {
        cJSON *jsHost = cJSON_GetObjectItem(jsRoot, "host");
        {
            host.setID(cJSON_GetObjectItem(jsHost, "ID")->valueint);
            host.setName(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "name")->valuestring));
            host.setPicFileSvPath(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "pic")->valuestring));
            host.setIsHost(cJSON_GetObjectItem(jsHost, "isHost")->type == cJSON_True);

            YYNotificationCenterDebug("host.getID() is: %d", host.getID());
            YYNotificationCenterDebug("host.getName() is: %s", host.getName().toUtf8().data());
            YYNotificationCenterDebug("host.getPicFileSvPath() is: %s", host.getPicFileSvPath().toUtf8().data());
            YYNotificationCenterDebug("host.getIsHost() is: %s", host.getIsHost() ? "true" : "false");
        }
    }
    cJSON_Delete(jsRoot);

    emit onHostInfo(host);
}

void YYNotificationCenter::parseMsgOnGetUserPicResponse(QByteArray &msg)
{
    YYNotificationCenterDebug("parseMsgOnGetUserPicResponse");
    cJSON *root = cJSON_Parse(msg.data());
    cJSON *user = cJSON_GetObjectItem(root, "user");
    int id = cJSON_GetObjectItem(user, "ID")->valueint;

    cJSON *response = cJSON_GetObjectItem(root , "response");
    int retcode = cJSON_GetObjectItem(response , "retcode")->valueint;
    QString pic = QString(cJSON_GetObjectItem(response , "pic")->valuestring);

    cJSON_Delete(root);
    YYNotificationCenterDebug("usrId %d , retcode %d ,pic %s" , id , retcode , pic.toUtf8().data());
    Player player;
    player.setPicFileSvPath(pic);
    player.setID(id);
    emit onGetUserPicResponse(player , retcode );
}

void YYNotificationCenter::parseMsgOnUsePropResponse(QByteArray &msg)
{
    YYNotificationCenterDebug("parseMsgOnUsePropResponse");
    cJSON *root = cJSON_Parse(msg.data());
    cJSON *jsProp = cJSON_GetObjectItem(root, "prop");
    int id = cJSON_GetObjectItem(jsProp, "ID")->valueint;
    int number = cJSON_GetObjectItem(jsProp, "number")->valueint;

    cJSON *response = cJSON_GetObjectItem(root , "response");
    int retcode = cJSON_GetObjectItem(response , "retcode")->valueint;
    QString orderNumber = QString(cJSON_GetObjectItem(response , "orderNumber")->valuestring);
    QString signature = QString(cJSON_GetObjectItem(response , "signature")->valuestring);

    /*QFile propResponse(ConfigHelper::getInstance()->getPluginPath()+"prop.dat");
    propResponse.open(QFile::WriteOnly);
    propResponse.write(msg);
    propResponse.close();*/
    cJSON_Delete(root);
    YYNotificationCenterDebug("id %d , number %d , retcode %d , orderNumber %s,signature %s",id ,number ,
                              retcode , orderNumber.toUtf8().data() , signature.toUtf8().data());
    Prop prop;
    prop.setID(id);
    prop.setUseNumber(number);
    emit onUsePropResponse(prop ,retcode , orderNumber , signature);
}

void YYNotificationCenter::parseMsgOnUserMoneyResponse(QByteArray &msg)
{
    YYNotificationCenterDebug("parseMsgOnUserMoneyResponse");
    cJSON *root = cJSON_Parse(msg.data());
    Player player;
    Prop prop;
    int result;
    player.setID(cJSON_GetObjectItem(root , "id")->valueint);
    prop.setID(cJSON_GetObjectItem(root , "goodtype")->valueint);
    prop.setUseNumber(cJSON_GetObjectItem(root , "goodnum")->valueint);
    result = cJSON_GetObjectItem(root , "result")->valueint;
    cJSON_Delete(root);
    emit onUserMoneyResponse(player , prop , result);
}

void YYNotificationCenter::parseMsgOnSwitchSinger(QByteArray &msg)
{
    //YYNotificationCenterDebug("Ihou recv <Y2P_SwitchSinger> response from yyNC succeed! ");
    YYNotificationCenterDebug("Ihou recv <Y2P_SwitchSinger> response from yyNC succeed! ");

    QList<Player> singerList;

    // if failed, YY may return a NULL string
    if (msg.length() <= 0)
    {
        emit onSwitchSingerResponse(singerList);
        return;
    }


    cJSON *root = cJSON_Parse(msg.data());
    cJSON *jsSingerList = cJSON_GetObjectItem(root, "userList");
    for (int i = 0; i < cJSON_GetArraySize(jsSingerList); i++)
    {
        cJSON *jsSinger = cJSON_GetArrayItem(jsSingerList, i);
        Player player;
        player.setID(cJSON_GetObjectItem(jsSinger , "ID")->valueint);
        singerList.append(player);
    }
    cJSON_Delete(root);

    emit onSwitchSingerResponse(singerList);

}

void YYNotificationCenter::parseMsgOnTimeStamp(QByteArray &msg)
{
//    YYNotificationCenterDebug("parseMsgOnTimeStamp");
    cJSON *root = cJSON_Parse(msg.data());
    cJSON *user = cJSON_GetObjectItem(root, "user");
    int id = cJSON_GetObjectItem(user, "ID")->valueint;
    float time = cJSON_GetObjectItem(root, "time")->valuedouble;

    cJSON_Delete(root);
//    YYNotificationCenterDebug("id %d , time %f" ,id , time);

    Player player;
    player.setID(id);
    emit onTimeStamp(player , time);
}

void YYNotificationCenter::parseMsgOnSetVolume(QByteArray &msg)
{
    YYNotificationCenterDebug("parseMsgOnSetVolume");
    cJSON *root = cJSON_Parse(msg.data());
    cJSON *volume = cJSON_GetObjectItem(root, "volume");
    int left = cJSON_GetObjectItem(volume, "left")->valueint;
    int right = cJSON_GetObjectItem(volume, "right")->valueint;
    bool bMute = cJSON_GetObjectItem(volume, "bMute")->type;

    cJSON_Delete(root);
    YYNotificationCenterDebug("left %d ,right %d ,mute %d" ,left ,right ,bMute);
    emit onSetVolume(left ,right , bMute);
}

void YYNotificationCenter::parseMsgOnGetPluginInfo()
{
    YYNotificationCenterDebug("parseMsgOnGetPluginInfo");
    emit onGetPluginInfo();
}

void YYNotificationCenter::parseMicDown(QByteArray msg)
{
    YYNotificationCenterDebug("parseMicDown");
    QList<Player> micDownList;

    cJSON *root = cJSON_Parse(msg.data());
    cJSON *jsMicDownList = cJSON_GetObjectItem(root, "MicDownList");
    for (int i = 0; i < cJSON_GetArraySize(jsMicDownList); i++)
    {
        cJSON *jsPlayer = cJSON_GetArrayItem(jsMicDownList, i);
        Player player;
        player.setID(cJSON_GetObjectItem(jsPlayer , "id")->valueint);
        micDownList.append(player);
    }
    cJSON_Delete(root);

    emit onMicTakenDown(micDownList);
}

void YYNotificationCenter::timerEvent(QTimerEvent *)
{
    if (sendMsgList->count() <= 0)
    {
        return;
    }

    YYNotificationCenterDebug("sendMsgList->count() is: %d", sendMsgList->count());

    YYNotificationCenterMsg msg = sendMsgList->takeFirst() ;

    if (!pipe->sendMsg(msg.name, msg.data))
    {
        YYNotificationCenterDebug("sendMsg failed! msg.name is: %s", msg.name.toUtf8().data());
        pipe->disconnectFromServer();
        emit onError(sendMessageFailed);
    }

    YYNotificationCenterDebug("sendMsg succeed! msg.name is: %s",
                              msg.name.toUtf8().data());
}


bool YYNotificationCenter::start(int port)
{
    YYNotificationCenterDebug("YYNotificationCenter start!");

    sendMsgList = new QLinkedList<YYNotificationCenterMsg>;
    this->port = port;

    thread = new QThread();


    this->moveToThread(thread);
    QObject::connect(thread, SIGNAL(started()), this, SLOT(handleThreadOnStarted()));

    this->pipeState = PipeConnecting;
    thread->start(QThread::HighestPriority);

    while (this->pipeState == PipeConnecting)
    {
        QCoreApplication::processEvents();
    }

    if (this->pipeState != PipeConnected)
    {
        YYNotificationCenterDebug("start failed!");
        return false;
    }

    YYNotificationCenterDebug("start succeed!");
    return true;
}

void YYNotificationCenter::stop()
{
    YYNotificationCenterDebug("YYNotificationCenter stop!");

    thread->quit();
    thread->wait(100);  // wait for 100 milliseconds

    delete thread;

    sendMsgList->clear();
    delete sendMsgList;
}




