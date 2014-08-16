#include "HostMainWidget.h"

enum PAGE_INDEX
{
    HOME_PAGE = 0,
    INVITE_PAGE ,
    BID_PAGE,
    BID_RESULT_PAGE,
    BID_RESULT_FAILED_PAGE,
    PK_ROOM_PAGE,
    PK_RESULT_PAGE,
    SONG_SELECT_PAGE
};

HostMainWidget::HostMainWidget(QWidget *parent):QStackedWidget(parent)
{
    //    QDesktopServices::openUrl(QUrl(QString("http://blog.const.net.cn")));
    this->setWindowIcon(QIcon("://image/AppIcon/icon_PK.ico"));

    yync = YYNotificationCenter::getInstance();
    NodeNC = NodeNotificationCenter::getInstance();
    stateCtrl = StateController::getInstance();

    followHelper = NULL;

    configMainPage();
    configHomePage();
    configInvitePage();
    configBidPage();
    configBidResultPage();
    configBidResultFailedPage();
    configPkPage();
    configPkResultPage();
    configFeedbackPage();
    configHelpPage();

    configSongSelectPage();

    //configFollowHelper();

    bMouseLeftBtnDown = false;
    this->setCurrentIndex(HOME_PAGE);
}

HostMainWidget::~HostMainWidget()
{
    helpWidget->deleteLater();
    feedBackWidget->deleteLater();
}

void HostMainWidget::configMainPage()
{
    HostMainWidgetDebug("HostMainWidget configMainPage");
    info = PkInfoManager::getInstance();
    pKInfo = P2Y_PKInfo::getInstance();

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint);

    // YYNodinotifcation SIGNALS
    QObject::connect(yync ,SIGNAL(onMove(QRect)) ,this ,SLOT(handleYYNotificationCenterOnMove(QRect)));
    QObject::connect(yync ,SIGNAL(onShow(bool)) ,this ,SLOT(handleYYNotificationCenterOnShow(bool)));
    QObject::connect(yync ,SIGNAL(onQuit()) ,this ,SLOT(handleYYNotificationCenterOnQuit()));
    QObject::connect(yync ,SIGNAL(onGetPluginInfo()) ,this ,SLOT(handleYYNotificationCenterOnGetPluginInfo()));
    QObject::connect(yync ,SIGNAL(onError(YYNotificationCenter::error)) ,this ,SLOT(handleYYNotificationCenterOnError(YYNotificationCenter::error)));
    QObject::connect(yync ,SIGNAL(onHostInfo(Player)),this, SLOT(handleYYNotificationCenterOnHostInfo(Player)));

    // NodeNotificationCenter SIGNALS
    QObject::connect(stateCtrl, SIGNAL(onStateLoginResult(int)), this, SLOT(handleStateControllerOnStateLoginResult(int)));
    QObject::connect(stateCtrl ,SIGNAL(onStateBidStart()),this,SLOT(handleStateControllerOnStateBidStart()));
    QObject::connect(stateCtrl, SIGNAL(onStatePKStart(QList<Player>)) ,this ,SLOT(handleStateControllerOnStatePKStart(QList<Player>)));
    QObject::connect(stateCtrl, SIGNAL(onStateBidSettlement(int)), this, SLOT(handleStateControllerOnStateBidSettlement(int)));
    QObject::connect(stateCtrl, SIGNAL(onStateBidFailed()), this, SLOT(handleStateControllerOnStateBidFailed()));
    //QObject::connect(stateCtrl, SIGNAL(OnStateSingReady(QList<SingReadyResult>)), this, SLOT(handleStateControllerOnStateSingReady(QList<SingReadyResult>)));
    QObject::connect(stateCtrl ,SIGNAL(onStatePKResult(QList<Score>, int, Prop, QList<int>)) ,this, SLOT(handleStateControllerOnStatePKResult(QList<Score>, int, Prop, QList<int>)));
    QObject::connect(NodeNC ,SIGNAL(onError(NodeNotificationCenter::error)) ,this, SLOT(handleNodeNotificationCenterOnError(NodeNotificationCenter::error)));
    QObject::connect(NodeNC, SIGNAL(onReconnNotify(int)), this, SLOT(handleNodeNotificationCenterOnReconnNotify(int)));

}

void HostMainWidget::configHomePage()
{
    HostMainWidgetDebug("HostMainWidget configHomePage");
    homePage = new HostHomeWidget();
    this->addWidget(homePage);
    homeController = new HostHomeController(this);
    homeController->setHomeWidget(homePage);

    connect(homeController ,SIGNAL(startInvite()) , this , SLOT(handleHomeControllerOnStartInvite()));
    connect(homeController , SIGNAL(onAdvice()) , this , SLOT(handleHomeControllerOnAdvice()));
    connect(homeController , SIGNAL(onHelpBtnClicked()) , this ,SLOT(handleHomeControllerOnHelpBtnClicked()));
    connect(homeController , SIGNAL(onCloseBtnClicked()) , this ,SLOT(handleYYNotificationCenterOnQuit()));
    connect(homeController , SIGNAL(onMinBtnClicked()) , this ,SLOT(handleHostOnMinBtnClicked()));
}

void HostMainWidget::configInvitePage()
{
    HostMainWidgetDebug("HostMainWidget configInvitePage");
    this->setGeometry(350,450,594,266);
    invitePage = new HostInviteWidget();
    this->addWidget(invitePage);
    inviteController = new HostInviteController(this);
    inviteController->setInviteWidget(invitePage);

    connect(inviteController , SIGNAL(onHostSendPkInvite(int)) , this , SLOT(handleHostInviteControllerOnHostSendPkInvite(int)));
    connect(inviteController , SIGNAL(onReturnToHomePage()) , this , SLOT(handleHostInviteControllerOnReturnToHomePage()));
    connect(inviteController , SIGNAL(onHelpBtnClicked()) , this , SLOT(handleHostInviteControllerOnHelpBtnClicked()));
    connect(inviteController , SIGNAL(onShowSongSelect()) , this , SLOT(handleHomeControllerOnShowSongSelect()));
    connect(inviteController , SIGNAL(onCloseBtnClicked()) , this ,SLOT(handleYYNotificationCenterOnQuit()));
    connect(inviteController , SIGNAL(onMinBtnClicked()) , this ,SLOT(handleHostOnMinBtnClicked()));
}

void HostMainWidget::configBidPage()
{
    HostMainWidgetDebug("HostMainWidget configBidPage");
    bidPage = new HostBidWidget();
    this->addWidget(bidPage);
    bidController = new HostBidController(this);
    bidController->setHostBidWidget(bidPage);

    connect(bidController  , SIGNAL(onHelpBtnClicked()) , this , SLOT(handleHostBidControllerOnHelpBtnClicked()));
    connect(bidController , SIGNAL(onCloseBtnClicked()) , this ,SLOT(handleYYNotificationCenterOnQuit()));
    connect(bidController , SIGNAL(onMinBtnClicked()) , this ,SLOT(handleHostOnMinBtnClicked()));
}

void HostMainWidget::configBidResultPage()
{
    HostMainWidgetDebug("HostMainWidget configBidResultPage");
    bidResulsPage = new BidResultWidget();
    this->addWidget(bidResulsPage);
    bidResultController = new BidResultController(this);
    bidResultController->setBidResultWidget(bidResulsPage);

    connect(bidResultController , SIGNAL(onCloseBtnClicked()) , this ,SLOT(handleYYNotificationCenterOnQuit()));
    connect(bidResultController , SIGNAL(onMinBtnClicked()) , this ,SLOT(handleHostOnMinBtnClicked()));
}

void HostMainWidget::configBidResultFailedPage()
{
    HostMainWidgetDebug("HostMainWidget configBidResultFailedPage");
    bidResultFailedPage = new BidResultFailedWidget();
    this->addWidget(bidResultFailedPage);
    QObject::connect(bidResultFailedPage, SIGNAL(onQuit()), this, SLOT(handleBidResultFailedPageOnQuit()));
}

void HostMainWidget::configPkPage()
{
    HostMainWidgetDebug("HostMainWidget configPkPage");
    PkPage = new PKRoomWidget();
    this->addWidget(PkPage);
    PkController = new PKRoomController(this);
    PkController->setPkWidget(PkPage);
    connect(PkController , SIGNAL(onQuit()) , this , SLOT(close()));
}

void HostMainWidget::configPkResultPage()
{
    HostMainWidgetDebug("HostMainWidget configPkResultPage");
    pkResultPage = new PKResultWidget();
    this->addWidget(pkResultPage);
    pkResultController = new PKResultController(this);
    pkResultController->setPkResultWidget(pkResultPage);

    connect(pkResultController,SIGNAL(onQuit()),this,SLOT(handlePkResultPageOnQuit()));
    connect(pkResultController , SIGNAL(onAdvice()) , this , SLOT(handlePkResultControllerOnAdvice()));
    connect(pkResultController,SIGNAL(onCloseBtnClicked()),this,SLOT(handleYYNotificationCenterOnQuit()));
    connect(pkResultController , SIGNAL(onMinBtnClicked()) , this , SLOT(handleHostOnMinBtnClicked()));
}

void HostMainWidget::configFeedbackPage()
{
    HostMainWidgetDebug("HostMainWidget configFeedbackPage");
    feedBackWidget = new FeedBackWidget;
    feedBackWidget->hide();
}

void HostMainWidget::configHelpPage()
{
    HostMainWidgetDebug("HostMainWidget configHelpPage");
    helpWidget = new HelpWidget;
    helpWidget->setFocus();
    helpWidget->hide();
}

void HostMainWidget::configSongSelectPage()
{
    HostMainWidgetDebug("HostMainWidget configSongSelectPage");
    songMenuWidget = new SongMenuWidget;
    this->addWidget(songMenuWidget);
    songMenuWidget->hide();

    connect(songMenuWidget, SIGNAL(onQuit()), this, SLOT(handleSongSelectPageOnQuit()));
    connect(songMenuWidget, SIGNAL(onSelectedSong(Song*)), inviteController, SLOT(onSongSelectWidgetOnSelectedSong(Song*)));
}

void HostMainWidget::configFollowHelper()
{
    HostMainWidgetDebug("HostMainWidget configFollowHelper");
    followHelper = new WindowFollowHelper(this);
    followHelper->setFollowWidget(this);
    followHelper->startFollow();
}


void HostMainWidget::handleYYNotificationCenterOnMove(QRect rect)
{
    HostMainWidgetDebug("HostMainWidget handleYYNotificationCenterOnMove");
    this->setGeometry(rect);
}

void HostMainWidget::handleStateControllerOnStatePKStart(QList<Player> playerList )
{
    HostMainWidgetDebug("HostMainWidget handleStateControllerOnStatePKStart");
    this->bidResulsPage->setProgressBarValue(100);
    this->bidResulsPage->killTimeID();

    this->setCurrentIndex(PK_ROOM_PAGE);
}

void HostMainWidget::handleStateControllerOnStatePKResult(QList<Score> scoreList, int heat, Prop propIncome, QList<int> lastRetcodeList)
{
    HostMainWidgetDebug("HostMainWidget handleStateControllerOnStatePKResult");
    this->setCurrentIndex(PK_RESULT_PAGE);
}

void HostMainWidget::handleStateControllerOnStateBidStart()
{
    HostMainWidgetDebug("HostMainWidget handleStateControllerOnStateBidStart");

}

void HostMainWidget::handleYYNotificationCenterOnShow(bool isShow)
{
    HostMainWidgetDebug("HostMainWidget handleYYNotificationCenterOnShow");
    if(isShow)
    {
        this->show();
    }
    else
    {
        this->hide();
    }
}

void HostMainWidget::handleYYNotificationCenterOnQuit()
{
    HostMainWidgetDebug("HostMainWidget handleYYNotificationCenterOnQuit");
    this->close();
}

void HostMainWidget::handleHostOnMinBtnClicked()
{
    HostMainWidgetDebug("HostMainWidget handleHomeControllerOnMinBtnClicked");
    this->showMinimized();
}

void HostMainWidget::handleHostInviteWidgetOnMinBtnClicked()
{
    invitePage->showMinimized();
}

void HostMainWidget::handleYYNotificationCenterOnGetPluginInfo()
{
    HostMainWidgetDebug("HostMainWidget handleYYNotificationCenterOnGetPluginInfo");
    yync->sendPluginInfo("iflytek","iHouPlugin","1.0");
}

void HostMainWidget::handleYYNotificationCenterOnHostInfo(Player player)
{
    HostMainWidgetDebug("HostMainWidget handleYYNotificationCenterOnHostInfo");
    if (!player.getIsHost())
    {
        this->close();
    }
}

void HostMainWidget::handleYYNotificationCenterOnError(YYNotificationCenter::error)
{
    HostMainWidgetDebug("HostMainWidget handleYYNotificationCenterOnError");
    this->close();
}

void HostMainWidget::handleNodeNotificationCenterOnReconnNotify(int cnt)
{
    HostMainWidgetDebug("host reconnect %d times!",cnt);
}
void HostMainWidget::handleNodeNotificationCenterOnError(NodeNotificationCenter::error)
{
    HostMainWidgetDebug("HostMainWidget handleNodeNotificationCenterOnError");
    this->close();
}

void HostMainWidget::handleStateControllerOnStateLoginResult(int retCode)
{
    HostMainWidgetDebug("HostMainWidget handleNodeNotificationCenterOnLogin");
    switch(retCode)
    {case 0:
        HostMainWidgetDebug("Login to IhouServer succeed!");
        break;
    default:
        HostMainWidgetDebug("Login to IhouServer failed!");
        break;
    }
    HostMainWidgetDebug("show home page");
    //change by zhaoli 2014.8.15
    homePage->stopLoading();
//    this->show();
//    this->setCurrentIndex(HOME_PAGE);
}

void HostMainWidget::handleStateControllerOnStateBidFailed()
{
    this->bidResulsPage->killTimeID();
    this->setCurrentIndex(BID_RESULT_FAILED_PAGE);
}

void HostMainWidget::handleHostInviteControllerOnHostSendPkInvite(int bidCount)
{
    HostMainWidgetDebug("HostMainWidget handleHostInviteControllerOnHostSendPkInvite");
    HostMainWidgetDebug("change to bid page!");
    this->setCurrentIndex(BID_PAGE);
    bidController->startBid(bidCount);
}

void HostMainWidget::handleHostInviteControllerOnReturnToHomePage()
{
    HostMainWidgetDebug("HostMainWidget handleHostInviteControllerOnReturnToHomePage");
    HostMainWidgetDebug("return to home page!");
    this->setCurrentIndex(HOME_PAGE);
}

void HostMainWidget::handleHostInviteControllerOnHelpBtnClicked()
{
    HostMainWidgetDebug("HostMainWidget handleHostInviteControllerOnHelpBtnClicked");
    helpWidget->show();
    helpWidget->setGeometry(this->x() + 125 , this->y() + 29 , helpWidget->width() , helpWidget->height());
}

void HostMainWidget::handleHomeControllerOnShowSongSelect()
{
    HostMainWidgetDebug("HostMainWidget handleHomeControllerOnShowSongSelect");
    songMenuWidget->show();
    this->setCurrentIndex(SONG_SELECT_PAGE);
}

void HostMainWidget::handleHomeControllerOnStartInvite()
{
    HostMainWidgetDebug("HostMainWidget handleHomeControllerOnStartInvite");
    pKInfo->pKStartTime.setDate(QDate::currentDate());
    pKInfo->pKStartTime.setTime(QTime::currentTime());

    this->setCurrentIndex(INVITE_PAGE);
    inviteController->startInvite();
}

void HostMainWidget::handleHomeControllerOnAdvice()
{
    HostMainWidgetDebug("HostMainWidget handleHomeControllerOnAdvice");
    feedBackWidget->show();
    feedBackWidget->setGeometry(this->x() + 125 , this->y() + 29 , feedBackWidget->width() , feedBackWidget->height());
    feedBackWidget->setFocus();
}

void HostMainWidget::handleHomeControllerOnHelpBtnClicked()
{
    HostMainWidgetDebug("HostMainWidget handleHomeControllerOnHelpBtnClicked");
    handleHostInviteControllerOnHelpBtnClicked();
}

void HostMainWidget::handleBidResultFailedPageOnQuit()
{
    HostMainWidgetDebug("HostMainWidget handleBidResultFailedPageOnQuit");
    pKInfo->startPKSuccessCount = 0;
    yync->sendHostPKInfo(pKInfo);

    this->close();
}

void HostMainWidget::handlePkResultControllerOnAdvice()
{
    HostMainWidgetDebug("HostMainWidget handlePkResultControllerOnAdvice");
    handleHomeControllerOnAdvice();
}

void HostMainWidget::handleHostBidControllerOnHelpBtnClicked()
{
    HostMainWidgetDebug("HostMainWidget handleHostBidControllerOnHelpBtnClicked");
    handleHostInviteControllerOnHelpBtnClicked();
}

void HostMainWidget::handleStateControllerOnStateBidSettlement(int complete)
{
    HostMainWidgetDebug("HostMainWidget handleStateControllerOnRoomInfoSettlement");
    HostMainWidgetDebug("It has compeleted user number is:%d", complete);
    this->setCurrentIndex(BID_RESULT_PAGE);
    if (0 == complete)
    {
        bidResulsPage->setBidResultWidgetInfo();
    }
}

void HostMainWidget::handleStateControllerOnStateSingReady(QList<SingReadyResult>)
{
    HostMainWidgetDebug("HostMainWidget handleStateControllerOnStateSingReady");
}

void HostMainWidget::handlePkResultPageOnQuit()
{
    HostMainWidgetDebug("HostMainWidget handlePkResultPageOnQuit");
    pKInfo->startPKSuccessCount = 1;
    yync->sendHostPKInfo(pKInfo);

    this->close();
}

void HostMainWidget::handleSongSelectPageOnQuit()
{
    HostMainWidgetDebug("HostMainWidget handleSongSelectPageOnQuit");
    songMenuWidget->hide();
    this->setCurrentIndex(INVITE_PAGE);
}

void HostMainWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(event->y()>30||event->x()>601)
        {
            event->ignore();
            return;
        }
        if(followHelper)
            this->windowPos = followHelper->getSelfPos() - followHelper->getYYParentPos() ;
        else
            this->windowPos = this->pos();
        this->mousePos = event->globalPos();
        this->dPos = mousePos - windowPos;
        this->bMouseLeftBtnDown = true;
        //HostMainWidgetDebug("this.x %d , this.y %d , mouse.x %d , mouse.y %d",this->windowPos.x() , this->windowPos.y() ,mousePos.x() , mousePos.y());
    }
    event->ignore();
}

void HostMainWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (bMouseLeftBtnDown)
    {
        this->move(event->globalPos() - this->dPos);
        //HostMainWidgetDebug("this.x %d , this.y %d , mouse.x %d , mouse.y %d",this->x()- followHelper->getYYParentPos().x() , this->y()- followHelper->getYYParentPos().y() ,event->globalX(), event->globalY());
    }
}

void HostMainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        bMouseLeftBtnDown = false;
    }
    event->ignore();
}

void HostMainWidget::showHostMainWidget()
{
    HostMainWidgetDebug("HostMainWidget show");
    this->setCurrentIndex(HOME_PAGE);
    this->show();
    homePage->startLoading();
}


