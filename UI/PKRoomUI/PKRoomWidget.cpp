#include "PKRoomWidget.h"
#include "ui_PKRoomWidget.h"
#include "ui_PropWidget.h"



PKRoomWidget::PKRoomWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PKRoomWidget)
{
    ui->setupUi(this);

    hostInfoWidget = new HostInfoWidget(ui->hostInfoWidget);
    peopleAnimeWidget = new PeopleAnimeWidget(ui->peopleAnimeWidget);

    messageWidget  = new MessageWidget(ui->messageWidget);
    playerListWidget = new PlayerListWidget(ui->playerListWidget);
    pkPlayerListWidget = new PKPlayerListWidget(ui->pkplayerlistwidget);
    propWidget     = new PropWidget(ui->propWidget);
    scoreWidget    = new ScoreWidget(ui->scoreWidget);
    lyricWidget    = new LyricWidget(ui->lyricWidget);
    fadeHelper     = new FadeHelper(this);
    songInfoDisplayWidget = new SongInfoDisplayWidget(ui->songInfoDisplayWidget);

    headPicWidth = QPixmap("://image/PKRoomUI/headLoc.png").width() - 9;
    middlePicWidth = QPixmap("://image/PKRoomUI/MiddleLoc.png").width();
    tailPicWidth = QPixmap("://image/PKRoomUI/TailLoc.png").width();

    info = PkInfoManager::getInstance();
    lyricParse = LyricParse::getInstance();
    animeMgr   = AnimationManager::getInstance();

    m_nCountTime = 0;

    ui->SpecialLable->hide();
    connect(lyricWidget, SIGNAL(OneSentenceEnd(int)), this, SLOT(handleShowScore(int)));
}

PKRoomWidget::~PKRoomWidget()
{
    delete ui;
}




void PKRoomWidget::initProgressBar(int numPlayers )
{
    PKRoomWidgetDebug("PKRoomWidget initProgressBar");
    QString style = "QProgressBar::chunk {"
            "background-color: #0084e6;"
            "}";
    style.append("QProgressBar {"
                 "border-radius: 2px;"
                 "background: rgba(255,255,255,0);"
                 "}");
    ui->progressBar->setStyleSheet(style);
    int numMiddlePlayers = numPlayers - 2;
    if(numMiddlePlayers < 0)
        numMiddlePlayers = 0;
    ui->progressBar->setGeometry( ui->progressBar->x() ,  ui->progressBar->y() ,headPicWidth + tailPicWidth + numMiddlePlayers * middlePicWidth , 2);
    ui->progressBar->setRange(.0 , ui->progressBar->width());
    ui->progressBar->setValue(0);
    ui->progressBar->show();
}


void PKRoomWidget::updateHostInfo(Player host)
{
    hostInfoWidget->updateHostInfo(host);
}

void PKRoomWidget::updateHostHeat(int heat)
{
    hostInfoWidget->updateHostHeat(heat);
}


void PKRoomWidget::reset()
{
    PKRoomWidgetDebug("PKRoomWidget reset");
    lyricWidget->hide();
    pkPlayerListWidget->hide();
    ui->progressBar->hide();
    messageWidget->reset();
    propWidget->resetProp();
    hostInfoWidget->reset();
    scoreWidget->reset();
    m_aTextShowHelper.ClearTextSet();
    m_nCountTime = 0;
    scoresMap.clear();
    lysicSentences.clear();

}

PropWidget *PKRoomWidget::GetPropWidget()
{
    return propWidget;
}

void PKRoomWidget::UpdatePKPlayerList(QList<Player> playerList)
{
    pkPlayerListWidget->UpdatePKPlayerList(playerList);
}

void PKRoomWidget::UpdatePlayerListTitle(bool isPlayer)
{
    pkPlayerListWidget->UpdatePlayerListTitle(isPlayer);
}

void PKRoomWidget::showPkPlayerListWidget(bool isShow)
{
    pkPlayerListWidget->PKPlayerListWidgetShow(isShow);
}

void PKRoomWidget::setSongInfoDisplay(Song song)
{
    songInfoDisplayWidget->setText(QString("《%1》  by %2").arg(song.getName(), song.getSinger()));
    PKRoomWidgetDebug("song - singer : %s", song.getSinger().toUtf8().data());
}

void PKRoomWidget::setScoresMap(Score sentenceScore, Score sumScore)
{
    PKRoomWidgetDebug("inset score index :%d", sentenceScore.getSentenceIndex());
    int index = sentenceScore.getSentenceIndex();
    Scores scores;
    scores.sentenceScore = sentenceScore;
    scores.sumScore = sumScore;
    scoresMap.insert(index, scores);

    QMap<int, bool>::iterator itr = lysicSentences.begin();
    while(itr != lysicSentences.end())
    {
        if(itr.key() >= index && itr.value()==false)
        {
            PKRoomWidgetDebug("inset score lysic have end index: %d", index);
            itr.value() = true;
            updateCurrentScore(info->getCurrentPkPlayerIndex(), sentenceScore, sumScore);

        }
        itr++;
    }

}

QRect PKRoomWidget::randomArea(QRect area)
{
    int x ,y ,w ,h;
    qsrand(QTime().currentTime().msec());
    w = area.width();
    h = area.height();
    if((this->width() - w - x) != 0)
        x = area.x() + qrand() % (this->width() - w - area.x());
    else
        x = area.x();
    if((this->height() - h - y) != 0)
        y = area.y() + qrand() % (this->height() - h - area.y());
    else
        y = area.y();


    return QRect(x , y ,w,h);
}


void PKRoomWidget::updatePlayerList(QList<Player> &playerList)
{
    playerListWidget->updatePlayerList(playerList);
}

void PKRoomWidget::updatePopulate(const int &count)
{
    //
}

void PKRoomWidget::updateProp(Player player,Prop prop)
{
    PKRoomWidgetDebug("PKRoomWidget updateProp");
    switch (prop.getID())
    {
    case PROP_ID_LUCKYWING:
    {
        propWidget->updateLuckyWing();
        animeMgr->addAnimation(this , QRect(268 , 14 , 300 , 180) ,prop);
        break;
    }
    case PROP_ID_PIG:
    {
        animeMgr->addAnimation(this , QRect(353 , 90 , 160 , 120) , prop);
        break;
    }
    case PROP_ID_KISS:
    {
        animeMgr->addAnimation(this , randomArea(QRect(0 , 0 , 200 , 200 )) ,prop);
        break;
    }
    case PROP_ID_SCRAWL:
    {
        animeMgr->addAnimation(this , QRect(ui->lyricWidget->x() ,ui->lyricWidget->y() - 30 ,305 , 105) , prop);
        break;
    }
    case PROP_ID_FROG:
    {
        animeMgr->addAnimation(this , QRect(0 , 90 , 260 , 100) , prop);
        break;
    }

    case PROP_ID_TRANSFORMER:
    case PROP_ID_TOMCAT:
    {
        peopleAnimeWidget->animateProp(prop);
    }

    default:
        break;
    }
    messageWidget->updateMessage(player, prop);
}

void PKRoomWidget::updateCurrentSingProgress(int currentIndex , const float &currentTime)
{
//    PKRoomWidgetDebug("PKRoomWidget updateCurrentSingProgress");
    LyricParse    *lyricParse = LyricParse::getInstance();
    float startTime = lyricParse->getSectionBeginTime(currentIndex);
    float endTime   = lyricParse->getSectionEndTime(currentIndex);
    float percent   = (currentTime - startTime) / (endTime - startTime);

    int  currentPosInProgressbar = 0;
    int  totalNumPlayers = info->getPkPlayerList().size();
    if (totalNumPlayers == 4)
    {
        switch (currentIndex) {
        case 0:
            currentPosInProgressbar = (int)((float)headPicWidth * percent);
            break;
        case 1:
            currentPosInProgressbar = (int)((float)middlePicWidth * percent) + headPicWidth;
            break;
        case 2:
            currentPosInProgressbar = (int)((float)middlePicWidth * percent) + headPicWidth + middlePicWidth;
            break;
        case 3:
            currentPosInProgressbar = (int)((float)tailPicWidth * percent) + headPicWidth + 2 * middlePicWidth;
        default:
            break;
        }
    }
    if (totalNumPlayers == 3)
    {
        switch (currentIndex) {
        case 0:
            currentPosInProgressbar = (int)((float)headPicWidth * percent);
            break;
        case 1:
            currentPosInProgressbar = (int)((float)middlePicWidth * percent) + headPicWidth;
            break;
        case 2:
            currentPosInProgressbar = (int)((float)tailPicWidth * percent) + headPicWidth + middlePicWidth;
            break;
        default:
            break;
        }
    }
    if (totalNumPlayers == 2)
    {
        switch (currentIndex) {
        case 0:
            currentPosInProgressbar = (int)((float)headPicWidth * percent);
            break;
        case 1:
            currentPosInProgressbar = (int)((float)tailPicWidth * percent) + headPicWidth;
            break;
        default:
            break;
        }
    }
    ui->progressBar->setValue(currentPosInProgressbar);
    ui->progressBar->show();
}

void PKRoomWidget::initScore(int currentIndex, Score & score)
{
    playerListWidget->updatePlayerScore(currentIndex , score);
}

void PKRoomWidget::updateCurrentScore(int currentIndex , Score &score, Score &sumScore)
{
    PKRoomWidgetDebug("PKRoomWidget updateCurrentScore");
    scoreWidget->updateScore(score);
    // QTimer::singleShot(1000 , fadeOutHelper , SLOT(startFade()));
    playerListWidget->updatePlayerScore(currentIndex , sumScore);
    if(score.getTotalScore() > 60 && score.getTotalScore() <=70)
        peopleAnimeWidget->updateText(1 , QString("comeon"));
    else if(score.getTotalScore() > 70 && score.getTotalScore() <= 80 )
        peopleAnimeWidget->updateText(1 , QString("cool"));
    else if(score.getTotalScore() > 80 && score.getTotalScore() <= 90 )
        peopleAnimeWidget->updateText(1 , QString("great"));
}

void PKRoomWidget::updateLyric(float time)
{
    lyricWidget->updatePlayerTime(time);
}

void PKRoomWidget::updateGirlTalkText(float time)
{   
//    PKRoomWidgetDebug("PKRoomWidget updateGirlTalkText");
    //PKRoomWidgetDebug("time is %f" , time);
    if(0 == info->getCurrentPkPlayerIndex() && !info->getMe().getIsHost())
    {
        QString str;
        if (time > 9900)
        {
            str = "快使用道具给主播捣乱吧！";
            if(m_aTextShowHelper.IsNeedShowText(1, str))
            {
               peopleAnimeWidget->updateText(2 , str);
            }
        }

        if(time > 19900)
        {
            str = "高调唱歌不如低调捣乱~";
            if(m_aTextShowHelper.IsNeedShowText(1, str))
            {
               peopleAnimeWidget->updateText(2 , str);
            }
        }

        if(time > 29900)
        {
            str = "变声一出，肯定跑偏!";
            if(m_aTextShowHelper.IsNeedShowText(1, str))
            {
               peopleAnimeWidget->updateText(2 , str);
            }
        }
    }
 /*   else
    {
        m_aTextShowHelper.DeleteText(10000, "快使用道具给主播捣乱吧！");
        m_aTextShowHelper.DeleteText(20000, "高调唱歌不如低调捣乱~");
        m_aTextShowHelper.DeleteText(30000, "变声一出，肯定跑偏!");
    }
    */

    if(info->getMeSectionIndex() == info->getCurrentPkPlayerIndex() && !info->getMe().getIsHost())
    {
        //PKRoomWidgetDebug("after time is %f" , time - lyricParse->getSectionBeginTime(info->getMeSectionIndex()));
        if((time - lyricParse->getSectionBeginTime(info->getMeSectionIndex())) > 9900)
        {
            QString str = "使用幸运之翼，分数蹭蹭看涨！";
            if(m_aTextShowHelper.IsNeedShowText(1, str))
            {
                peopleAnimeWidget->updateText(3 , str);
            }
        }
    }
/*    else
    {
        m_aTextShowHelper.DeleteText(10000, "使用幸运之翼，分数蹭蹭看涨！");
    }
    */

    //if(info->getMeSectionIndex() != -1 /*&& !info->getMe().getIsHost()*/)

    int currentIndex = info->getCurrentPkPlayerIndex();
    int NameLen = 61;
    QString str;
    if(time > 1000 && currentIndex == 0) //for host
    {
        if(m_aTextShowHelper.IsNeedShowText(1, "即将开始演唱第一段，请准备"))
        {
            QString nameLbStr = QString("%1").arg(info->getHostPlayer().getShortCutName(ui->SpecialLable->font(), NameLen));
            str = QString::fromLocal8Bit("<html><head/><body><p><span style=\" color:#fff000;\">%1</span><span style=\" color:#ffffff;\">即将开始演唱第一段，请准备</span></p></body></html>").arg(nameLbStr);
            fadeHelper->StopShow();
            ui->SpecialLable->setText(str);
            fadeHelper->DealFadeProcess(ui->SpecialLable,250, 500, 3250);
        }


    }

    if(currentIndex < info->getPkPlayerList().size() -1)
    {
        int lastStIndex = lyricParse->getSectionLastSentenceIndex(currentIndex);
        float lastStEndTime = lyricParse->getSentenceEndTime(lastStIndex);

        if(lastStEndTime - time <= 2200)
        {      
            QString str = info->getCurrentPkPlayer().getName();
            str += QString("即将演唱结束，下一位请准备");
            if(m_aTextShowHelper.IsNeedShowText(2, str))
            {
                QString nameLbStr = QString("%1").arg(info->getCurrentPkPlayer().getShortCutName(ui->SpecialLable->font(), NameLen));
                str = QString::fromLocal8Bit("<html><head/><body><p><span style=\" color:#fff000;\">%1</span><span style=\" color:#ffffff;\">即将演唱结束，下一位请准备</span></p></body></html>").arg(nameLbStr);
                fadeHelper->StopShow();
                ui->SpecialLable->setText(str);
                fadeHelper->DealFadeProcess(ui->SpecialLable,250, 500, 3250);
                //           peopleAnimeWidget->updateText(4 , str);

            }

        }
    }

    if(info->getMeSectionIndex() != -1)
    {
        int nextSectionIndex = info->getMeSectionIndex();
        int firstStIndexInMeSection = lyricParse->getSectionFirstSentenceIndex(nextSectionIndex);
        float nextStBeginTime = lyricParse->getSentenceBeginTime(firstStIndexInMeSection);

        if(m_nCountTime == 0 && nextStBeginTime - time <= 5999)
        {
            m_nCountTime = (nextStBeginTime - time)/1000;
        }

        if(nextStBeginTime - time <= 5100 && m_nCountTime==5)
        {
            peopleAnimeWidget->updateText(5 ,"ready");
            m_nCountTime--;
        }
        else if(nextStBeginTime - time <= 4100 && m_nCountTime==4)
        {
            peopleAnimeWidget->updateText(5 ,"3");
            m_nCountTime--;
        }
        else if(nextStBeginTime - time <= 3100 && m_nCountTime==3)
        {
            peopleAnimeWidget->updateText(5 ,"2");
            m_nCountTime--;
        }
        else if(nextStBeginTime - time <= 2100 && m_nCountTime==2)
        {
            peopleAnimeWidget->updateText(5 ,"1");
            m_nCountTime--;
        }
        else if(nextStBeginTime - time <= 1100 && m_nCountTime==1)
        {
            peopleAnimeWidget->updateText(5 ,"go");
            m_nCountTime=0;
        }

    }
}

void PKRoomWidget::startSing(int lastSingerIndex)
{
    PKRoomWidgetDebug("PKRoomWidget startSing");
    int sentenceIndex = lyricParse->getSectionFirstSentenceIndex(info->getCurrentPkPlayerIndex());
    if (sentenceIndex < 0)
    {
        return;
    }
    if (sentenceIndex & 1)
    {
        sentenceIndex -= 1;
    }
    PKRoomWidgetDebug("sentenceIndex is %d" ,sentenceIndex);
    lyricWidget->start(sentenceIndex);

    scoreWidget->hide();

    playerListWidget->updateCurrentSinger(info->getCurrentPkPlayerIndex() , lastSingerIndex);

    // stop prop button's animation
    propWidget->resetProp();
    animeMgr->stopAllAnimation();

}

void PKRoomWidget::handlePropClicked(int num, Player player)
{
    PKRoomWidgetDebug("PKRoomWidget handlePropClicked");
    int NameLen = 61;
    QString nameLbStr = player.getShortCutName(ui->SpecialLable->font(), NameLen);
    switch(num)
    {
    case(1):
    {
        ui->SpecialLable->setText(QString::fromLocal8Bit("<html><head/><body><p><span style=\" font-family:'宋体'; color:#fff000;\">%1</span><span style=\" font-family:'宋体'; color:#ffffff;\">祭出了</span><span style=\" font-family:'宋体'; color:#fff000;\">幸运之翼</span><span style=\" font-family:'宋体'; color:#ffffff;\">，得分蹭蹭上涨</span></p></body></html>").arg(nameLbStr));
        fadeHelper->DealFadeProcess(ui->SpecialLable,250, 500, 2250);
        break;
    }
    case(2):
    {
        ui->SpecialLable->setText(QString::fromLocal8Bit("<html><head/><body><p><span style=\" color:#fff000;\">%1</span><span style=\" color:#ffffff;\">使用了</span><span style=\" color:#fff000;\">汤姆猫</span><span style=\" color:#ffffff;\">，喵星人的世界你不懂~</span></p></body></html>").arg(nameLbStr));
        fadeHelper->DealFadeProcess(ui->SpecialLable,250, 500, 2250);
        break;
    }
    case(3):
    {
        ui->SpecialLable->setText(QString::fromLocal8Bit("<html><head/><body><p><span style=\" font-family:'宋体'; font-size:12px; color:#fff000;\">%1</span><span style=\" font-family:'宋体'; font-size:12px; color:#ffffff;\">使出了</span><span style=\" font-family:'宋体'; font-size:12px; color:#fff000;\">变形金刚</span><span style=\" font-family:'宋体'; font-size:12px; color:#ffffff;\">，擎天柱来也</span><span style=\" font-family:'Calibri,sans-serif'; font-size:12px; color:#ffffff;\">~</span></p></body></html>").arg(nameLbStr));
        fadeHelper->DealFadeProcess(ui->SpecialLable,250, 500, 2250);
        break;
    }
    default:
        break;

    }
}

void PKRoomWidget::handleShowScore(int currentIndex)
{
    PKRoomWidgetDebug("show core %d", currentIndex);
    lysicSentences.insert(currentIndex, false);
    QMap<int, Scores>::iterator itr = scoresMap.begin();
    while(itr != scoresMap.end())
    {
        PKRoomWidgetDebug("core map index: %d", itr.key());
        if(currentIndex <= itr.key() && info->getCurrentPkPlayerIndex() >=0)
        {
            if(lysicSentences.find(itr.key())!= lysicSentences.end())
            {
                lysicSentences[itr.key()] = true;
                updateCurrentScore(info->getCurrentPkPlayerIndex(), itr.value().sentenceScore, itr.value().sumScore);
            }

        }
        itr++;
    }
}




