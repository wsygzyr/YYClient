#include "HostInviteController.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "ui_HostInviteWidget.h"
#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "UI/ToastWidget/ToastWidget.h"
#include "Modules/StateController/StateController.h"

#define DEFAULT_CHANGE_BID_COUNT 5

HostInviteController::HostInviteController(QObject *parent) :
    QObject(parent)
{
    httpSearcher = new HttpGet(this);
    bSelectingSong = false;
    connect(httpSearcher, SIGNAL(onSongList(bool ,QList<Song>)), this, SLOT(onSearcherReply(bool ,QList<Song>)));
    connect(httpSearcher , SIGNAL(onSongPathInfo(bool,Song)) , this , SLOT(handleHttpGetOnSongPathInfo(bool,Song)));
    bidCount = 0;
    bGetSongPathError = false;
}

HostInviteController::~HostInviteController()
{

}



void HostInviteController::setInviteWidget(HostInviteWidget *widget)
{
    HostInviteControllerDebug("HostInviteController setInviteWidget");
    if(widget == NULL)
    {
        HostInviteControllerDebug("HostInviteController widget == NULL");
        return;
    }

    inviteWidget = widget;
    connect(inviteWidget->ui->returnBtn, SIGNAL(clicked()), this, SLOT(handleInviteWidgetReturnBtnOnClicked()));
    connect(inviteWidget->ui->helpBtn, SIGNAL(clicked()), this, SIGNAL(onHelpBtnClicked()));
    connect(inviteWidget->ui->closeBtn, SIGNAL(clicked()), this, SIGNAL(onCloseBtnClicked()));

    connect(inviteWidget->ui->selectSongNextBtn, SIGNAL(clicked()), this, SLOT(handleInviteWidgetNextBtnOnClicked()));
    connect(inviteWidget->ui->setBidCountNextBtn, SIGNAL(clicked()), this, SLOT(handleInviteWidgetNextBtnOnClicked()));

    connect(inviteWidget->ui->selectSongBtn, SIGNAL(clicked()), this, SIGNAL(onShowSongSelect()));

    connect(inviteWidget->ui->searchLineEdit ,SIGNAL(textChanged(QString)) ,this,SLOT(onSearchLineChanged(QString)));
    connect(inviteWidget->ui->bidCountLineEdit , SIGNAL(textChanged(QString)) , this , SLOT(onBidCountChanged(QString)));
    connect(inviteWidget->ui->plusBtn, SIGNAL(clicked()), this, SLOT(handleInviteWidgetPlusBtnOnClicked()));
    connect(inviteWidget->ui->minusBtn, SIGNAL(clicked()), this, SLOT(handleInviteWidgetMinusBtnOnClicked()));

    connect(inviteWidget->ui->joinBtn ,SIGNAL(clicked()) ,this ,SLOT(onStartBid()));
    connect(inviteWidget->ui->songListWidget , SIGNAL(clicked(QModelIndex)) ,this , SLOT(onSongSelected(QModelIndex)));



    connect(inviteWidget, SIGNAL(onSearchHotSongList(QString)) , this , SLOT(onSearchLineChanged(QString)));

    connect(inviteWidget->ui->minBtn , SIGNAL(clicked()) , this , SIGNAL(onMinBtnClicked()));
}

void HostInviteController::startInvite()
{
    HostInviteControllerDebug("HostInviteController startInvite");
    selectedSong = Song();
    bSelectingSong = false;
    inviteWidget->reset();
    //get HistorySong From Ini
    getHistorySongFromIni();
}

void HostInviteController::setRewardToIni(const QList<VictoryReward>& vicList)
{
    HostInviteControllerDebug("HostInviteController setRewardToIni");
    ConfigHelper *cfg = ConfigHelper::getInstance();
    QSettings settings(QString("%1setup.ini").arg(cfg->getPluginPath()), QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
//    settings.clear();
    settings.beginWriteArray("Rewards");
    for (int i = 0; i < vicList.size(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("ID", vicList[i].getID());
        settings.setValue("name", vicList[i].getName().toLocal8Bit().data());
        settings.setValue("desp", vicList[i].getDesp().toLocal8Bit().data());
    }
    settings.endArray();
}

void HostInviteController::getHistorySongFromIni()
{
    HostInviteControllerDebug("HostInviteController getHistorySongFromIni");

    historySongList.clear();
    ConfigHelper *cfg = ConfigHelper::getInstance();
    Song song;
    QSettings settings(QString("%1setup.ini").arg(cfg->getPluginPath()), QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
    int size = settings.beginReadArray("HistoryRecords");
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        song.setSongID(settings.value("songID").toString());
        song.setName(settings.value("name").toString());
        song.setSinger(settings.value("singer").toString());
        song.setSectionCount(settings.value("sectionCount").toInt());

        historySongList.append(song);
    }
    settings.endArray();
}

void HostInviteController::setHistorySongToIni()
{
    HostInviteControllerDebug("HostInviteController setHistorySongToIni");
    ConfigHelper *cfg = ConfigHelper::getInstance();
    QSettings settings(QString("%1setup.ini").arg(cfg->getPluginPath()), QSettings::IniFormat);
    settings.setIniCodec("UTF-8");
//    settings.clear();
    settings.beginWriteArray("HistoryRecords");
    for (int i = 0; i < historySongList.size(); ++i)
    {
        settings.setArrayIndex(i);
        settings.setValue("songID", historySongList[i].getSongID().toLocal8Bit().data());
        settings.setValue("name", historySongList[i].getName().toLocal8Bit().data());
        settings.setValue("singer", historySongList[i].getSinger().toLocal8Bit().data());
        settings.setValue("sectionCount", QString::number(historySongList[i].getSectionCount()).toLocal8Bit().data());
    }
    settings.endArray();
}


void HostInviteController::onStartBid()
{
    PkInfoManager *info = PkInfoManager::getInstance();

    QList<VictoryReward> vicList = inviteWidget->getLocalVicList();
    for (int i = 0; i < vicList.length(); i ++)
    {
        if (vicList[i].getID() == 10000)
        {
            vicList.removeAt(i);
        }
    }
    info->setVictoryRewardList(vicList);
    //test
    HostInviteControllerDebug("song section id:%d",selectedSong.getSectionCount());
    //test
    vicList = inviteWidget->getLocalVicList();
    for(int i = 0; i < 4; i++ )
    {
        vicList.removeFirst();
    }
    HostInviteControllerDebug("vic desp:%s",vicList.at(0).getDesp().toUtf8().data());
    setRewardToIni(vicList);

    info->setCurrentSong(selectedSong);
    YYNotificationCenter *yync = YYNotificationCenter::getInstance();
    yync->sendHostCreateGame();
    Prop prop;
    prop.setUseNumber(bidCount);
    prop.setID(PROP_ID_PKBEAN);

    //record history song
    HostInviteControllerDebug("record history song");
    int selectSongIndex = -1;
    for (int i = 0; i < historySongList.length(); i ++)
    {
        if (historySongList.at(i).getSongID() == selectedSong.getSongID())
        {
            selectSongIndex = i;
            break;
        }
    }
    if (selectSongIndex == -1)
    {
        if (historySongList.length() >= 7)
            historySongList.removeLast();
    }
    else
    {
        historySongList.removeAt(selectSongIndex);
    }
    historySongList.push_front(selectedSong);
    //save history song
    setHistorySongToIni();

    //send to state machine
    StateController *stateCtrl = StateController::getInstance();
    stateCtrl->sendStatePkInvite(selectedSong,prop,info->getVictoryRewardList());
    emit onHostSendPkInvite(this->bidCount);
}

void HostInviteController::onSearchLineChanged(QString searchWord)
{
    if (bSelectingSong)
    {
        bSelectingSong = false;
    }

    if (searchWord.isEmpty())
    {
        //show history song
        this->onSearcherReply(true, historySongList);
        inviteWidget->ui->searchLineEdit->setToolTip("");
        return;
    }

    inviteWidget->ui->searchLineEdit->setToolTip(QString("<html><head/><body><span style=\" font-family:'宋体'; font-size:12px; color:#ffffff;\">%1</span></body></html>").arg(searchWord));

    ConfigHelper *cfg = ConfigHelper::getInstance();
    QString qsHttpPath;
    HttpGetInfo httpgetInfo;
    httpgetInfo.setType(HttpGetInfo::InfoType::SONG_LIST_INFO);
    if(searchWord.simplified() == "")
    {
        qsHttpPath = cfg->getHotSongListPrefix();
    }
    else
    {
        qsHttpPath = cfg->getSongSearchPrefix() + searchWord.simplified();
    }


    httpgetInfo.setUrl(QUrl(qsHttpPath));
    httpSearcher->request(httpgetInfo);
}

void HostInviteController::onSearcherReply(bool retCode , QList<Song> songList)
{
    HostInviteControllerDebug("HostInviteController onSearcherReply");
    if(!retCode)
        return;
    HostInviteControllerDebug("searchReply ");
    searchSongList.clear();
    QStringList songStrList;

    for(int i = 0; i < songList.count(); i++)
    {
        searchSongList.append(songList[i]);
        songStrList.append("    " + songList[i].getName() + " - " +songList[i].getSinger());
        HostInviteControllerDebug("song(%d) is %s " , i , songList[i].getName().toUtf8().data());
        if( i >= 6)
            break;
    }

    inviteWidget->updateStringListWidget(songStrList ,true);
}

void HostInviteController::onBidCountChanged(QString bidCount)
{
    if (bidCount.isEmpty())
    {
        return;
    }
    this->bidCount = bidCount.toInt();

    inviteWidget->ui->minusBtn->setEnabled(true);
    inviteWidget->ui->plusBtn->setEnabled(true);

    if (this->bidCount == 0)
    {
        inviteWidget->ui->minusBtn->setEnabled(false);
    }
    if (this->bidCount >= 99999)
    {
        inviteWidget->ui->plusBtn->setEnabled(false);
    }
}

void HostInviteController::onSongSelected(QModelIndex index)
{
    HostInviteControllerDebug("HostInviteController onSongSelected %d", index.row());
    selectedSong = searchSongList.at(index.row());

    inviteWidget->updateSearchLineText(selectedSong.getName() + " - " + selectedSong.getSinger());

    bSelectingSong = true;
    QStringList songStrList;
    for(int i = 0; i < searchSongList.length(); i++)
    {
        songStrList.append(searchSongList[i].getName() + " - " +searchSongList[i].getSinger());
    }

    inviteWidget->updateStringListWidget(songStrList ,false);

    inviteWidget->updatePlayerNumber(selectedSong.getSectionCount());

    selectedSong.setAccompanyUrl(QUrl(""));
    selectedSong.setLyricUrl(QUrl(""));
    selectedSong.setSectionUrl(QUrl(""));

    HttpGetInfo httpGetInfo;
    httpGetInfo.setType(HttpGetInfo::SONG_PATH_INFO);
    httpGetInfo.setUrl(ConfigHelper::getInstance()->getSongPathPrefix() + selectedSong.getSongID());
    httpGetInfo.setExtraData(selectedSong.getSongID());
    bGetSongPathError = false;
    httpSearcher->request(httpGetInfo);
}

void HostInviteController::handleHttpGetOnSongPathInfo(bool succeed, Song song)
{
    if(!succeed)
    {
        HostInviteControllerDebug("httpGet songPath info error");
        return;
    }
    if(selectedSong.getSongID() == song.getSongID())
    {
        bGetSongPathError = true;
        selectedSong.setAccompanyUrl(song.getAccompanyUrl());
        selectedSong.setLyricUrl(song.getLyricUrl());
        selectedSong.setSectionUrl(song.getSectionUrl());
    }
}

void HostInviteController::onSongSelectWidgetOnSelectedSong(Song *song)
{
    HostInviteControllerDebug("HostInviteController onSongSelectWidgetOnSelectedSong");
    inviteWidget->updateSearchLineText(song->getName() + " - " + song->getSinger());
    inviteWidget->updatePlayerNumber(song->getSectionCount());
    selectedSong = *song;
    bSelectingSong = true;

    HttpGetInfo httpGetInfo;
    httpGetInfo.setType(HttpGetInfo::SONG_PATH_INFO);
    httpGetInfo.setUrl(ConfigHelper::getInstance()->getSongPathPrefix() + selectedSong.getSongID());
    httpGetInfo.setExtraData(selectedSong.getSongID());
    bGetSongPathError = false;
    httpSearcher->request(httpGetInfo);

}

void HostInviteController::handleInviteWidgetReturnBtnOnClicked()
{
    HostInviteControllerDebug("HostInviteController handleInviteWidgetReturnBtnOnClicked");
    if (inviteWidget->ui->stackedWidget->currentIndex() == HostInviteWidget::SELECT_SONG_PAGE)
    {
        inviteWidget->reset();
        emit onReturnToHomePage();
    }
    else
    {
        //change stackWidget page
        inviteWidget->handleReturnBtnOnClicked();
    }
}

void HostInviteController::handleInviteWidgetNextBtnOnClicked()
{
    HostInviteControllerDebug("HostInviteController handleInviteWidgetNextBtnOnClicked");
    if (inviteWidget->ui->stackedWidget->currentIndex() == HostInviteWidget::SELECT_SONG_PAGE)
    {
        if(! bSelectingSong || selectedSong.getName() == "")
        {
            ToastWidget *toast = new ToastWidget(inviteWidget);
            toast->setText("请选择一首歌曲~");
            toast->setGeometry((inviteWidget->width() - toast->width()) / 2 , (inviteWidget->height() - toast->height() - 32) , toast->width() , toast->height());
            toast->show();
            return;
        }

        if(bGetSongPathError == false)
        {
            ToastWidget *toast = new ToastWidget(inviteWidget);
            toast->setText("该歌曲有误，请重新选择");
            toast->setGeometry((inviteWidget->width() - toast->width()) / 2 , (inviteWidget->height() - toast->height() - 32) , toast->width() , toast->height());
            toast->show();
            return;
        }
    }
    else if (inviteWidget->ui->stackedWidget->currentIndex() == HostInviteWidget::SET_BID_COUNT_PAGE)
    {
        if (inviteWidget->ui->bidCountLineEdit->text().isEmpty())
        {
            ToastWidget *toast = new ToastWidget(inviteWidget);
            toast->setText("请设置竞拍底价");
            toast->setGeometry((inviteWidget->width() - toast->width()) / 2 , (inviteWidget->height() - toast->height() - 32) , toast->width() , toast->height());
            toast->show();
            return;
        }
        P2Y_PKInfo *pKInfo = P2Y_PKInfo::getInstance();
        if (bidCount)
        {
            pKInfo->bZeroStart = false;
        }
        else
        {
            pKInfo->bZeroStart = true;
        }
    }

    //if any is ok, next
    inviteWidget->handleNextBtnOnClicked();
}

void HostInviteController::handleInviteWidgetMinusBtnOnClicked()
{
    int bidCount;
    if (this->bidCount <= 4)
        bidCount = 0;
    else
        bidCount = this->bidCount - DEFAULT_CHANGE_BID_COUNT;
    inviteWidget->ui->bidCountLineEdit->setText(QString::number(bidCount));
}

void HostInviteController::handleInviteWidgetPlusBtnOnClicked()
{
    int bidCount;
    if (this->bidCount >= 99995)
        bidCount = 99999;
    else
        bidCount = this->bidCount + DEFAULT_CHANGE_BID_COUNT;
    inviteWidget->ui->bidCountLineEdit->setText(QString::number(bidCount));
}

