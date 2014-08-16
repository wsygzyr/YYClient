#include "HostHomeController.h"
#include "ui_HostHomeWidget.h"
#include "PkInfoManager/PkInfoManager.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include <QDesktopServices>

HostHomeController::HostHomeController(QObject *parent) :
    QObject(parent)
{
    ConfigHelper *cfg = ConfigHelper::getInstance();
    PkInfoManager* info = PkInfoManager::getInstance();

    httpGetMainPageInfo = new HttpGet();
    HttpGetInfo httpGetInfo;
    httpGetInfo.setType(HttpGetInfo::InfoType::MAIN_PAGE_INFO);
    httpGetInfo.setUrl(QUrl(cfg->getMainPagePrefix() + QString("%1").arg(info->getHostPlayer().getID())));

    httpGetMainPageInfo->request(httpGetInfo);
    connect(httpGetMainPageInfo, SIGNAL(onMainPageInfo(bool, MainPageInfo)), this, SLOT(handleHttpGetOnMainPageInfo(bool, MainPageInfo)));
}

HostHomeController::~HostHomeController()
{
    delete httpGetMainPageInfo;
    httpGetMainPageInfo = NULL;
}

void HostHomeController::setHomeWidget(HostHomeWidget *widget)
{
    HostHomeControllerDebug("HostHomeController setHomeWidget");
    homeWidget = widget;
    connect(homeWidget->ui->startBtn , SIGNAL(clicked()) , this , SIGNAL(startInvite())); //to hostmainwidget
    connect(homeWidget->ui->adviceBtn , SIGNAL(clicked()) ,this ,SIGNAL(onAdvice()));//to hostmainwidget
    connect(homeWidget->ui->helpBtn , SIGNAL(clicked()) , this ,  SIGNAL(onHelpBtnClicked()));//to hostmainwidget
    connect(homeWidget->ui->closeBtn , SIGNAL(clicked()) , this ,  SIGNAL(onCloseBtnClicked()));//to hostmainwidget
    connect(homeWidget->ui->minBtn , SIGNAL(clicked()) , this ,  SIGNAL(onMinBtnClicked()));
    connect(homeWidget->ui->viewBtn , SIGNAL(clicked()) , this ,  SLOT(handleOnViewBtnClicked()));
}

void HostHomeController::handleHttpGetOnMainPageInfo(bool succeed, const MainPageInfo &mainPageInfo)
{
    HostHomeControllerDebug("HostHomeController handleHttpGetOnMainPageInfo");
    HostHomeControllerDebug("succeed is: %s", succeed ? "true" : "false");
    if (!succeed)
    {
        return;
    }
    homeWidget->updateWidgetInfo(mainPageInfo);
}

void HostHomeController::handleOnViewBtnClicked()
{
    QDesktopServices::openUrl(QUrl(QString("http://m.yy.com/s/hd/ihou/index.html")));
}
