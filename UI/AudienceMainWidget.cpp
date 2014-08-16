#include "AudienceMainWidget.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/ResManager/ResManager.h"
//#include "Modules/StateController/StateController.h"

AudienceMainWidget::AudienceMainWidget(QWidget *widget) : QStackedWidget(widget)
{
    this->setWindowIcon(QIcon("://image/AppIcon/icon_PK.ico"));

    yync = YYNotificationCenter::getInstance();
    NodeNC = NodeNotificationCenter::getInstance();
    info = PkInfoManager::getInstance();
    pKInfo = P2Y_PKInfo::getInstance();
    stateCtrl = StateController::getInstance();
    configUI();
    configSlots();
    bMouseLeftBtnDown = false;
    //    //test
    this->setCurrentWidget(pkRoomWidget);
    //    //test
}

AudienceMainWidget::~AudienceMainWidget()
{
    delete audienceErrorWidget;
    delete feedBackWidget;
    delete helpWidget;
    delete audienceInvitePKWidget;
}

void AudienceMainWidget::configUI()
{
    AudienceMainWidgetDebug("AudienceMainWidget configUI");
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setGeometry(FIRST_SHOW_GEOMETRY_X,FIRST_SHOW_GEOMETRY_X,GEOMETRY_WIDTH,GEOMETRY_HEIGHT);

    audienceInvitePKWidget = new AudienceInvitePKWidget();
    audienceInviteController = new AudienceInviteController();
    audienceInviteController->setAudienceInvitePKWidget(audienceInvitePKWidget);

    //    audienceInvitePKWidget->setWindowState(Qt::WindowStaysOnTopHint);

    //config AudienceBid
    audienceBidWidget = new AudienceBidWidget();
    this->addWidget(audienceBidWidget);

    //config bidResult
    bidResultWidget = new BidResultWidget();
    this->addWidget(bidResultWidget);
    bidResultController = new BidResultController(this);
    bidResultController->setBidResultWidget(bidResultWidget);


    bidResultFailedWidget = new BidResultFailedWidget();
    this->addWidget(bidResultFailedWidget);
    QObject::connect(bidResultFailedWidget, SIGNAL(onQuit()), this, SLOT(hide()));

    pkRoomWidget = new PKRoomWidget();
    this->addWidget(pkRoomWidget);
    pkRoomController = new PKRoomController(this);
    pkRoomController->setPkWidget(pkRoomWidget);

    pkResultWidget = new PKResultWidget();
    this->addWidget(pkResultWidget);
    pkResultController = new PKResultController(this);
    pkResultController->setPkResultWidget(pkResultWidget);
    connect(pkResultController,SIGNAL(onQuit()),this,SLOT(hide()));

    connect(pkResultController,SIGNAL(onCloseBtnClicked()),this,SLOT(handlePkResultWidgetClose()));
    connect(pkResultController,SIGNAL(onMinBtnClicked()),this,SLOT(handlePkResultWidgetMin()));

    audienceErrorWidget = new AudienceErrorWidget;
    this->setCurrentWidget(audienceBidWidget);
    audienceErrorWidget->hide();

    feedBackWidget   = new FeedBackWidget;
    feedBackWidget->hide();

    helpWidget       = new HelpWidget;
    helpWidget->hide();

    //    followHelper = new WindowFollowHelper(this);
    //    followHelper->setFollowWidget(this);
    //    followHelper->startFollow();

}
//config signal and slot
void AudienceMainWidget::configSlots()
{
    AudienceMainWidgetDebug("AudienceMainWidget configSlots");
    //yync slots
    QObject::connect(yync, SIGNAL(onMove(QRect)), this, SLOT(handleYYNotificationCenterOnMove(QRect)));
    QObject::connect(yync, SIGNAL(onShow(bool)), this, SLOT(handleYYNotificationCenterOnShow(bool)));
    QObject::connect(yync, SIGNAL(onQuit()), this, SLOT(handleYYNotificationCenterOnQuit()));
    QObject::connect(yync ,SIGNAL(onGetPluginInfo()) ,this ,SLOT(handleYYNotificationCenterOnGetPluginInfo()));
    QObject::connect(yync ,SIGNAL(onHostInfo(Player)) ,this ,SLOT(handleYYNotificationCenterOnHostInfo(Player)));
    QObject::connect(yync ,SIGNAL(onError(YYNotificationCenter::error)) ,this ,SLOT(handleYYNotificationCenterOnError(YYNotificationCenter::error)));

    // NodeNC slots
    //QObject::connect(stateCtrl, SIGNAL(onLoginResult(int)), this, SLOT(handleStateControllerOnStateLoginResult(int)));
    QObject::connect(stateCtrl, SIGNAL(onStatePKInvite(Player, Song, Prop, QList<VictoryReward>, int)), this, SLOT(handleStateControllerOnStatePKInvite(Player, Song, Prop, QList<VictoryReward>, int)));
    QObject::connect(stateCtrl, SIGNAL(onStateBidStart()), this, SLOT(handleStateControllerOnStateBidStart()));
    QObject::connect(NodeNC, SIGNAL(onBidEnd()), this, SLOT(handleNodeNotificationCenterOnBidEnd()));
    QObject::connect(stateCtrl, SIGNAL(onStateBidSettlement(int)), this, SLOT(handleStateControllerOnStateBidSettlement(int)));
    //QObject::connect(stateCtrl, SIGNAL(onStateSingReady(QList<SingReadyResult>)), this, SLOT(handleStateControllerOnStateSingReady(QList<SingReadyResult>)));
    QObject::connect(stateCtrl, SIGNAL(onStatePKStart(QList<Player>)), this, SLOT(handleStateControllerOnStatePKStart(QList<Player>)));
    QObject::connect(stateCtrl, SIGNAL(onStatePKResult(QList<Score>, int , Prop, QList<int>)), this, SLOT(handleStateControllerOnStatePKResult(QList<Score>, int , Prop, QList<int>)));
    QObject::connect(NodeNC, SIGNAL(onError(NodeNotificationCenter::error)), this, SLOT(handleNodeNotificationCenterOnError(NodeNotificationCenter::error)));
    QObject::connect(stateCtrl ,SIGNAL(onStateErrorEvent(int)) , this , SLOT(handleStateControllerOnStateErrorEvent(int)));
    QObject::connect(stateCtrl, SIGNAL(onStateBidFailed()), this, SLOT(handleStateControllerOnStateBidFailed()));
    QObject::connect(NodeNC, SIGNAL(onReconnNotify(int)), this, SLOT(handleNodeNotificationCenterOnReconnNotify(int)));

    QObject::connect(audienceErrorWidget , SIGNAL(onQuit()) , this, SLOT(handleAudienceErrorWidgetOnQuit()));
    QObject::connect(pkResultController , SIGNAL(onAdvice()) , this ,SLOT(handlePkResultControllerOnAdvice()));
    QObject::connect(audienceBidWidget , SIGNAL(onHelpBtnClicked())  , this , SLOT(handleAudienceBidWidgetOnHelpBtnClicked()));
    QObject::connect(audienceInvitePKWidget, SIGNAL(onAcceptInviteEvent()), this, SLOT(handleAudienceInvitePKWidgetOnAcceptInviteEvent()));
    QObject::connect(audienceInviteController , SIGNAL(onCloseBtnClicked())  , this , SLOT(handleAudienceInviteWidgetClose()));
    QObject::connect(audienceBidWidget , SIGNAL(onCloseBtnClicked()), this, SLOT(handleAudienceBidWidgetClose()));
    QObject::connect(audienceBidWidget , SIGNAL(onMinBtnClicked()), this, SLOT(handleAudienceBidWidgetMin()));

    connect(bidResultController , SIGNAL(onCloseBtnClicked()) , this, SLOT(handleBidResultWidgetClose()));
    connect(bidResultController , SIGNAL(onMinBtnClicked()) , this, SLOT(handleBidResultWidgetMin()));

}

void AudienceMainWidget::handleYYNotificationCenterOnShow(bool bShow)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleYYNotificationCenterOnShow");
    if (bShow)
    {
        this->show();
    }
    else
    {
        this->hide();
    }
}

void AudienceMainWidget::handleYYNotificationCenterOnMove(QRect rect)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleYYNotificationCenterOnMove");
    AudienceMainWidgetDebug("receive move,rect(%d,%d,%d,%d)",rect.x(),rect.y(),rect.width(),rect.height());
    if(this->currentWidget() != audienceInvitePKWidget)
    {
        this->setGeometry(rect);
    }
}

void AudienceMainWidget::handleYYNotificationCenterOnQuit()
{
    AudienceMainWidgetDebug("AudienceMainWidget handleYYNotificationCenterOnQuit");
    this->close();
}

void AudienceMainWidget::handleYYNotificationCenterOnGetPluginInfo()
{
    AudienceMainWidgetDebug("AudienceMainWidget handleYYNotificationCenterOnGetPluginInfo");

    yync->sendPluginInfo("iflytek","iHouPlugin","1.0");
}

void AudienceMainWidget::handleYYNotificationCenterOnError(YYNotificationCenter::error)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleYYNotificationCenterOnError");

    this->close();
}

void AudienceMainWidget::handleYYNotificationCenterOnHostInfo(Player player)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleYYNotificationCenterOnHostInfo");
    if (player.getIsHost())
    {
        ConfigHelper *cfg = ConfigHelper::getInstance();
        NodeNC->start(cfg->getNodeServerIP(),cfg->getNodeServerPort(),
                      cfg->getStatusServerIP(),cfg->getStatusServerPort(),
                      info->getChannelID(),info->getMe());
    }
    else
    {
        if (!audienceInvitePKWidget->isHidden() || !this->isHidden())
        {
            audienceErrorWidget->show();
            audienceErrorWidget->setGeometry(this->x() + 125 , this->y() + 29 , audienceErrorWidget->width() , audienceErrorWidget->height());
        }
        NodeNC->stop();
    }
}

void AudienceMainWidget::handleNodeNotificationCenterOnError(NodeNotificationCenter::error)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleNodeNotificationCenterOnError");
    AudienceMainWidgetDebug("NodeNC disconnected!!!");
    NodeNC->stop();
    this->hide();
}

void AudienceMainWidget::handleNodeNotificationCenterOnReconnNotify(int cnt)
{
    AudienceMainWidgetDebug("Audience reconnect %d times!!!",cnt);
}

void AudienceMainWidget::handleStateControllerOnStateLoginResult(int retCode)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleNodeNotificationCenterOnLogin");
    switch(retCode)
    {
    case 0:
        AudienceMainWidgetDebug("Link to NodeServer success...");
        break;
    default:
        AudienceMainWidgetDebug("Link to NodeServer failed...");
        break;
    }
}


void AudienceMainWidget::handleStateControllerOnStatePKInvite(Player host,
                                                              Song song,
                                                              Prop bidProp,
                                                              QList<VictoryReward> victoryRewardList,
                                                              int acceptedPlayers)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleStateControllerOnPKInvite");
    AudienceMainWidgetDebug("go into handleNodeNotificationCenterOnInvitePK");
    PkInfoManager *info = PkInfoManager::getInstance();
    ConfigHelper  *cfg = ConfigHelper::getInstance();
    info->setHostPlayer(host);
    info->setCurrentSong(song);
    info->setLeastBidProp(bidProp);
    for(int i = 0 ; i < victoryRewardList.size(); i++)
    {
        for(int j = 0 ; j < cfg->getVictoryRewardList().size() ; j++)
        {
            if(victoryRewardList[i].getID() == cfg->getVictoryRewardList().at(j).getID())
            {
                if(victoryRewardList[i].getID() != 100 && victoryRewardList[i].getID() != 101)
                {
                    victoryRewardList[i].setName(cfg->getVictoryRewardList().at(i).getName());
                    victoryRewardList[i].setDesp(cfg->getVictoryRewardList().at(i).getDesp());
                }
            }
        }
    }
    info->setVictoryRewardList(victoryRewardList);

    audienceInvitePKWidget->setUIControlsInfo( song, acceptedPlayers);
    audienceInvitePKWidget->show();
    //this->setGeometry(FIRST_SHOW_GEOMETRY_X,FIRST_SHOW_GEOMETRY_Y,AUDIENCE_INVITE_WIDGET_WIDTH,AUDIENCE_INVITE_WIDGET_HEIGHT);
    this->hide();
    audienceErrorWidget->hide();
}

void AudienceMainWidget::handleStateControllerOnStateBidStart()
{
    AudienceMainWidgetDebug("AudienceMainWidget handleStateControllerOnBidStart");
    AudienceMainWidgetDebug("go into audienceBidWidget...");
    this->show();
    move(this->audienceInvitePKWidget->x() + this->audienceInvitePKWidget->width()/2 - this->audienceBidWidget->width()/2,
         this->audienceInvitePKWidget->y() + this->audienceInvitePKWidget->height()/2 - this->audienceBidWidget->height()/2);
    this->setCurrentWidget(audienceBidWidget);
    audienceBidWidget->startBid();
}

void AudienceMainWidget::handleNodeNotificationCenterOnBidEnd()
{
    //First we show the bidWidget to the user ,use audienceBidWidget->startBid() to initialize
    //the data in the widget , then we change the widget to the bidEnd state using endBid().

    AudienceMainWidgetDebug("AudienceMainWidget handleNodeNotificationCenterOnBidEnd");
    AudienceMainWidgetDebug("go into audienceBidWidget...");
    this->show();
    move(this->audienceInvitePKWidget->x() + this->audienceInvitePKWidget->width()/2 - this->audienceBidWidget->width()/2,
         this->audienceInvitePKWidget->y() + this->audienceInvitePKWidget->height()/2 - this->audienceBidWidget->height()/2);
    this->setCurrentWidget(audienceBidWidget);
    audienceBidWidget->startBid();

    audienceBidWidget->endBid();

}


void AudienceMainWidget::handleNodeNotificationCenterOnBidResult(QList<Player> playerList, QList<Prop> propList)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleNodeNotificationCenterOnBidResult");
    AudienceMainWidgetDebug("go into BidResult...");
    if (propList.size() == 0)
    {
        this->setCurrentWidget(bidResultFailedWidget);

        ConfigHelper *cfg = ConfigHelper::getInstance();
        NodeNC->stop();
        NodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
                      cfg->getStatusServerIP(), cfg->getStatusServerPort(),
                      info->getChannelID(), info->getMe());
    }
    else
    {
        this->setCurrentWidget(bidResultWidget);
        bidResultWidget->setBidResultWidgetInfo();

    }
}


void AudienceMainWidget::handleStateControllerOnStatePKStart(QList<Player> playerList)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleStateControllerOnStatePKStart");
    AudienceMainWidgetDebug("PKStart...");
    bidResultWidget->setProgressBarValue(100);
    bidResultWidget->killTimeID();

    this->setCurrentWidget(pkRoomWidget);

    //pkRoomController->handleMainWidgetOnStartPK(playerList);
    this->show();
}

void AudienceMainWidget::handleStateControllerOnStatePKResult(QList<Score> scoreList, int heat, Prop propIncome, QList<int> lastRetcodeList)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleStateControllerOnStatePKResult");
    AudienceMainWidgetDebug("PKResult...");
    this->setCurrentWidget(pkResultWidget);
}

void AudienceMainWidget::handleStateControllerOnStateErrorEvent(int errCode)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleStateControllerOnErrorEvent");
    //    if (!this->isHidden() || !audienceInvitePKWidget->isHidden())
    //    {
    //        audienceErrorWidget->show();
    //        audienceInvitePKWidget->hide();
    //        if(!this->isHidden())
    //            audienceErrorWidget->setGeometry(this->x() + 125 , this->y() + 29 , audienceErrorWidget->width() , audienceErrorWidget->height());
    //        else
    //            audienceErrorWidget->setGeometry(audienceInvitePKWidget->x()  , audienceInvitePKWidget->y()  , audienceErrorWidget->width() , audienceErrorWidget->height());
    //    }

    PkInfoManager *info = PkInfoManager::getInstance();
    if(!info->getMe().getIsHost())
    {
        NodeNC->stop();
        ConfigHelper *cfg = ConfigHelper::getInstance();
        NodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
                      cfg->getStatusServerIP(), cfg->getStatusServerPort(),
                      info->getChannelID(), info->getMe());
    }
}

void AudienceMainWidget::handleStateControllerOnStateBidFailed()
{
    this->setCurrentWidget(bidResultFailedWidget);
}


void AudienceMainWidget::handleStateControllerOnStateBidSettlement(int complete)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleStateControllerOnStateBidSettlement");
    AudienceMainWidgetDebug("It has compeleted user number is:%d", complete);
    this->setCurrentWidget(bidResultWidget);
    if (0 == complete)
    {
        bidResultWidget->setBidResultWidgetInfo();
    }
    this->show();
}

void AudienceMainWidget::handleStateControllerOnStateSingReady(QList<SingReadyResult> SingReadyList)
{
    AudienceMainWidgetDebug("AudienceMainWidget handleStateControllerOnStateSingReady");
}

void AudienceMainWidget::handleAudienceErrorWidgetOnQuit()
{
    AudienceMainWidgetDebug("AudienceMainWidget handleAudienceErrorWidgetOnQuit");
    audienceErrorWidget->hide();
    audienceInvitePKWidget->hide();
    this->hide();
}

void AudienceMainWidget::handlePkResultControllerOnAdvice()
{
    AudienceMainWidgetDebug("AudienceMainWidget handlePkResultControllerOnAdvice");
    feedBackWidget->show();
    feedBackWidget->setGeometry(this->x() + 125 , this->y() + 29 , feedBackWidget->width() , feedBackWidget->height());
    feedBackWidget->setFocus();
}

void AudienceMainWidget::handleAudienceBidWidgetOnHelpBtnClicked()
{
    AudienceMainWidgetDebug("AudienceMainWidget handleAudienceBidWidgetOnHelpBtnClicked");
    helpWidget->show();
    helpWidget->setGeometry(this->x() + 125 , this->y() + 29 , helpWidget->width() , helpWidget->height());
}

void AudienceMainWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(event->y()>30||event->x()>601)
        {
            event->ignore();
            return;
        }
        this->windowPos = this->pos();
        this->mousePos = event->globalPos();
        this->dPos = mousePos - windowPos;
        this->bMouseLeftBtnDown = true;
    }
    event->ignore();
}

void AudienceMainWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (bMouseLeftBtnDown)
    {
        this->move(event->globalPos() - this->dPos);
    }
}

void AudienceMainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        bMouseLeftBtnDown = false;
    }
    event->ignore();
}

void AudienceMainWidget::handleAudienceInvitePKWidgetOnAcceptInviteEvent()
{
    AudienceMainWidgetDebug("receive accept Pkinvite event");
    stateCtrl->sendStateAcceptInvite();
}
void AudienceMainWidget::handleAudienceInviteWidgetClose()
{
    AudienceMainWidgetDebug("AudienceMainWidget handleAudienceInviteWidgetClose");
    audienceInvitePKWidget->hide();
    this->hide();
    ConfigHelper *cfg = ConfigHelper::getInstance();
    NodeNC->stop();
    NodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
                  cfg->getStatusServerIP(), cfg->getStatusServerPort(),
                  info->getChannelID(), info->getMe());
}
void AudienceMainWidget::handleAudienceBidWidgetClose()
{
    AudienceMainWidgetDebug("AudienceMainWidget handleAudienceBidWidgetClose");
    audienceBidWidget->hide();
    this->hide();
    ConfigHelper *cfg = ConfigHelper::getInstance();
    NodeNC->stop();
    NodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
                  cfg->getStatusServerIP(), cfg->getStatusServerPort(),
                  info->getChannelID(), info->getMe());
}

void AudienceMainWidget::handleAudienceBidWidgetMin()
{
    AudienceMainWidgetDebug("AudienceMainWidget handleAudienceBidWidgetMin");
    this->showMinimized();
}


void AudienceMainWidget::handleBidResultWidgetClose()
{
    AudienceMainWidgetDebug("AudienceMainWidget handleBidResultWidgetClose");
    bidResultWidget->hide();
    this->hide();
    ConfigHelper *cfg = ConfigHelper::getInstance();
    NodeNC->stop();
    NodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
                  cfg->getStatusServerIP(), cfg->getStatusServerPort(),
                  info->getChannelID(), info->getMe());
}

void AudienceMainWidget::handleBidResultWidgetMin()
{
    AudienceMainWidgetDebug("AudienceMainWidget handleBidResultWidgetMin");
    this->showMinimized();
}


void AudienceMainWidget::handlePkResultWidgetClose()
{
    AudienceMainWidgetDebug("AudienceMainWidget handlePkResultWidgetClose");
    pkResultWidget->hide();
    this->hide();
    stateCtrl->sendPkEnd();
    ConfigHelper *cfg = ConfigHelper::getInstance();
    NodeNC->stop();
    NodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
                  cfg->getStatusServerIP(), cfg->getStatusServerPort(),
                  info->getChannelID(), info->getMe());
}

void AudienceMainWidget::handlePkResultWidgetMin()
{
    AudienceMainWidgetDebug("AudienceMainWidget handlePkResultWidgetMin");
    this->showMinimized();
}

bool AudienceMainWidget::winEvent(MSG *message, long *result)
{
    if(message->message == WM_SYSCOMMAND && message->wParam == SC_CLOSE)
    {
        AudienceMainWidgetDebug("winEvent destory downloading widget");
        pkRoomController->DestoryDownLoadingWidget();

        this->hide();
        ConfigHelper *cfg = ConfigHelper::getInstance();
        NodeNC->stop();
        NodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
                      cfg->getStatusServerIP(), cfg->getStatusServerPort(),
                      info->getChannelID(), info->getMe());
        return true;
    }
    return false;
}
