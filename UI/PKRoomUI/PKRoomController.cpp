#include "PKRoomController.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "ui_PKRoomWidget.h"
#include <QMessageBox>

//#include "SingChangeErrorWidget/SingChangeErrorWidget.h"
#include "Modules/ihouAuth/des/DES.h"
#include "UI/PKRoomUI/ResourceErrorWidget/ResourceErrorWidget.h"
#include "AudioHelper/AudioHelper.h"
#include "Modules/ExceptionController/ExceptionController.h"
#include <QSound>
#include <iostream>
//#include "PKPlayerListWidget/pkplayerlistwidget.h"
//#include "Modules/StateController/StateController.h"

PKRoomController::PKRoomController(QObject *parent):
    QObject(parent)
{
    pkInfo          = PkInfoManager::getInstance();
    yync            = YYNotificationCenter::getInstance();
    nodeNC          = NodeNotificationCenter::getInstance();
    resManager      = ResManager::getInstance();
    singEngine      = SingEngine::getInstance();
    lyricParse      = LyricParse::getInstance();
    cfg             = ConfigHelper::getInstance();
    stateCtrl       = StateController::getInstance();
    exceptionController = ExceptionController::getInstance();

    //startpk signal contains playlist and songinfo to download
    connect(stateCtrl ,SIGNAL(onStatePKStart(QList<Player>)),this, SLOT(handleStateControllerOnStatePKStart(QList<Player>)));
    ///undo
    connect(stateCtrl ,SIGNAL(onStateSingChange(int,QList<int>, int, quint32)),this, SLOT(handleStateControllerOnStateSingChange(int,QList<int>, int, quint32)));
    connect(stateCtrl ,SIGNAL(onStateSingTimeOffset(float)),this, SLOT(handleStateControllerOnStateSingTimeOffset(float)));
    connect(stateCtrl ,SIGNAL(onStateScore(Score,Score)) , this ,SLOT(handleStateControllerOnStateScore(Score,Score)));
    connect(stateCtrl ,SIGNAL(onStatePropConfirm(Prop)) , this ,SLOT(handleStateControllerOnStatePropConfirm(Prop)));
    connect(stateCtrl ,SIGNAL(onStatePropActive(Player, Prop)) , this ,SLOT(handleStateControllerOnStatePropActive(Player, Prop)));
    ///undo
    connect(stateCtrl ,SIGNAL(onStatePKResult(QList<Score>, int, Prop, QList<int>)) , this ,SLOT(handleStateControllerOnStatePKResult(QList<Score>, int, Prop, QList<int>)));
//    connect(stateCtrl, SIGNAL(onStateSingReady(QList<SingReadyResult>)), this, SLOT(handleStateControllerOnStateSingReady(QList<SingReadyResult>)));
    connect(stateCtrl ,SIGNAL(onStateSingPrepare(int)) , this ,SLOT(handleStateControllerOnStateSingPrepare(int)));
    connect(stateCtrl ,SIGNAL(onStateErrorEvent(int)) , this , SLOT(handleStateControllerOnStateErrorEvent(int)));
    connect(stateCtrl ,SIGNAL(onStateUpdateHeat(int)) , this , SLOT(handleStateControllerOnStateUpdateHeat(int)));    


    connect(yync ,SIGNAL(onSwitchSingerResponse(QList<Player>)) , this ,SLOT(handleYYNotificationCenterOnSwitchSingerResponse(QList<Player>)));
    connect(yync ,SIGNAL(onTimeStamp(Player,float)) ,this, SLOT(handleYYNotificationCenterOnTimeStamp(Player,float)));
    connect(yync ,SIGNAL(onSetVolume(int, int, bool)) ,this, SLOT(handleYYNotificationCenterOnSetVolume(int, int, bool)));
    connect(yync ,SIGNAL(onUsePropResponse(Prop, int, QString, QString)) ,this, SLOT(handleYYNotificationCenterOnUsePropResponse(Prop, int, QString, QString)));
    connect(yync ,SIGNAL(onHostInfo(Player)), this, SLOT(handleYYNotificationCenterOnHostInfo(Player)));
    connect(yync ,SIGNAL(onMicTakenDown(QList<Player>)), this, SLOT(handleYYNotificationCenterOnMicTakenDown(QList<Player>)));

    connect(singEngine ,SIGNAL(onMusicTime(float)),this, SLOT(handleSingEngineOnMusicTime(float)));
    connect(singEngine ,SIGNAL(onScore(Score,Score)) ,this ,SLOT(handleSingEngineOnScore(Score,Score)));
    connect(singEngine ,SIGNAL(onSingFinish(bool)) ,this ,SLOT(handleSingEngineOnSingFinish(bool)));
    connect(singEngine , SIGNAL(onError(SingEngine::error)) , this , SLOT(handleSingEngineOnError(SingEngine::error)));

    connect(this ,SIGNAL(onSingEngineInit(QString, QString, int, int, int, int)) ,singEngine ,SLOT(init(QString, QString, int, int, int, int)));
    connect(this ,SIGNAL(onSingEngineStart(int))     , singEngine ,SLOT(start(int)));
    connect(this ,SIGNAL(onSingEngineStop())     , singEngine ,SLOT(stop()));
    connect(this ,SIGNAL(onSingEngineUninit())   , singEngine ,SLOT(unInit()));



    startTimer(5000);

    changeSpeechPropMark = 0;
    changeSpeechPropMarkForTimer = 0;
    isDownloadError = false;
    bHaveSendReady = false;
    blResourceWidgetHaveShow = false;

    blXmlParsed = false;
    blSectionXmlParsed = false;
    blPcmInit = false;
    m_nCurrentIndex = -1;
    recErrorWidget = NULL;
    recCurSingerErrorWidget = NULL;

    //sunly
//    connect(resManager ,SIGNAL(onDownloadResPrepared(QString,bool)) ,this ,SLOT(handleResManageronDownloadResPrepared(QString,bool)));
}

bool PKRoomController::boolSleep = false;

void PKRoomController::setPkWidget(PKRoomWidget *widget)
{
    PkWidget = widget;
//    connect(this, SIGNAL(propClicked(int, Player)), PkWidget, SLOT(handlePropClicked(int, Player)));

}

void PKRoomController::SleepDefine(int sleepTime)
{
    if(!boolSleep)
    {
        boolSleep = true;
        QTime t;
        t.start();
        while(t.elapsed()<sleepTime)
            QCoreApplication::processEvents();
        boolSleep = false;
    }
}

void PKRoomController::handleStateControllerOnStatePKStart(QList<Player> playerList)
{
    PKRoomControllerDebug("PKRoomController handleStateControllerOnPKStart");

    if (pkInfo->getHostPlayer().getID() == pkInfo->getMe().getID())
    {
        yync->sendSetSingerList(playerList);
    }

    QString downloadPath = cfg->getPluginPath() + cfg->getDownloadFileSavePath();
    Player Me = pkInfo->getMe();
    bool isSinger = false;
    pkInfo->setPkPlayerList(playerList);
//    if(!pkInfo->getMe().getIsHost())
//    {
//        song.clearSectionXmlData();
//        song.clearLyricXmlData();
//        resManager->reset();
//        song.setAccompanyFilePath(cfg->getPluginPath() + song.getAccompanyFilePath());

//        PKRoomControllerDebug("setCurrentSong7  songID:%s", song.getSongID().toUtf8().data());

//        QString string1 = QString(pkInfo->getCurrentSong().getLyricXmlData());
//        PKRoomControllerDebug("pkstart LyricContent:%s",string1.toUtf8().data());
//        pkInfo->setCurrentSong(song);

//        QString string2 = QString(pkInfo->getCurrentSong().getLyricXmlData());
//        PKRoomControllerDebug("pkstart LyricContent:%s",string2.toUtf8().data());
//    }
//    else
//    {
//        Song song1 = pkInfo->getCurrentSong();
//        song1.setAccompanyFilePath(cfg->getPluginPath() + song.getAccompanyFilePath());
//        PKRoomControllerDebug("setCurrentSong8");
//        pkInfo->setCurrentSong(song1);
//        QString string = QString(pkInfo->getCurrentSong().getLyricXmlData());
//        PKRoomControllerDebug("pkstart LyricContent:%s",string.toUtf8().data());
//    }

    for(int i = 0; i < pkInfo->getPkPlayerList().size() ; i++)
    {
        Player player = pkInfo->getPkPlayer(i);
        if(player.getID() == Me.getID())
        {
            isSinger = true;
        }
        player.setSectionIndex(i);
        player.setPicFileName(QString("player%1.jpg").arg(i));
        player.setPicFileSvPath(downloadPath + player.getPicFileName());
        //httpDownload->addRequest(player.getPicFileName() ,player.getHeadPicUrl(),player.getPicFileSvPath());
        pkInfo->setPkPlayer(player ,i);
    }

    PkWidget->reset();
    PkWidget->updatePlayerList(pkInfo->getPkPlayerList());
    PkWidget->UpdatePKPlayerList(pkInfo->getPkPlayerList());
    PkWidget->updateHostInfo(pkInfo->getHostPlayer());
    PkWidget->setSongInfoDisplay(pkInfo->getCurrentSong());
    bool isPlayer = false;
    for(int i = 0 ; i < pkInfo->getPkPlayerList().size() ; i++)
    {
        if(pkInfo->getPkPlayer(i).getSumScore().getTotalScore() != 0)
        {
            PkWidget->initScore(i , pkInfo->getPkPlayer(i).getSumScore());
        }

        if(pkInfo->getMe().getID() == pkInfo->getPkPlayer(i).getID() && (!pkInfo->getMe().getIsHost()))
        {
            isPlayer = true;
        }
    }
    PkWidget->UpdatePlayerListTitle(isPlayer);

    PkWidget->showPkPlayerListWidget(true);
    playerListTimer = new QTimer;
    playerListTimer->setSingleShot(true);
    connect(playerListTimer, SIGNAL(timeout()), this, SLOT(handlePKPlaygerListWidgetHide()));
    playerListTimer->start(4000);

    isDownloadError = false;
    bHaveSendReady = false;
    blResourceWidgetHaveShow = false;
    blXmlParsed = false;
    blSectionXmlParsed = false;
    blPcmInit = false;
    m_nCurrentIndex = -1;
    recErrorWidget = NULL;
    recCurSingerErrorWidget = NULL;
	if(isSinger)
    {
		yync->sendPCMBegin(22050, 1, 16);
	}
}



bool PKRoomController::ShowCurrentSingerResStatus(int curSingerResStatus, int sectionIndex)
{
    int hostNameLen = 50;
    QFont font;
    font.setPixelSize(12);
    font.setWeight(75);
    font.setBold(false);
    font.setFamily("宋体");

    if(curSingerResStatus == RESFAILED && sectionIndex != pkInfo->getMeSectionIndex())
    {
        PKRoomControllerDebug("current singer resource download failed!!!");
        Player player;
        player.setName(pkInfo->getPkPlayer(pkInfo->getCurrentPkPlayerIndex()-1).getName());
        QString errorText = QString("<font color= \"#22fffe\">%1</font> 资源下载失败，将由下一位玩家继续演唱。").arg(player.getShortCutName(font,hostNameLen));

        SingChangeErrorWidget *errorWidget = new SingChangeErrorWidget(errorText, PkWidget);
        errorWidget->SetWaitLoopLabel();
        errorWidget->move(PkWidget->mapToGlobal(QPoint(0 ,0)).x() + 143 , PkWidget->mapToGlobal(QPoint(0 , 0)).y()+54);
        errorWidget->show();
        QTimer::singleShot(3100, errorWidget , SLOT(deleteLater()));
        SleepDefine(3000);
        return false;

    }
    else if(curSingerResStatus == RESDOWNLOADING && sectionIndex != pkInfo->getMeSectionIndex())
    {
        PKRoomControllerDebug("current singer resource downloading");
        Player player;
        player.setName(pkInfo->getPkPlayer(pkInfo->getCurrentPkPlayerIndex()).getName());
        QString errorText = QString("<font color= \"#22fffe\">%1</font> 网速不给力，系统正在抢救...").arg(player.getShortCutName(font,hostNameLen));

        if(recCurSingerErrorWidget != NULL)
        {
            recCurSingerErrorWidget->close();
            recCurSingerErrorWidget->deleteLater();
            recCurSingerErrorWidget = NULL;
        }
        recCurSingerErrorWidget = new SingChangeErrorWidget(errorText, PkWidget);
        recCurSingerErrorWidget->SetWaitLoopLabel(true);
        recCurSingerErrorWidget->move(PkWidget->mapToGlobal(QPoint(0 ,0)).x() + 143 , PkWidget->mapToGlobal(QPoint(0 , 0)).y()+54);
        recCurSingerErrorWidget->show();

//        QTimer::singleShot(3100, errorWidget , SLOT(deleteLater()));
        return false;

    }
    else
    {
        PKRoomControllerDebug("current singer resource success");
        if(recCurSingerErrorWidget != NULL)
        {
            recCurSingerErrorWidget->close();
            recCurSingerErrorWidget->deleteLater();
            recCurSingerErrorWidget = NULL;
        }
    }
    return true;
}

bool PKRoomController::PaseXmlAndSection()
{
    //pase xml and sectionxml
    if(blXmlParsed == false)
    {
        blXmlParsed = true;
        QString string = QString(pkInfo->getCurrentSong().getLyricXmlData());
//            PKRoomControllerDebug("LyricContent:%s",string.toUtf8().data());

        char *buf = (char*)malloc(string.length());
        int len = DES_Decrypt(string.toUtf8().data(), string.length(), buf, DES_KEY);
        string = QString::fromLocal8Bit(buf, len);
        free(buf);

        PKRoomControllerDebug("decrypt lyric is: %s", string.toUtf8().data());

        PKRoomControllerDebug("song path is : %s",pkInfo->getCurrentSong().getLyricFilePath().data());
        if( !lyricParse->parseLyricXML(pkInfo->getCurrentSong().getLyricFilePath(),string) )
        {
            PKRoomControllerDebug("lyricFile parse error");
 //           stateCtrl->sendStateSingEnd(NodeNotificationCenter::resOpenError);
            return false;
        }
        PkWidget->initProgressBar(pkInfo->getPkPlayerList().size());
    }


    if(blSectionXmlParsed == false)
    {
        blSectionXmlParsed = true;
        QString string = QString(pkInfo->getCurrentSong().getSectionXmlData());
        //PKRoomControllerDebug("SectionContent:%s", string.toUtf8().data());

        char *buf = (char*)malloc(string.length());
        int len = DES_Decrypt(string.toUtf8().data(), string.length(), buf, DES_KEY);
        string = QString::fromLocal8Bit(buf, len);
        free(buf);

        PKRoomControllerDebug("decrypt section is: %s", string.toUtf8().data());

        if( !lyricParse->parseSectionXML(pkInfo->getCurrentSong().getSectionFilePath(),string) )
        {
            PKRoomControllerDebug("section parse error");
//            stateCtrl->sendStateSingEnd(NodeNotificationCenter::resOpenError);
            return false;
        }

        return true;
    }

}

void PKRoomController::SingChangeProcess(int sectionIndex, QList<int> &recodeList)
{
    int hostNameLen = 50;
    QFont font;
    font.setPixelSize(12);
    font.setWeight(75);
    font.setBold(false);
    font.setFamily("宋体");

    //sing chang prepare
    PKRoomControllerDebug("singer change %d %d %d",m_nCurrentIndex, sectionIndex , recodeList.size());
    if(m_nCurrentIndex != sectionIndex)
    {
        PKRoomControllerDebug("singer change ");
        // add by xianli, to reset all the PK props (e.g. LUCKY, TRANSFORMER, TOMCAT)
        pkInfo->resetProp();

        pkInfo->setCurrentPkPlayerIndex(sectionIndex-recodeList.size());
        int lastSingerIndex = pkInfo->getCurrentPkPlayerIndex();
        pkInfo->setCurrentPkPlayerIndex(sectionIndex);
        PKRoomControllerDebug("singer change lastSingerIndex %d ", lastSingerIndex);

        PkWidget->startSing(lastSingerIndex);
        if (lastSingerIndex != -1 && lastSingerIndex == pkInfo->getMeSectionIndex())
        {
            singEngine->stop();
        }
        bFirstTimeStamp = true;
        bCheered = true;
    }

    if(m_nCurrentIndex != sectionIndex)
    {
        for(int i=0; i<recodeList.size(); i++)
        {
            PKRoomControllerDebug("me is current singer, singchange error retcode: %d", recodeList[i]);
            PKRoomControllerDebug("get current %d", pkInfo->getCurrentPkPlayerIndex());
            if(recodeList[i] != 0)
            {
                PKRoomControllerDebug("PKPlayer id:%d, me id:%d", pkInfo->getPkPlayer(pkInfo->getCurrentPkPlayerIndex()-recodeList.size()+i).getID(), pkInfo->getMe().getID());
                if(pkInfo->getPkPlayer(pkInfo->getCurrentPkPlayerIndex()-recodeList.size()+i).getID() != pkInfo->getMe().getID())
                {
                    Player player;
                    player.setName(pkInfo->getPkPlayer(pkInfo->getCurrentPkPlayerIndex()-recodeList.size()+i).getName());

                    QString errorText = QString("<font color= \"#22fffe\">%1</font> 连接异常，将由下一位玩家继续演唱。(%2)").arg(player.getShortCutName(font,hostNameLen).arg(recodeList[i]));

                    SingChangeErrorWidget *errorWidget = new SingChangeErrorWidget(errorText, PkWidget);
                    errorWidget->SetWaitLoopLabel(false);
                    errorWidget->move(PkWidget->mapToGlobal(QPoint(0 ,0)).x() + 143 , PkWidget->mapToGlobal(QPoint(0 , 0)).y()+54);
                    errorWidget->show();
                    QTimer::singleShot(3100, errorWidget , SLOT(deleteLater()));
                    SleepDefine(3000);
//                           connect(errorWidget,SIGNAL(onTimeOut(int)),singEngine,SLOT(start(int)));
                }
                else
                {
                    if(recodeList[i] == 3)
                    {
                        PKRoomControllerDebug("me sing chang failed");
                        exceptionController->setExceptionNo(ExceptionController::eExceptionNo::eSingChangeFailedExcep);
                    }
                }

            }

        }

    }

}

void PKRoomController::handleStateControllerOnStateSingChange(int sectionIndex,QList<int> recodeList, int curSingerResStatus, quint32 myResStatus)
{
    PKRoomControllerDebug("PKRoomController handleStateControllerOnStateSingChange");
    PKRoomControllerDebug("receive singchange from nodeNC:sectionIndex:%d recodeList size:%d, curSingerResStatus:%d, myResStatus:%d", sectionIndex, recodeList.size(), curSingerResStatus, myResStatus);

    int hostNameLen = 50;
    QFont font;
    font.setPixelSize(12);
    font.setWeight(75);
    font.setBold(false);
    font.setFamily("宋体");

    if(pkInfo->getMeSectionIndex()!=-1 && pkInfo->getMeSectionIndex() == sectionIndex)
    {
        PKRoomControllerDebug("I am Current Singer");
        if(myResStatus&RECV_XML_ING || myResStatus&RECV_SECTION_ING || myResStatus&RECV_MP3_ING)
        {
            PKRoomControllerDebug("xml or section or mp3 is downloading");
            if(recErrorWidget == NULL)
            {
//                QString errorText = QString("您的资源正在下载中，请稍候!!!");
                Player player;
                player.setName(pkInfo->getMe().getName());
                QString errorText = QString("<font color= \"#22fffe\">%1</font> 网速不给力，系统正在抢救...").arg(player.getShortCutName(font,hostNameLen));

                if(recErrorWidget != NULL)
                {
                    recErrorWidget->close();
                    recErrorWidget->deleteLater();
                    recErrorWidget = NULL;
                }

                recErrorWidget = new SingChangeErrorWidget(errorText, PkWidget);
                recErrorWidget->SetWaitLoopLabel(true);
                recErrorWidget->move(PkWidget->mapToGlobal(QPoint(0 ,0)).x() + 143 , PkWidget->mapToGlobal(QPoint(0 , 0)).y()+54);
                recErrorWidget->show();
//              QTimer::singleShot(3100, errorWidget , SLOT(deleteLater()));
            }
            return;
        }

        if(recErrorWidget != NULL)
        {
            recErrorWidget->close();
            recErrorWidget->deleteLater();
            recErrorWidget = NULL;
        }

        if(myResStatus&RECV_XML_OK && myResStatus&RECV_SECTION_OK && myResStatus&RECV_MP3_OK )
        {
            PKRoomControllerDebug("xml or section or mp3 download success");
            //current singer error
            if(!ShowCurrentSingerResStatus(curSingerResStatus, sectionIndex))
            {
                PKRoomControllerDebug("current singer resource error!!");
                return;
            }

            //pase XML and Section
            PaseXmlAndSection();

            //sing change process
            SingChangeProcess(sectionIndex, recodeList);

            //mp3 handle process
            if(blPcmInit==false)
            {
                blPcmInit=true;
                QFile file(cfg->getPluginPath()+cfg->getDownloadFileSavePath()+ "/"+ QString::number(pkInfo->getMe().getID()) +"_song.pcm");
                if(file.size() != 0)
                {
                    PKRoomControllerDebug("song.pcm exists...");
                    resManager->cutPcm(cfg->getPluginPath()+cfg->getDownloadFileSavePath()+ "/"+ QString::number(pkInfo->getMe().getID()) +"_song.pcm",
                                       cfg->getPluginPath()+cfg->getDownloadFileSavePath()+ "/"+ QString::number(pkInfo->getMe().getID()) +"_songcut.pcm",
                                       resManager->getBits(),resManager->getChannels(),resManager->getSampleRate(),
                                       lyricParse->getSectionBeginTime(pkInfo->getMeSectionIndex()),
                                       lyricParse->getSectionEndTime(pkInfo->getMeSectionIndex()));


                    // if i am singer, init singEngine
                    ConfigHelper *cfg = ConfigHelper::getInstance();
                    QString downloadPath = cfg->getPluginPath()+cfg->getDownloadFileSavePath()+ "/"+ QString::number(pkInfo->getMe().getID()) +"_song.pcm";

                    singEngine->init(pkInfo->getCurrentSong().getAccompanyFilePath(),
                                     downloadPath,
                                     pkInfo->getMeSectionIndex(),
                                     resManager->getSampleRate(),
                                     resManager->getBits(),
                                     resManager->getChannels());
                }
                else
                {
                    isDownloadError = true;
                }

            }

            int meSectionIndex = pkInfo->getMeSectionIndex();
            PKRoomControllerDebug("meSectionIndex is: %d", meSectionIndex);

            if(!isDownloadError)
            {
                PKRoomControllerDebug("emit onSingEngineStart()");
                if(!singEngine->start(meSectionIndex))
                {
                    PKRoomControllerDebug("singEngine start failed");
                    singEngine->stop();
                    stateCtrl->sendStateSingEnd(NodeNotificationCenter::resInitError);
//                      disconnect(resManager ,SIGNAL(onDownloadResPrepared(QString,bool)) ,this ,SLOT(handleResManageronDownloadResPrepared(QString,bool)));
                }
            }
            else
            {
                PKRoomControllerDebug("res not prepared!");
                singEngine->stop();
                stateCtrl->sendStateSingEnd(NodeNotificationCenter::resOpenError);
//              disconnect(resManager ,SIGNAL(onDownloadResPrepared(QString,bool)) ,this ,SLOT(handleResManageronDownloadResPrepared(QString,bool)));
            }
        }
        else
        {
            PKRoomControllerDebug("xml or section or mp3 download failed");
            stateCtrl->sendStateSingEnd(NodeNotificationCenter::resDownloadError);
            exceptionController->setExceptionNo(ExceptionController::eExceptionNo::eResourceDownLoadFailedExcep);
            return;
        }
    }
    else
    {
        PKRoomControllerDebug("I am not Current Singer");
        if(myResStatus&RECV_XML_ING || myResStatus&RECV_SECTION_ING)
        {
            PKRoomControllerDebug("xml or section is downloading");
            if(recErrorWidget == NULL)
            {
//                QString errorText = QString("您的资源正在下载中，请稍候!!!");
                Player player;
                player.setName(pkInfo->getMe().getName());
                QString errorText = QString("<font color= \"#22fffe\">%1</font> 网速不给力，系统正在抢救...").arg(player.getShortCutName(font,hostNameLen));

                if(recErrorWidget != NULL)
                {
                    recErrorWidget->close();
                    recErrorWidget->deleteLater();
                    recErrorWidget = NULL;
                }

                recErrorWidget = new SingChangeErrorWidget(errorText, PkWidget);
                recErrorWidget->SetWaitLoopLabel(true);
                recErrorWidget->move(PkWidget->mapToGlobal(QPoint(0 ,0)).x() + 143 , PkWidget->mapToGlobal(QPoint(0 , 0)).y()+54);
                recErrorWidget->show();
 //               QTimer::singleShot(3100, errorWidget , SLOT(deleteLater()));
            }

            return;
        }

        if(recErrorWidget != NULL)
        {
            recErrorWidget->close();
            recErrorWidget->deleteLater();
            recErrorWidget = NULL;
        }

        if(myResStatus&RECV_XML_OK && myResStatus&RECV_SECTION_OK)
        {
            PKRoomControllerDebug("xml or section download success");


            //current singer error
            if(!ShowCurrentSingerResStatus(curSingerResStatus, sectionIndex))
            {
                PKRoomControllerDebug("current singer resource error!!");
                return;
            }

            //pase XML and Section
            PaseXmlAndSection();

            //sing change process
            SingChangeProcess(sectionIndex, recodeList);
        }
        else
        {
            PKRoomControllerDebug("xml or section download failed");
            exceptionController->setExceptionNo(ExceptionController::eExceptionNo::eResourceDownLoadFailedExcep);
            return;
        }
    }

    if(m_nCurrentIndex != sectionIndex)
    {
        m_nCurrentIndex = sectionIndex;
    }
}

void PKRoomController::handleStateControllerOnStateSingPrepare(int sectionIndex)
{
    PKRoomControllerDebug("PKRoomController handleStateControllerOnStateSingPrepare %d", sectionIndex);
    //send SwitchSinger while me is host
	if (! pkInfo->getMe().getIsHost())
    {
        PKRoomControllerDebug("error: sectionIndex %d is not host", sectionIndex);
        return;
    }

    if (sectionIndex <= 0 || sectionIndex >= lyricParse->getSectionCount())
    {
        PKRoomControllerDebug("error: sectionIndex is %d", sectionIndex);
        return;
    }

    pkInfo->setPrepareSectionIndex(sectionIndex);
    pkInfo->setPrepareSinger(pkInfo->getPkPlayer(sectionIndex));
    Player currentSinger = pkInfo->getPkPlayer(sectionIndex - 1);
    if (currentSinger.getID() == pkInfo->getHostPlayer().getID())
    {
        currentSinger.setID(0); // if is Host, ID should be 0 according to YY
    }
    Player nextSinger = pkInfo->getPkPlayer(sectionIndex);
    yync->sendSwitchSinger(currentSinger, nextSinger);
    QList<Player>  playerList = pkInfo->getYYonlineUserList();
	//    QList<Player>  playerList = normalPlayerList;
    PKRoomControllerDebug("playerList yyonline is %d", playerList.length());
    for (int i = 0; i < playerList.length(); i ++)
    {
        PKRoomControllerDebug("playerList at(%d) ID = %d, sectionIndex ID = %d",
                              i, playerList.at(i).getID(), pkInfo->getPkPlayer(sectionIndex).getID());
        if (playerList.at(i).getID() == pkInfo->getPkPlayer(sectionIndex).getID())
        {
            nodeNC->sendSingPrepare(pkInfo->getPrepareSectionIndex(), SINGER_PREPARE_SUCCESS);
            PKRoomControllerDebug("sing prepare : success");
            return;
        }
    }
    nodeNC->sendSingPrepare(pkInfo->getPrepareSectionIndex(), SINGER_PREPARE_FAILED);
    PKRoomControllerDebug("sing prepare : failed");
}

void PKRoomController::handleStateControllerOnStateSingTimeOffset(float time)
{
    PKRoomControllerDebug("PKRoomController handleStateControllerOnStateSingTimeOffset");
    PKRoomControllerDebug("receive timeoffset from nodeNC:timeoffset:%f",time);
    pkInfo->setTimeOffset(time);
    bFirstTimeStamp = false;
}

void PKRoomController::handleStateControllerOnStateScore(Score score, Score sumScore)
{
    PKRoomControllerDebug("PKRoomController handleStateControllerOnScore");
    PKRoomControllerDebug("receive score from nodeNC: score:%f, sumScore:%f",
                          score.getTotalScore(), sumScore.getTotalScore());

    PkWidget->setScoresMap(score, sumScore);

//    if(pkInfo->getCurrentPkPlayerIndex() != -1)
//    {
//        PkWidget->updateCurrentScore(pkInfo->getCurrentPkPlayerIndex() , score, sumScore);
//    }
}

void PKRoomController::handleStateControllerOnStatePropConfirm(Prop prop)
{
    PKRoomControllerDebug("PKRoomController handleStateControllerOnStatePropConfirm");
    PKRoomControllerDebug("receive PropConfirm from nodeNC...");
    yync->sendUsePropRequest(prop);
}

void PKRoomController::handleStateControllerOnStatePropActive(Player player, Prop prop)
{
    PKRoomControllerDebug("PKRoomController handleStateControllerOnStatePropActive");
    PKRoomControllerDebug("receive PropActive from nodeNC...");
    switch (prop.getID())
    {
    case PROP_ID_LUCKYWING:
    {
        if (pkInfo->getMe().getID() == pkInfo->getCurrentPkPlayer().getID())
        {
            prop.setIsActive(true);
            pkInfo->setLuckyWingProp(prop);
        }
        PkWidget->handlePropClicked(1, player);
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_LUCKYWING"].isValid = false;
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_LUCKYWING"].aTimer->stop();
    }
        break;
    case PROP_ID_TRANSFORMER:
    {
        changeSpeechPropMark++;
        if (pkInfo->getChangeSpeechProp().getIsActive())
        {
            PKRoomControllerDebug("changeSpeechProp is already active!");
            prop.setIsActive(false);
            pkInfo->setChangeSpeechProp(prop);
            if (pkInfo->getMe().getID() == pkInfo->getCurrentPkPlayer().getID())
            {
                singEngine->propDeactive();
            }
        }


        prop.setIsActive(true);
        pkInfo->setChangeSpeechProp(prop);
        if (pkInfo->getMe().getID() == pkInfo->getCurrentPkPlayer().getID())
        {
            singEngine->propActive(prop);
        }

        QTimer::singleShot(cfg->getPropForID(prop.getID()).getCountdownTime(), this, SLOT(handleThisOnChangeSpeechPropTimeout()));
        PkWidget->handlePropClicked(3, player);
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_TRANSFORMER"].isValid = false;
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_TRANSFORMER"].aTimer->stop();
    }
        break;
    case PROP_ID_TOMCAT:
    {
        changeSpeechPropMark++;
        if (pkInfo->getChangeSpeechProp().getIsActive())
        {
            PKRoomControllerDebug("changeSpeechProp is already active!");
            prop.setIsActive(false);
            pkInfo->setChangeSpeechProp(prop);
            if (pkInfo->getMe().getID() == pkInfo->getCurrentPkPlayer().getID())
            {
                singEngine->propDeactive();
            }
        }


        prop.setIsActive(true);
        pkInfo->setChangeSpeechProp(prop);
        if (pkInfo->getMe().getID() == pkInfo->getCurrentPkPlayer().getID())
        {
            singEngine->propActive(prop);
        }

        QTimer::singleShot(cfg->getPropForID(prop.getID()).getCountdownTime(), this, SLOT(handleThisOnChangeSpeechPropTimeout()));
        PkWidget->handlePropClicked(2, player);
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_TOMCAT"].isValid = false;
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_TOMCAT"].aTimer->stop();
    }
        break;
    case PROP_ID_PIG:
    {
        prop.setUseNumber(pkInfo->getPigProp().getUseNumber() + 1);
        pkInfo->setPigProp(prop);
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_PIG"].isValid = false;
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_PIG"].aTimer->stop();
    }
        break;
    case PROP_ID_KISS:
    {
        prop.setUseNumber(pkInfo->getKissProp().getUseNumber() + 1);
        pkInfo->setKissProp(prop);
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_KISS"].isValid = false;
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_KISS"].aTimer->stop();
    }
        break;
    case PROP_ID_SCRAWL:
    {
        prop.setUseNumber(pkInfo->getScrawlProp().getUseNumber() + 1);
        pkInfo->setScrawlProp(prop);
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_SCRAWL"].isValid = false;
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_SCRAWL"].aTimer->stop();
    }
        break;
    case PROP_ID_FROG:
    {
        prop.setUseNumber(pkInfo->getFrogProp().getUseNumber() + 1);
        pkInfo->setFrogProp(prop);
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_FROG"].isValid = false;
        PkWidget->GetPropWidget()->m_aPropButtonVoild["PROP_ID_FROG"].aTimer->stop();
    }
        break;
    default:
        break;
    }
    PkWidget->GetPropWidget()->updatePropItemWidget(prop);
    PkWidget->updateProp(player, prop);
}

void PKRoomController::handleStateControllerOnStatePKResult(QList<Score> scoreList, int heat, Prop propIncome, QList<int> lastrecodeList)
{    
    PKRoomControllerDebug("PKRoomController handleStateControllerOnStatePKResult");

    int hostNameLen = 50;
    QFont font;
    font.setPixelSize(12);
    font.setWeight(75);
    font.setBold(false);
    font.setFamily("宋体");

    DestoryDownLoadingWidget();

    PKRoomControllerDebug("pkplayer count:%d", pkInfo->getPkPlayerList().size());
    for(int i=0; i<lastrecodeList.size(); i++)
    {
        PKRoomControllerDebug("pkresult, singchange error retcode: %d", lastrecodeList[i]);
        PKRoomControllerDebug("get current %d", pkInfo->getCurrentPkPlayerIndex());
        if(lastrecodeList[i] != 0)
        {
            PKRoomControllerDebug("PKPlayer id:%d, me id:%d", pkInfo->getPkPlayer(pkInfo->getPkPlayerList().size()-lastrecodeList.size()+i).getID(), pkInfo->getMe().getID());
            if(pkInfo->getPkPlayer(pkInfo->getPkPlayerList().size()-lastrecodeList.size()+i).getID() != pkInfo->getMe().getID())
            {
                Player player;
                player.setName(pkInfo->getPkPlayer(pkInfo->getPkPlayerList().size()-lastrecodeList.size()+i).getName());

                QString errorText = QString("<font color= \"#22fffe\">%1</font> 连接异常，将由下一位玩家继续演唱。").arg(player.getShortCutName(font,hostNameLen));

                SingChangeErrorWidget *errorWidget = new SingChangeErrorWidget(errorText, PkWidget);
                errorWidget->SetWaitLoopLabel();
                errorWidget->move(PkWidget->mapToGlobal(QPoint(0 ,0)).x() + 143 , PkWidget->mapToGlobal(QPoint(0 , 0)).y()+54);
                errorWidget->show();
                QTimer::singleShot(3100, errorWidget , SLOT(deleteLater()));
                SleepDefine(3000);
//                           connect(errorWidget,SIGNAL(onTimeOut(int)),singEngine,SLOT(start(int)));
            }
            else
            {
                if(lastrecodeList[i] == 3)
                {
                    PKRoomControllerDebug("me sing chang failed");
                    exceptionController->setExceptionNo(ExceptionController::eExceptionNo::eSingChangeFailedExcep);
                }
            }

        }

    }

    if (pkInfo->getMeSectionIndex() != -1)
    {
//        disconnect(resManager ,SIGNAL(onDownloadResPrepared(QString,bool)) ,this ,SLOT(handleResManageronDownloadResPrepared(QString,bool)));
        singEngine->stop();
        singEngine->unInit();
    }

    if (pkInfo->getMe().getIsHost())
    {
        Player fakePlayer;
        fakePlayer.setID(-1);   // for the mark of PK finish
        pkInfo->setPrepareSinger(fakePlayer);

        QList<Player> playerList = pkInfo->getPkPlayerList();
        Player nextSinger;
        nextSinger.setID(0);
        for (int i = 0; i < playerList.length(); i ++)
        {
            Player singer = playerList.at(i);
            PKRoomControllerDebug("singer %d is host : %s", singer.getID(), singer.getIsHost() ? "true" : "false");
            if (! singer.getIsHost())
            {
                yync->sendSwitchSinger(singer, nextSinger);
            }
        }

//        Player currentSinger = pkInfo->getCurrentPkPlayer();
//        Player nextSinger;
//        nextSinger.setID(0);    // if is Host, ID should be 0 according to YY
//        yync->sendSwitchSinger(currentSinger, nextSinger);
    }
}

void PKRoomController::handleStateControllerOnStateErrorEvent(int errCode)
{
    PKRoomControllerDebug("PKRoomController handleStateControllerOnStateErrorEvent");
    if (errCode == -1)
    {
        PKRoomControllerDebug("error event");
//        disconnect(resManager ,SIGNAL(onDownloadResPrepared(QString,bool)) ,this ,SLOT(handleResManageronDownloadResPrepared(QString,bool)));
        singEngine->stop();
        singEngine->unInit();
        bFirstTimeStamp = true;
    }
}


void PKRoomController::handleStateControllerOnStateUpdateHeat(int heat)
{
    PKRoomControllerDebug("PKRoomController handleStateControllerOnStateUpdateHeat");
    PkWidget->updateHostHeat(heat);
}

void PKRoomController::handleStateControllerOnStateSingTick()
{
    PKRoomControllerDebug("PKRoomController handleNodeNotificationCenterOnSingTick");
    if (AudioHelper::AudioInputIsOK() && AudioHelper::AudioOutputIsOK())
    {
        PKRoomControllerDebug("earphone is ok");
    }
    else
    {
        PKRoomControllerDebug("system cannot detect earphone");
        singEngine->stop();
        nodeNC->sendSingEnd(-1);
    }
}

void PKRoomController::handleYYNotificationCenterOnSwitchSingerResponse(QList<Player> userList)
{
    PKRoomControllerDebug("PKRoomController handleYYNotificationCenterOnSwitchSingerResponse");
	//	if(!bSingPrepare)
	//	{
	//	    PKRoomControllerDebug("Not in singPrepare");
	//	    return;
	//	}
//    bRecvSwitchMsg = true;

    pkInfo->setYYonlineUserList(userList);
//    normalPlayerList = userList;
    PKRoomControllerDebug("userList length is %d", userList.length());

    bool isCurrentPlayer = false;
    bool currentPlayerInNormalPlayerList = false;
    PKRoomControllerDebug("me ID is  %d", pkInfo->getMe().getID());
    //step 1. get is current player?
    if (pkInfo->getCurrentPkPlayer().getID() == pkInfo->getMe().getID())
    {
        isCurrentPlayer = true;
    }
    //step 2. get current player is in normal list?
    if (pkInfo->getCurrentPkPlayer().getID() == -1 ||
            ! isCurrentPlayer)
    {
        //if you is not current player or player is init, continue
        currentPlayerInNormalPlayerList = true;
    }
    else
    {
        //i am current player
        if (isCurrentPlayer && pkInfo->getMe().getIsHost())
        {
            //if i am host,
            currentPlayerInNormalPlayerList = true;
        }
        for (int i = 0; i < userList.length(); i ++)
        {
            if (pkInfo->getCurrentPkPlayer().getID() == userList.at(i).getID())
            {
                currentPlayerInNormalPlayerList = true;
            }
        }
    }
    //step 3. if is not in normal.
    if (! currentPlayerInNormalPlayerList)
    {
        //need know you are normal over or host move MIC
        float currentSectionEndTime = lyricParse->getSectionEndTime(pkInfo->getCurrentPkPlayerIndex());
        float currentMusicTime = singEngine->getRecordTime() + lyricParse->getSectionBeginTime(pkInfo->getCurrentPkPlayerIndex());
        PKRoomControllerDebug("currentSectionEndTime %f, currentMusicTime %f", currentSectionEndTime, currentMusicTime);
        if (currentSectionEndTime > currentMusicTime)
        {
            //normally, recordTime > accompanyTime
            // host move MIC
            PKRoomControllerDebug("current player --> %d move MIC", pkInfo->getCurrentPkPlayer().getID());

            singEngine->stop();
            singEngine->unInit();
            nodeNC->sendSingEnd(-1);

            //send exception
            exceptionController->setExceptionNo(ExceptionController::eHostMoveSingerDownMICExcpep);
        }
    }
    //if you are current player and is normal
    if (isCurrentPlayer && currentPlayerInNormalPlayerList)
    {
        yync->sendPCMBegin(22050, 1, 16);
    }
}

void PKRoomController::handleYYNotificationCenterOnTimeStamp(Player player, float time)
{
    // "SingChange" has not been received
    if (pkInfo->getCurrentPkPlayerIndex() < 0)
    {
        return;
    }
    // wrong timestamp
    if (player.getID() != pkInfo->getCurrentPkPlayer().getID())
    {
        return;
    }

    if (pkInfo->getCurrentPkPlayer().getID() == pkInfo->getMe().getID())
    {
        PKRoomControllerDebug("i am singer");
    }
    else
    {
        PKRoomControllerDebug("i am not singer");

        if (bFirstTimeStamp)
        {
            PKRoomControllerDebug("have not got timeOffset from NODE!");
            return;
        }
        PKRoomControllerDebug("player.getID() is: %d, time: %f", player.getID(), time);
        float timeOffset = pkInfo->getTimeOffset();
        float currentTime = time
                - timeOffset
                + lyricParse->getSectionBeginTime(pkInfo->getCurrentPkPlayerIndex());
        PkWidget->updateLyric(currentTime);
        PkWidget->updateGirlTalkText(currentTime);
        PkWidget->updateCurrentSingProgress(pkInfo->getCurrentPkPlayerIndex(),currentTime);

        float currentSectionEndTime = lyricParse->getSectionEndTime(pkInfo->getCurrentPkPlayerIndex());
        if(currentSectionEndTime - currentTime <= 1000 && bCheered)
        {
            bCheered = false;
            QSound::play(cfg->getPluginPath() + "Sound\\PropSound\\cheer.wav");
        }
    }
}



void PKRoomController::handleYYNotificationCenterOnUsePropResponse(Prop prop, int retCode, QString orderNumber, QString signature)
{
    PKRoomControllerDebug("PKRoomController handleYYNotificationCenterOnUsePropResponse");
    PKRoomControllerDebug("receive PropResponse from yync...");
    if (prop.getID() == PROP_ID_PKBEAN)
    {
        PKRoomControllerDebug("prop is PKBean");
        return;
    }
    nodeNC->sendPropConfirm(prop, retCode, orderNumber, signature);
}

void PKRoomController::handleYYNotificationCenterOnHostInfo(Player player)
{
    PKRoomControllerDebug("PKRoomController handleYYNotificationCenterOnHostInfo");
    PKRoomControllerDebug("player.getIsHost() is: %s", player.getIsHost() ? "true" : "false");
    if (!player.getIsHost())
    {
//        disconnect(resManager ,SIGNAL(onDownloadResPrepared(QString,bool)) ,this ,SLOT(handleResManageronDownloadResPrepared(QString,bool)));
        singEngine->stop();
        singEngine->unInit();
        bFirstTimeStamp = true;
    }
}


void PKRoomController::handleSingEngineOnSingFinish(bool isError)
{
    PKRoomControllerDebug("PKRoomController handleSingEngineOnSingFinish");
    PKRoomControllerDebug("play finished!");
    nodeNC->sendSingEnd(isError ? isError : 0);
	singEngine->stop();
//    disconnect(resManager ,SIGNAL(onDownloadResPrepared(QString,bool)) ,this ,SLOT(handleResManageronDownloadResPrepared(QString,bool)));
}

void PKRoomController::handleSingEngineOnScore(Score sentenceScore, Score sumScore)
{
    PKRoomControllerDebug("PKRoomController handleSingEngineOnScore");
    nodeNC->sendScore(sentenceScore, sumScore);
    PkWidget->setScoresMap(sentenceScore, sumScore);

//    //test
//    if(pkInfo->getCurrentPkPlayerIndex() >= 0)
//    {
//        PkWidget->updateCurrentScore(pkInfo->getCurrentPkPlayerIndex() ,sentenceScore ,sumScore);
//    }
//    //test
}

void PKRoomController::handleSingEngineOnMusicTime(float musicTime)
{
    PKRoomControllerDebug("PKRoomController handleSingEngineOnMusicTime  musicTime(absolute):%f", musicTime);
//    PKRoomControllerDebug("musicTime(absolute) before is: %f", musicTime);
    if(pkInfo->getCurrentPkPlayerIndex() < 0)
    {
        return ;
    }
    //PKRoomControllerDebug("musicTime(absolute) is: %f", musicTime);
    // curcrent singer show lyric diff from others
    PkWidget->updateLyric(musicTime);
    PkWidget->updateGirlTalkText(musicTime);
    PkWidget->updateCurrentSingProgress(pkInfo->getCurrentPkPlayerIndex() ,musicTime);

    float currentSectionEndTime = lyricParse->getSectionEndTime(pkInfo->getCurrentPkPlayerIndex());
    if(currentSectionEndTime - musicTime <= 300 && bCheered)
    {
        bCheered = false;
        QSound::play(cfg->getPluginPath() + "Sound\\PropSound\\cheer.wav");
    }

    PKRoomControllerDebug("return");
}

void PKRoomController::handleSingEngineOnError(SingEngine::error error)
{
    PKRoomControllerDebug("PKRoomController handleSingEngineOnError");
    ToastWidget *toast = new ToastWidget(PkWidget);
    toast->setText("内部错误~");
    toast->setGeometry((PkWidget->width() - toast->width()) / 2 , (PkWidget->height() - toast->height() - 32) , toast->width() , toast->height());
    toast->show();
    singEngine->stop();
    if(pkInfo->getCurrentPkPlayerIndex() == pkInfo->getMeSectionIndex())
    {
        nodeNC->sendSingEnd(error);
    }
}



void PKRoomController::handleResouceErrorWidgetOnHide()
{
    PKRoomControllerDebug("PKRoomController handleResouceErrorWidgetOnHide");
//    YYNotificationCenter *yync = YYNotificationCenter::getInstance();
//    yync->sendGameOver();

//    if (!pkInfo->getMe().getIsHost())
//    {
//        nodeNC->stop();
//        nodeNC->start(cfg->getNodeServerIP(), cfg->getNodeServerPort(),
//                      cfg->getStatusServerIP(), cfg->getStatusServerPort(),
//                      pkInfo->getChannelID(), pkInfo->getMe());
//    }

//    emit onQuit();
    PKRoomControllerDebug("resError quit");
}

void PKRoomController::handlePKPlaygerListWidgetHide()
{
    PkWidget->showPkPlayerListWidget(false);
}

void PKRoomController::handleThisOnChangeSpeechPropTimeout()
{
    PKRoomControllerDebug("PKRoomController handleThisOnChangeSpeechPropTimeout");
    PKRoomControllerDebug("changeSpeechPropMark is: %d, changeSpeechPropMarkForTimer is: %d",
                          changeSpeechPropMark, changeSpeechPropMarkForTimer);

    changeSpeechPropMarkForTimer++;
    if (changeSpeechPropMarkForTimer != changeSpeechPropMark)
    {
        PKRoomControllerDebug("mark unmatch!");
        return;
    }

    if (pkInfo->getMe().getID() == pkInfo->getCurrentPkPlayer().getID())
    {
        singEngine->propDeactive();
    }
    Prop prop = pkInfo->getChangeSpeechProp();
    prop.setIsActive(false);
    pkInfo->setChangeSpeechProp(prop);
}

void PKRoomController::DestoryDownLoadingWidget()
{
    if(recCurSingerErrorWidget != NULL)
    {
        recCurSingerErrorWidget->close();
        recCurSingerErrorWidget->deleteLater();
        recCurSingerErrorWidget = NULL;
    }

    if(recErrorWidget != NULL)
    {
        recErrorWidget->close();
        recErrorWidget->deleteLater();
        recErrorWidget = NULL;
    }
}

void PKRoomController::timerEvent(QTimerEvent *event)
{
    PKRoomControllerDebug("5s");
}









