#include "PKResultController.h"
#include "ui_PKResultWidget.h"
//#include "Modules/StateController/StateController.h"

PKResultController::PKResultController(QObject *parent) :
    QObject(parent)
{

    cfg = ConfigHelper::getInstance();
    info = PkInfoManager::getInstance();

    NodeNC = NodeNotificationCenter::getInstance();
    stateCtrl = StateController::getInstance();
    connect(stateCtrl ,SIGNAL(onStatePKResult(QList<Score>, int, Prop, QList<int>)) ,this ,SLOT(handleStateControllerOnStatePKResult(QList<Score>, int, Prop, QList<int>)));
    connect(stateCtrl ,SIGNAL(onStateSelectReward(VictoryReward)) ,this, SLOT(handleStateControllerOnStateSelectReward(VictoryReward)));
}

void PKResultController::setPkResultWidget(PKResultWidget * widget)
{
    PKResultControllerDebug("PKResultController setPkResultWidget");
    pkResultWidget = widget;
    connect(pkResultWidget->ui->quitButton ,SIGNAL(clicked()) , this , SLOT(handlePKResultWidgetOnQuit()));
    connect(pkResultWidget ,SIGNAL(sendVictory(VictoryReward)) , this ,SLOT(handlePKResultWidgetOnSendVic(VictoryReward)));
    connect(pkResultWidget->ui->adviceBtn , SIGNAL(clicked()) , this , SIGNAL(onAdvice()));
    connect(pkResultWidget->ui->closebtn , SIGNAL(clicked()) , this , SIGNAL(onCloseBtnClicked()));
    connect(pkResultWidget->ui->minBtn , SIGNAL(clicked()) , this , SIGNAL(onMinBtnClicked()));
    connect(pkResultWidget , SIGNAL(audienceQuit()) , this , SLOT(handlePKResultWidgetOnQuit()));
}

void PKResultController::startAni()
{
    PKResultControllerDebug("PKResultController startAni");
    QString path = cfg->getPluginPath() + QString("Animation");
    pkResultWidget->startAni(path);
}

void PKResultController::handleStateControllerOnStatePKResult(QList<Score> scoreList, int heat, Prop propIncome, QList<int> lastcodeList)
{
    PKResultControllerDebug("PKResultController handleStateControllerOnPKResult");

    //set nonestate
    stateCtrl->sendPkEnd();

    QList<Player> pkPlayerList = info->getPkPlayerList();

    for (int i = 0; i < pkPlayerList.size(); i++)
    {
        pkPlayerList[i].setSumScore(scoreList.at(i));
    }


    //rank the score from top to bottom
    for (int i = 0; i < pkPlayerList.size(); i++)
    {
        for (int j = 0; j < pkPlayerList.size() - i - 1; j++)
        {
            if (pkPlayerList.at(j).getSumScore().getTotalScore() < pkPlayerList.at(j+1).getSumScore().getTotalScore())
            {
                pkPlayerList.swap(j, j+1);
            }
        }
    }
    //if host's score equals the first audience's score, do
    if(pkPlayerList.at(0).getIsHost())
    {
        if((int)pkPlayerList.at(0).getSumScore().getTotalScore() == (int)pkPlayerList.at(1).getSumScore().getTotalScore())
        {
            pkPlayerList.swap(0,1);
        }
    }


    if (pkPlayerList.at(0).getIsHost())
    {
        PKResultControllerDebug("Host WIN!");

        if (!info->getMe().getIsHost())
        {
            NodeNC->stop();
            NodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
                          cfg->getStatusServerIP(), cfg->getStatusServerPort(),
                          info->getChannelID(), info->getMe());
        }
    }

    pkResultWidget->updateResult(pkPlayerList, heat, propIncome);

    startAni();
}

void PKResultController::handleStateControllerOnStateSelectReward(VictoryReward reward)
{
    PKResultControllerDebug("PKResultController handleStateControllerOnSelectReward");

    //set nonestate
    stateCtrl->sendPkEnd();

    pkResultWidget->updateSelectedVictoryReward(reward);
    YYNotificationCenter *yync = YYNotificationCenter::getInstance();
    yync->sendGameOver();

    if(!info->getMe().getIsHost())
    {
        NodeNC->stop();
        NodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
                      cfg->getStatusServerIP(), cfg->getStatusServerPort(),
                      info->getChannelID(), info->getMe());
    }
}

void PKResultController::handlePKResultWidgetOnQuit()
{
    PKResultControllerDebug("PKResultController handlePKResultWidgetOnQuit");
    P2Y_PKInfo *pKInfo = P2Y_PKInfo::getInstance();
    YYNotificationCenter *yync = YYNotificationCenter::getInstance();
    pKInfo->bAcceptPlayPK = true;
    yync->sendAudiencePKInfo(pKInfo);

    emit onQuit();
    PKResultControllerDebug("quit");
}

void PKResultController::handlePKResultWidgetOnSendVic(VictoryReward vic)
{
    PKResultControllerDebug("send victoryReward");
    NodeNC->sendSelectReward(vic);

}



