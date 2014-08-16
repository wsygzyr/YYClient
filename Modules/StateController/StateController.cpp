#include "StateController.h"
#include "Modules/ExceptionController/ExceptionController.h"

StateController::StateController()
{
    this->curPluginState = NoneState;
    pkinfo = PkInfoManager::getInstance();
    eventHandler = new EventHandler();
    nodeNC = NodeNotificationCenter::getInstance();
    resManager = ResManager::getInstance();
    resetResState();
    totalTime = 0;
}

StateController::~StateController()
{
    StateControllerDebug("~StateController()");
    if(NULL != eventHandler)
    {
        eventHandler->deleteLater();
        eventHandler = NULL;
    }
}

QAtomicPointer<StateController> StateController::instance;
QMutex StateController::instanceMutex;

StateController* StateController::getInstance()
{
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            StateControllerDebug("create instance!");
            instance = new StateController();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void StateController::destory()
{
    StateControllerDebug("destroy!");
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}

void StateController::startStateMachine()
{
    this->initConnectInterface();
    this->setNextState(NoneState);
}

void StateController::stopStateMachine()
{
    this->setNextState(NoneState);
}

//slots handle func
void StateController::initConnectInterface()
{
    QObject::connect(nodeNC, SIGNAL(onLoginStatusServer(int)), this, SLOT(handleNodeNotificationCenterOnLoginStatusServer(int)));
    QObject::connect(nodeNC, SIGNAL(onLoginNodeServer(int)), this, SLOT(handleNodeNotificationCenterOnLoginNodeServer(int)));
    QObject::connect(nodeNC, SIGNAL(onLoginStatusResult(int)), this, SLOT(handleNodeNotificationCenterOnLoginStatusResult(int)));
    QObject::connect(nodeNC, SIGNAL(onLoginNodeResult(int)), this, SLOT(handleNodeNotificationCenterOnLoginNodeResult(int)));
    QObject::connect(nodeNC, SIGNAL(onPKInvite(Player, Song, Prop, QList<VictoryReward>, int)),this, SLOT(handleNodeNotificationCenterOnPKInvite(Player, Song,Prop, QList<VictoryReward>,int)));
    QObject::connect(nodeNC, SIGNAL(onBidInfo(int, Prop, Prop)), this, SLOT(handleNodeNotificationCenterOnBidInfo(int, Prop, Prop)));
    QObject::connect(nodeNC, SIGNAL(onBidStop(int)), this, SLOT(handleNodeNotificationCenterOnBidStop(int)));
    QObject::connect(nodeNC, SIGNAL(onBidConfirm(Prop)), this, SLOT(handleNodeNotificationCenterOnBidConfirm(Prop)));
    QObject::connect(nodeNC, SIGNAL(onBidSettlement(int)), this, SLOT(handleNodeNotificationCenterOnBidSettlement(int)));
    QObject::connect(nodeNC, SIGNAL(onSingReady(QList<SingReadyResult>)), this, SLOT(handleNodeNotificationCenterOnSingReady(QList<SingReadyResult>)));
    QObject::connect(nodeNC, SIGNAL(onPKStart(QList<Player>)), this, SLOT(handleNodeNotificationCenterOnPKStart(QList<Player>)));
    QObject::connect(nodeNC, SIGNAL(onSingPrepare(int)), this, SLOT(handleNodeNotificationCenterOnSingPrepare(int)));
    QObject::connect(nodeNC, SIGNAL(onSingChange(int, QList<int>)), this, SLOT(handleNodeNotificationCenterOnSingChange(int, QList<int>)));
    QObject::connect(nodeNC, SIGNAL(onPKResult(QList<Score>, int, Prop, QList<int>)), this, SLOT(handleNodeNotificationCenterOnPKResult(QList<Score>, int, Prop, QList<int>)));
    QObject::connect(nodeNC, SIGNAL(onSelectReward(VictoryReward)), this, SLOT(handleNodeNotificationCenterOnSelectReward(VictoryReward)));
    QObject::connect(nodeNC, SIGNAL(onUpdateHeat(int)), this, SLOT(handleNodeNotificationCenterOnUpdateHeat(int)));
    QObject::connect(nodeNC, SIGNAL(onSingTimeOffset(float)), this, SLOT(handleNodeNotificationCenterOnSingTimeOffset(float)));
    QObject::connect(nodeNC, SIGNAL(onScore(Score, Score)), this, SLOT(handleNodeNotificationCenterOnScore(Score, Score)));
    QObject::connect(nodeNC, SIGNAL(onPropConfirm(Prop)), this, SLOT(handleNodeNotificationCenterOnPropConfirm(Prop)));
    QObject::connect(nodeNC, SIGNAL(onPropActive(Player, Prop)), this, SLOT(handleNodeNotificationCenterOnPropActive(Player, Prop)));
    QObject::connect(nodeNC, SIGNAL(onErrorEvent(int)), this, SLOT(handleNodeNotificationCenterOnErrorEvent(int)));
    QObject::connect(nodeNC, SIGNAL(onUploadLog()), this, SLOT(handleNodeNotificationCenterOnUploadLog()));
    QObject::connect(nodeNC, SIGNAL(onUploadDumpFile()), this, SLOT(handleNodeNotificationCenterOnUploadDumpFile()));
    QObject::connect(nodeNC, SIGNAL(onConnectServerResult(NodeNotificationCenter::eServerType, NodeNotificationCenter::eConnectErrorCode)), this, SLOT(handleNodeNotificationCenterOnConnectServerResult(NodeNotificationCenter::eServerType, NodeNotificationCenter::eConnectErrorCode)));
    QObject::connect(nodeNC, SIGNAL(onRoomInfo(int, int, Prop, Prop, int, int)), this, SLOT(handleNodeNotificationCenterOnRoominfoPk(int, int, Prop, Prop, int, int)));
    QObject::connect(resManager,SIGNAL(onDownloadResPrepared(QString,bool)) ,this ,SLOT(handleResManageronDownloadResPrepared(QString,bool)));
}


void StateController::handleNodeNotificationCenterOnLoginStatusServer(int nonce)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnLoginStatusServer");
    eventHandler->setCurEvent(EventHandler::StatusSyncTimeEvent);
    if(this->needProcessEvent(eventHandler->getCurEvent()))
    {
        nodeNC->sendOnLoginStatusServer(nonce);
        this->setNextState(LoginStatusState);
        return;
    }
    StateControllerDebug("StateController current event is:%d, current state is:%d, it's wrong, not handle!!!"
          ,eventHandler->getCurEvent(), getState());
}

void StateController::handleNodeNotificationCenterOnLoginStatusResult(int result)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnLoginStatusResult");
    eventHandler->setCurEvent(EventHandler::LoginStatusResultEvent);
    if(this->needProcessEvent(eventHandler->getCurEvent()))
    {
        //0:login ok, -1:login failed
        if(LoginSucceed == static_cast<LoginResCode>(result))
        {
            this->setNextState(LoginedStatusState);
        }
        else
        {
            this->setNextState(NoneState);
        }

        //-1: login auth failed
        //0： login succeed
        //if failed, wait 5 seconds, trigger
        //StatusNetworkDisconnEvent/NodeNetworkDisconnEvent
        emit onStateLoginResult(result);
        return;
    }
    StateControllerDebug("StateController current event is:%d, current state is:%d, it's wrong, not handle!!!"
            ,eventHandler->getCurEvent(), getState());

}


void StateController::handleNodeNotificationCenterOnLoginNodeServer(int nonce)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnLoginNodeServer");
    eventHandler->setCurEvent(EventHandler::NodeSyncTimeEvent);
    if(this->needProcessEvent(eventHandler->getCurEvent()))
    {
        nodeNC->sendOnLoginNodeServer(nonce);
        this->setNextState(LoginNodeState);
        return;
    }
    StateControllerDebug("StateController current event is:%d, current state is:%d, it's wrong, not handle!!!"
             ,eventHandler->getCurEvent(), getState());
}

void StateController::handleNodeNotificationCenterOnLoginNodeResult(int resCode)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnLoginNodeResult");
    eventHandler->setCurEvent(EventHandler::LoginNodeResultEvent);
    if(this->needProcessEvent(eventHandler->getCurEvent()))
    {
        //0:login ok, -1:login failed
        if(LoginSucceed == static_cast<LoginResCode>(resCode))
        {
            this->setNextState(LoginedNodeState);
        }
        else if(LoginFailed == static_cast<LoginResCode>(resCode))
        {
            this->setNextState(NoneState);
        }
        else if(GameNoStart == static_cast<LoginResCode>(resCode))
        {
            StateControllerDebug("StateController game is not start");
        }
        else if(otherLoginError == static_cast<LoginResCode>(resCode))
        {
            StateControllerDebug("StateController other login error");
        }
        emit onStateLoginResult(resCode);

        return;
    }
    StateControllerDebug("StateController current event is:%d, current state is:%d, it's wrong, not handle!!!"
         ,eventHandler->getCurEvent(), getState());
}

void StateController::handleNodeNotificationCenterOnPKInvite(Player host,
                                      Song song,
                                      Prop bidProp,
                                      QList<VictoryReward> VictoryRewardList,
                                      int acceptedPlayers)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnPKInvite");
    eventHandler->setCurEvent(EventHandler::PkInviteEvent);
    if(this->needProcessEvent(eventHandler->getCurEvent()))
    {
        emit onStatePKInvite(host, song, bidProp, VictoryRewardList, acceptedPlayers);
        this->setNextState(InviteWaitState);
        return;
    }
    StateControllerDebug("StateController current event is:%d, current state is:%d, it's wrong, not handle!!!"
             ,eventHandler->getCurEvent(), getState());
}

void StateController::handleNodeNotificationCenterOnBidInfo(int biderNumber, Prop leastProp, Prop expectIncome)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnBidInfo bindinfo num=%d,curstate=%d", biderNumber, getState());
    emit onStateBidInfo(biderNumber, leastProp, expectIncome);
}

void StateController::handleNodeNotificationCenterOnBidSettlement(int completeNum)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnBidSettlement");
    if((0 == completeNum && BidState != getState())
            || ( 0 < completeNum && SettleState != getState()))
    {
        StateControllerDebug("current state is:%d, it's not BidState & SettleState", getState());
        return;
    }

    emit onStateBidSettlement(completeNum);
    if(0 == completeNum)
    {
        StateControllerDebug("set state : SettleState");
        this->setNextState(SettleState);
    }
}

void StateController::handleNodeNotificationCenterOnBidStop(int countDown)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnBidStop");

    eventHandler->setCurEvent(EventHandler::BidStopEvent);
    if(this->needProcessEvent(eventHandler->getCurEvent()))
    {
        emit onStateBidStop(countDown);
        return;
    }
    StateControllerDebug("StateController current event is:%d, current state is:%d, it's wrong, not handle!!!"
            ,eventHandler->getCurEvent(), getState());
}

void StateController::handleNodeNotificationCenterOnBidConfirm(Prop prop)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnBidConfirm");
    emit onStateBidConfirm(prop);
}

void StateController::handleNodeNotificationCenterOnPKStart(QList<Player> playerList)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnPKStart");
    eventHandler->setCurEvent(EventHandler::PkStartEvent);
    if(this->needProcessEvent(eventHandler->getCurEvent()))
    {
        if(1 >= playerList.size())
        {
            StateControllerDebug("StateController playerlist letter than 1, no pkstart");
            emit onStateBidFailed();
            setNextState(NoneState);
            return;
        }

        pkinfo->setCurrentPkPlayerIndex(0);
        emit onStatePKStart(playerList);

        this->setNextState(PkState);
        //send host singchange
        //clear singend list at first
        if(!(resState & RECV_SING_CHANGE))
        {
            QList<int> singEndCodeList;
            singEndCodeList.clear();
            pkinfo->setLastSingEndRetcode(singEndCodeList);
        }

        handleNodeNotificationCenterOnSingChange(0, pkinfo->getLastSingEndRetcode());

        return;
    }
    StateControllerDebug("StateController current event is:%d, current state is:%d, it's wrong, not handle!!!"
               ,eventHandler->getCurEvent(), getState());
}

void StateController::handleNodeNotificationCenterOnSingChange(int sectionIndex, QList<int> singEndList)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnSingChange");
    for(int i = 0; i < singEndList.size(); i++)
    {
        StateControllerDebug("StateController 33333 singEndList[%d]=%d", i, singEndList[i]);
    }

    resState |= RECV_SING_CHANGE;
    if(PkState != getState())
    {
        StateControllerDebug("StateController current state=%d, not pkstate", getState());
        return;
    }

    //set current singer index
    pkinfo->setCurrentPkPlayerIndex(sectionIndex);
    StateControllerDebug("curSingerIndex =%d", sectionIndex);

    //define current singer resource download state
    int curSingerDownloadRet = 1;
    QList<SingReadyResult> dlResultList = pkinfo->getResDownloadRetcode();
    QList<SingReadyResult>::iterator iter = dlResultList.begin();
    for(; iter != dlResultList.end(); iter++)
    {
        StateControllerDebug("download list get singer index =%d", (*iter).sectionIndex);
        if((*iter).sectionIndex == sectionIndex)
        {
            curSingerDownloadRet = (*iter).retcode;
            StateControllerDebug("current singer resdownlaod result code:%d", curSingerDownloadRet);
            break;
        }
    }

    //if i am singer now and resource download uncomplete, start timer
    if(pkinfo->getMe().getIsHost() || sectionIndex == pkinfo->getMeSectionIndex())
    {
        StateControllerDebug("current i am singer, start timer...");
        if(( resState & RECV_XML_ING || resState & RECV_SECTION_ING || resState & RECV_MP3_ING))
        {
            StateControllerDebug("current singer resource downloading, start timer wait 30s and check......");
            if(0 != downLoadResTimer)
            {
                killTimer(downLoadResTimer);
                downLoadResTimer = 0;
            }
            downLoadResTimer = startTimer(1000);
        }
    }
    else //for test 20140814 16:58
    {
        StateControllerDebug("current i am not  singer, start timer...");
        if(( resState & RECV_XML_ING || resState & RECV_SECTION_ING ))
        {
            StateControllerDebug("current audience resource downloading, start timer wait 30s and check......");
            StateControllerDebug("timer id =%d",downLoadResTimer);
            if(0 != downLoadResTimer)
            {
                killTimer(downLoadResTimer);
                downLoadResTimer = 0;
            }
            downLoadResTimer = startTimer(1000);
        }
    }

    StateControllerDebug("send singchange pkinfo->getMeSectionIndex()=%d, section=%d, resState=%d", pkinfo->getMeSectionIndex(),sectionIndex, resState);
    emit onStateSingChange(sectionIndex, singEndList, curSingerDownloadRet, resState);
}

void StateController::handleNodeNotificationCenterOnPKResult(QList<Score> scoreList, int heat, Prop propIncome, QList<int> lastretCodeList)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnPKResult");
    eventHandler->setCurEvent(EventHandler::PkResultEvent);
    if(this->needProcessEvent(eventHandler->getCurEvent()))
    {
        emit onStatePKResult(scoreList, heat, propIncome, lastretCodeList);
        this->setNextState(PkEndState);
        return;
    }
    StateControllerDebug("StateController current event is:%d, current state is:%d, it's wrong, not handle!!!"
         ,eventHandler->getCurEvent(), getState());
}

void StateController::handleNodeNotificationCenterOnSingPrepare(int sectionIndex)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnSingPrepare");
    if(PkState != getState())
    {
        StateControllerDebug("StateController current state is:%d, it's wrong, not handle!!!",getState());
        return;
    }
    emit onStateSingPrepare(sectionIndex);
}

void StateController::handleNodeNotificationCenterOnSelectReward(VictoryReward var)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnSelectReward");
    if(PkEndState != getState())
    {
        StateControllerDebug("StateController current state is:%d, it's wrong, not handle!!!",getState());
        return;
    }

    emit onStateSelectReward(var);
}

void StateController::handleNodeNotificationCenterOnUpdateHeat(int heat)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnUpdateHeat");
    emit onStateUpdateHeat(heat);
}

void StateController::handleNodeNotificationCenterOnSingTimeOffset(float offsetTime)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnSingTimeOffset");
    emit onStateSingTimeOffset(offsetTime);
}

void StateController::handleNodeNotificationCenterOnScore(Score sentenceScore, Score SumScore)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnScore");
    emit onStateScore(sentenceScore, SumScore);
}

void StateController::handleNodeNotificationCenterOnPropConfirm(Prop prop)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnPropConfirm");
    emit onStatePropConfirm(prop);
}

void StateController::handleNodeNotificationCenterOnPropActive(Player player, Prop prop)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnPropActive");
    emit onStatePropActive(player, prop);
}


void StateController::handleNodeNotificationCenterOnErrorEvent(int errorCode)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnErrorEvent receive error event id:%d",errorCode);
    emit onStateErrorEvent(errorCode);

    //send to  ExceptionController
    ExceptionController *exceptionCtrl = ExceptionController::getInstance();
    ExceptionController::eExceptionNo  exceptionNo;
    switch(errorCode)
    {
    case -1: //host micphone is offline
        exceptionNo = ExceptionController::eHostLostConnectionExcep;
        break;
    case -2: //player repeat login
        exceptionNo = ExceptionController::ePlayerRepetitionLoginExcep;
        break;
    case -3: //room progress exception
        exceptionNo = ExceptionController::eRoomProgressErrorExcep;
        break;
    default:
        StateControllerDebug("receive error event id:%d, not need to handle",errorCode);
        return;
    }
    StateControllerDebug("send error event to ExceptionController");
    exceptionCtrl->setExceptionNo(exceptionNo);
}

void StateController::handleNodeNotificationCenterOnUploadLog()
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnUploadLog");
    QList<Player> playerList = PkInfoManager::getInstance()->getPkPlayerList();
    int i = 0;
    for (; i != playerList.size(); ++i)
    {
        if (PkInfoManager::getInstance()->getMe().getID() == playerList[i].getID())
        {
            StateControllerDebug("player %d is uploading logfile", PkInfoManager::getInstance()->getMe().getID());
            emit onStateUploadLog();
            break;
        }
    }
    if (i == playerList.size())
    {
        StateControllerDebug("player %d is audience, do not upload logfile", PkInfoManager::getInstance()->getMe().getID());
    }
}


void StateController::handleNodeNotificationCenterOnUploadDumpFile()
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnUploadDumpFile");
    emit onStateUploadDumpFile();
}


void StateController::handleNodeNotificationCenterOnSingReady(QList<SingReadyResult> downloadList)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnSingReady");

    int curSingerIndex = pkinfo->getCurrentPkPlayerIndex();
    int SingerLastState = pkinfo->getCurPkPlayerDownloadState(curSingerIndex);

    bool needSendSingChange = false;
    for(int i = 0; i < downloadList.size(); i++)
    {
        if(downloadList[i].sectionIndex == curSingerIndex)
        {
            if(downloadList[i].retcode != SingerLastState)
            {
                StateControllerDebug("need to send singchange");
                needSendSingChange = true;
            }
            else
            {
                StateControllerDebug("not need to send singchange");
                needSendSingChange = false;
            }
        }
    }

    //print current resource download status
    for(int i=0; i < downloadList.size(); i++)
    {
        StateControllerDebug("SingReady, current download list--> sectionIndex=%d,recode=%d",downloadList[i].sectionIndex, downloadList[i].retcode);
    }

    //update pkinfo singready list
    pkinfo->setResDownloadRetcode(downloadList);

    //no change no send
    if(!needSendSingChange)
        return;

    //emit onStateSingReady(downloadList);
    if(resState & RECV_SING_CHANGE)
    {
        StateControllerDebug("StateController  receive singchange before singready");
        int curSingerIndex = pkinfo->getCurrentPkPlayerIndex();
        int curSingerDownloadRet = 1;
        QList<SingReadyResult>::iterator iter = pkinfo->getResDownloadRetcode().begin();
        for(; iter != pkinfo->getResDownloadRetcode().end(); iter++)
        {
            if((*iter).sectionIndex == curSingerIndex)
            {
                curSingerDownloadRet = (*iter).retcode;
                StateControllerDebug("StateController  get current singer resource state:%d",curSingerDownloadRet);
                break;
            }
        }
        emit onStateSingChange(curSingerIndex, pkinfo->getLastSingEndRetcode(), curSingerDownloadRet, resState);
    }
}

void StateController::handleNodeNotificationCenterOnRoominfoPk(int status, int bidnum, Prop leastProp,
                                                               Prop expIncome, int completed,
                                                               int heat)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnRoominfoPk"); 
    eventHandler->setCurEvent(EventHandler::RoomInfoEvent);
    if(!this->needProcessEvent(eventHandler->getCurEvent()))
    {
        StateControllerDebug("StateController current event is:%d, current state is:%d, it's wrong, not handle!!!"
                             ,eventHandler->getCurEvent(), getState());
        return;
    }

    QList<Player> playerList = pkinfo->getPkPlayerList();
    float timeoffset = pkinfo->getTimeOffset();
    int curSecIndex = pkinfo->getCurrentPkPlayerIndex();

    //send message by room status
    switch(status)
    {
    case NodeNotificationCenter::SpareTimeState:
        break;
    case NodeNotificationCenter::BidState:
        this->setNextState(BidState);
        eventHandler->downloadRes4PkState();
        emit onStateBidStart();
        emit onStateBidInfo(bidnum, leastProp, expIncome);
        break;
    case NodeNotificationCenter::SettlementState:
       this->setNextState(SettleState);
       emit onStateBidSettlement(completed);
       eventHandler->downloadRes4PkState();
       break;
    case NodeNotificationCenter::PkState:
        this->setNextState(PkState);
        eventHandler->downloadRes4PkState();
        emit onStatePKStart(playerList);
        if(pkinfo->getMe().getIsHost())
        {
            emit onStateSingPrepare(curSecIndex);
        }
        handleNodeNotificationCenterOnSingChange(curSecIndex, pkinfo->getLastSingEndRetcode());
        emit onStateSingTimeOffset(timeoffset);
        emit onStateUpdateHeat(heat);
       break;
    default:
       break;
    }
}

void StateController::handleNodeNotificationCenterOnConnectServerResult(NodeNotificationCenter::eServerType type, NodeNotificationCenter::eConnectErrorCode errorcode)
{
    StateControllerDebug("StateController handleNodeNotificationCenterOnConnectServerResult");
    //confirm reconnect server by cureent state
    //connect failed,send reconnect
    if(NodeNotificationCenter::connectFailed == errorcode)
    {
        if(NodeNotificationCenter::nodeServerType == type)
        {
            //if host disconnect with node, reconnect 3 times , if it still failed
            // send exception , and if audience reconnect 3 times, or else reconnect
            //status server,always.
            StateControllerDebug("StateController state check node server disconnect, reconnect 3 times failed");
            ExceptionController *exceptionCtrl = ExceptionController::getInstance();
            if(pkinfo->getMe().getIsHost())
            {
                exceptionCtrl->setExceptionNo(ExceptionController::eHostLostConnectionExcep);
            }
            else
            {
                exceptionCtrl->setExceptionNo(ExceptionController::eAudienceLostConnectionExcep);
                nodeNC->sendReconnServer(NodeNotificationCenter::statusServerType);
            }
        }
        else
        {
            nodeNC->sendReconnServer(type);
        }
        this->setNextState(NoneState);
    }
    else //send connect succeed
    {
        if(NodeNotificationCenter::statusServerType == type)
        {
            StateControllerDebug("StateController connect status server succeed!!!");
            this->setNextState(ConnectStatusState);
        }
        else
        {
            StateControllerDebug("StateController connect node server succeed!!!");
            this->setNextState(ConnectNodeState);
        }
    }
}


void StateController::handleResManageronDownloadResPrepared(QString keyWord, bool error)
{
    StateControllerDebug("StateController handleResManageronDownloadResPrepared");

    if(keyWord == pkinfo->getCurrentSong().getLyricFileName())
    {
        if(!error)
        {
            resState |= RECV_XML_OK;
            StateControllerDebug("download LYRIC file ok!!!!");
        }
        else
        {  
            StateControllerDebug("download LYRIC file error!!!!");
            nodeNC->sendSingReady(-1);
        }
        resState &= ~RECV_XML_ING;  //set ing for 0
        resState &= ~RECV_SECTION_ING;
        resState &= ~RECV_MP3_ING;
    }

    if(keyWord == pkinfo->getCurrentSong().getSectionFileName())
    {
        if(!error)
        {
            resState |= RECV_SECTION_OK;
            StateControllerDebug("download SECTION file ok!!!!");
        }
        else
        {
            StateControllerDebug("download SECTION file error!!!!");
            nodeNC->sendSingReady(-1);
        }
        resState &= ~RECV_XML_ING;
        resState &= ~RECV_SECTION_ING;
        resState &= ~RECV_MP3_ING;
    }

    if(keyWord == pkinfo->getCurrentSong().getAccompanyFileName())
    {
        if(!error)
        {
            resState |= RECV_MP3_OK;
            StateControllerDebug("download MP3 file ok!!!!");
        }

        resState &= ~RECV_MP3_ING;
    }

    if(error)
    {
        resState |= RECV_ERROR;
    }

    if((resState & RECV_XML_OK) && (resState & RECV_SECTION_OK) && (resState & RECV_MP3_OK) )
    {
        StateControllerDebug("handleResManageronDownloadResPrepared resource download succeed");
        nodeNC->sendSingReady(0);
    }

    if((resState & RECV_SING_CHANGE ) == 0)
    {
        //bidstate begin to download resource
        if(BidState != getState()
          && SettleState != getState()
          && PkState  != getState())
        {
            StateControllerDebug("StateController  current state=%d", getState());
            return;
        }
    } 
}

bool StateController::needProcessEvent(EventHandler::EventType event)
{
    //need added in this,if you have want to add event
    StateControllerDebug("check event type,EventId:%d, curstateId:%d",event, getState());
    bool ret = true;
    switch(event)
    {
    case EventHandler::StatusSyncTimeEvent:
         if(ConnectStatusState != getState())
             ret = false;
         break;
    case EventHandler::LoginStatusResultEvent:
         if(LoginStatusState != getState())
            ret = false;
         break;
    case EventHandler::PkInviteEvent:
         if(LoginedStatusState != getState())
             ret = false;
         break;
    case EventHandler::AcceptInviteEvent:
         if(InviteWaitState != getState()
            && LoginedStatusState != getState()) //need check
             ret = false;
         break;
    case EventHandler::NodeSyncTimeEvent:
         if(ConnectNodeState != getState())
             ret = false;
         break;
    case EventHandler::LoginNodeResultEvent:
        if(LoginNodeState != getState())
            ret = false;
        break;
    case EventHandler::RoomInfoEvent:
         if(LoginedNodeState != getState())
             ret = false;
         break;
    case EventHandler::BidStopEvent:
         if(BidState == getState()
            || LoginedNodeState == getState())
             ret = true;
         else
             ret = false;
         break;
    case EventHandler::PkStartEvent:
        if(BidState == getState()
           || LoginedNodeState == getState()
           || SettleState == getState()
           || PkState == getState())
            ret = true;
        else
            ret = false;
         break;
    case EventHandler::PkResultEvent:
         if(PkState != getState()
            && LoginedNodeState != getState())
             ret = false;
         break;
    case EventHandler::StatusNetworkDisconnEvent:
         if(pkinfo->getMe().getIsHost())
             ret = false;
         break;
    case EventHandler::NodeNetWorkDisconnEvent:
        ret = true;
        break;
    default:
        ret = false;
        break;
    }

    return ret;
}

//check resource download state, if it has been in a state of being downloaded
//and exceed 30s, then send downlaod fail
void StateController::timerEvent(QTimerEvent *downloadTimer)
{
    StateControllerDebug("StateController timerEvent, come in resource download waiting timer!!!");
    totalTime += 1000;
    StateControllerDebug("StateController timerEvent, wait %d seconds ...", totalTime/1000);
    int curSingerIndex;
    int curSingerResState;
    if(30*1000 <= totalTime)
    {
        killTimer(downLoadResTimer);
        downLoadResTimer = 0;
    }

    if(30*1000 == totalTime)
    {
        curSingerIndex = pkinfo->getCurrentPkPlayerIndex();

        if(pkinfo->getMe().getIsHost() || curSingerIndex == pkinfo->getMeSectionIndex())
        {
            if(resState & RECV_XML_ING
              || resState & RECV_SECTION_ING
              || resState & RECV_MP3_ING
              || resState & RECV_ERROR)
            {
                //downloading,timeout, kill timer and send download failed
                StateControllerDebug("wait 30s download failed still, send resource failed !!!!");
                curSingerResState = pkinfo->getCurPkPlayerDownloadState(curSingerIndex);
                StateControllerDebug("current index is:%d, download state:%d", curSingerIndex, curSingerResState);
                if(pkinfo->getMe().getIsHost())
                     emit onStateSingChange(0, pkinfo->getLastSingEndRetcode(), -1, resState);
                else
                    emit onStateSingChange(curSingerIndex, pkinfo->getLastSingEndRetcode(), -1, resState);
            }
         }
        else
        {
            if(resState & RECV_XML_ING
              || resState & RECV_SECTION_ING
              || resState & RECV_ERROR)
            {
                //downloading,timeout, kill timer and send download failed
                StateControllerDebug("wait 30s download failed still, send resource failed !!!!");
                curSingerResState = pkinfo->getCurPkPlayerDownloadState(curSingerIndex);
                StateControllerDebug("current index is:%d, download state:%d", curSingerIndex, curSingerResState);
                emit onStateSingChange(curSingerIndex, pkinfo->getLastSingEndRetcode(), -1, resState);
            }
        }

    }

    if(pkinfo->getMe().getIsHost() || curSingerIndex == pkinfo->getMeSectionIndex())
    {
        if((resState & RECV_XML_OK )
          && (resState & RECV_SECTION_OK)
          && (resState & RECV_MP3_OK))
        {
            StateControllerDebug("111 resouce download succeed");
            killTimer(downLoadResTimer);
            downLoadResTimer = 0;
            nodeNC->sendSingReady(0);
            curSingerIndex = pkinfo->getCurrentPkPlayerIndex();
            curSingerResState = pkinfo->getCurPkPlayerDownloadState(curSingerIndex);
            //download ok
            StateControllerDebug("current index is:%d, download state:%d", curSingerIndex, curSingerResState);
            if(pkinfo->getMe().getIsHost())
                 emit onStateSingChange(0, pkinfo->getLastSingEndRetcode(), 0, resState);
            else
                emit onStateSingChange(curSingerIndex, pkinfo->getLastSingEndRetcode(), 0, resState);
        }
    }
    else
    {
        if((resState & RECV_XML_OK )
          && (resState & RECV_SECTION_OK))
        {
            StateControllerDebug("111 resouce download succeed");
            killTimer(downLoadResTimer);
            downLoadResTimer = 0;
            nodeNC->sendSingReady(0);
            curSingerIndex = pkinfo->getCurrentPkPlayerIndex();
            curSingerResState = pkinfo->getCurPkPlayerDownloadState(curSingerIndex);
            //download ok
            StateControllerDebug("current index is:%d, download state:%d", curSingerIndex, curSingerResState);
            emit onStateSingChange(curSingerIndex, pkinfo->getLastSingEndRetcode(), 0, resState);
        }
    }

}


bool StateController::sendStateAcceptInvite()
{
    StateControllerDebug("StateController sendStateAcceptInvite");
    //bool isAccept = false;
    eventHandler->setCurEvent(EventHandler::AcceptInviteEvent);
    if(this->needProcessEvent(eventHandler->getCurEvent()))
    {
        this->resetResState();
        nodeNC->sendSwitchServer(NodeNotificationCenter::nodeServerType, NodeNotificationCenter::statusServerType);
        return true;
    }
    StateControllerDebug("StateController current event is:%d, current state is:%d, it's wrong, not handle!!!"
          ,eventHandler->getCurEvent(), getState());
    return false;
}

void StateController::sendStatePkInvite(Song song, Prop bidProp, QList<VictoryReward> vrList)
{
    StateControllerDebug("StateController sendStatePkInvite, host send pkinvite");
    if(LoginedNodeState != getState())
    {
        StateControllerDebug("host send pkinvite, current state is:%d, it's wrong, not to handle!!!",getState());
        return;
    }
    this->resetResState();
    nodeNC->sendPKInvite(song, bidProp, vrList);
    this->setNextState(BidState);
}


void StateController::sendStateBidStop()
{
     StateControllerDebug("StateController sendStateBidStop");
     if(BidState == getState())
         nodeNC->sendBidStop();
     else
         StateControllerDebug("StateController current state is:%d, it's wrong, not handle!!!" ,getState());
}

void StateController::sendStateBidConfirm(Prop prop, int retcode, QString orderNumber, QString signature)
{
    StateControllerDebug("StateController sendStateBidConfirm");
    nodeNC->sendBidConfirm(prop, retcode, orderNumber, signature);
}

void StateController::sendPkEnd()
{
    if(PkEndState == getState())
         this->setNextState(NoneState);
}

void StateController::sendStateSingEnd(int errorcode)
{
    nodeNC->sendSingEnd(errorcode);
}

void StateController::resetResState()
{
    StateControllerDebug("StateController resetResState, init resource state flag");
    resState &= 0x0;
    resState |= RECV_XML_ING;
    resState |= RECV_SECTION_ING;
    resState |= RECV_MP3_ING;
}



