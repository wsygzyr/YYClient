#include "HostBidController.h"
#include "ui_HostBidWidget.h"
#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
//#include "Modules/StateController/StateController.h"
HostBidController::HostBidController(QObject *parent) :
    QObject(parent)
{
    NodeNC = NodeNotificationCenter::getInstance();
    pkInfo = PkInfoManager::getInstance();
    resManager = ResManager::getInstance();
    stateCtrl = StateController::getInstance();
    connect(stateCtrl, SIGNAL(onStateBidInfo(int,Prop,Prop)), this, SLOT(handleStateControllerOnUpdateBidInfo(int,Prop ,Prop)));
}

void HostBidController::setHostBidWidget(HostBidWidget * widget)
{
    HostBidControllerDebug("HostBidController setHostBidWidget");
    bidWidget = widget;
    connect(bidWidget , SIGNAL(onHostStopBid()) , this ,SLOT(handleBidWidgetOnHostStopBid()));
    connect(bidWidget->ui->helpBtn , SIGNAL(clicked()) , this , SIGNAL(onHelpBtnClicked()));
    connect(bidWidget->ui->closeBtn , SIGNAL(clicked()) , this , SIGNAL(onCloseBtnClicked()));
    connect(bidWidget->ui->minBtn , SIGNAL(clicked()) , this , SIGNAL(onMinBtnClicked()));
}

void HostBidController::startBid(int bidCount)
{
    HostBidControllerDebug("HostBidController startBid");
    ConfigHelper *cfg = ConfigHelper::getInstance();

    resManager->reset();
    resManager->addDownLoadResRequest(pkInfo->getCurrentSong().getLyricFileName(),
                             pkInfo->getCurrentSong().getLyricUrl() ,
                             "");

    resManager->addDownLoadResRequest(pkInfo->getCurrentSong().getSectionFileName(),
                             pkInfo->getCurrentSong().getSectionUrl() ,
                             "");

    resManager->addDownLoadResRequest(pkInfo->getCurrentSong().getAccompanyFileName(),
                             pkInfo->getCurrentSong().getAccompanyUrl() ,
                             cfg->getPluginPath() +  pkInfo->getCurrentSong().getAccompanyFilePath());
    bidWidget->startBid(bidCount);
}

void HostBidController::handleStateControllerOnUpdateBidInfo(int biderNumber, Prop leastProp, Prop expectIncome)
{
    HostBidControllerDebug("HostBidController handleNodeNotificationCenterOnUpdateBidInfo");
    bidWidget->updateBidInfo(biderNumber , leastProp , expectIncome);

    P2Y_PKInfo *pKInfo = P2Y_PKInfo::getInstance();
        pKInfo->joinPKUserCount = biderNumber;
}

void HostBidController::handleBidWidgetOnHostStopBid()
{
    HostBidControllerDebug("HostBidController handleBidWidgetOnHostStopBid");
    //NodeNC->sendBidStop();
    stateCtrl->sendStateBidStop();
}
