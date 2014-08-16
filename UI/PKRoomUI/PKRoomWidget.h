#ifndef PKROOMWIDGET_H
#define PKROOMWIDGET_H

#include <QWidget>
#include "Animation/AnimationManager.h"
#include "HostInfoWidget/HostInfoWidget.h"
#include "PeopleAnimeWidget/PeopleAnimeWidget.h"
#include "MessageWidget/MessageWidget.h"
#include "PlayerListWidget/PlayerListWidget.h"
#include "ScoreWidget/ScoreWidget.h"
#include "PropWidget/PropWidget.h"
#include "LyricWidget/LyricWidget.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/FadeHelper/FadeHelper.h"
#include "Modules/TextShowHelper/TextShowHelper.h"
#include "UI/PKRoomUI/PKPlayerListWidget/pkplayerlistwidget.h"
#include "UI/PKRoomUI/SongInfoDisplayWidget/SongInfoDisplayWidget.h"
#include "Score/Score.h"

#define DEBUG_PKROOM_WIDGET   1
#if DEBUG_PKROOM_WIDGET
#include <QDebug>
#define PKRoomWidgetDebug(format,...) qDebug("%s,LINE: %d -->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define PKRoomWidgetDebug(format,...)
#endif

class PKRoomController;
namespace Ui {
class PKRoomWidget;
}

struct Scores
{
    Score sentenceScore;
    Score sumScore;
};

class PKRoomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PKRoomWidget(QWidget *parent = 0);
    ~PKRoomWidget();

private:
    Ui::PKRoomWidget             *ui;
    HostInfoWidget               *hostInfoWidget;
    PeopleAnimeWidget            *peopleAnimeWidget;
    MessageWidget                *messageWidget;
    PlayerListWidget             *playerListWidget;
    PKPlayerListWidget           *pkPlayerListWidget;
    ScoreWidget                  *scoreWidget;
    PropWidget                   *propWidget;
    LyricWidget                  *lyricWidget;

    SongInfoDisplayWidget        *songInfoDisplayWidget;

    int                          headPicWidth;
    int                          middlePicWidth;
    int                          tailPicWidth;

    PkInfoManager                *info;
    AnimationManager             *animeMgr;
    LyricParse                   *lyricParse;
    FadeHelper                   *fadeHelper;

    int                          m_nCountTime;
    CTextShowHelper              m_aTextShowHelper;

    QMap<int, Scores>            scoresMap;
    QMap<int, bool>              lysicSentences;


public:
    void     initProgressBar(int numPlayers);
    void     updatePlayerList(QList<Player> &playerList);
    void     updatePopulate(const int &count);
	void     updateHostInfo(Player host);
    void     updateHostHeat(int heat);
    void     updatePlayerList();
    void     updateProp(Player,Prop);
    void     updateCurrentSingProgress(int currentIndex, const float& currentTime);
    void     initScore(int currentIndex , Score&);
    void     updateCurrentScore(int currentIndex, Score&, Score&);
    void     updateLyric(float);
    void     updateGirlTalkText(float);
    void     startSing(int lastSingerIndex);
    void     reset();
    void     animateProp(Prop);
    PropWidget* GetPropWidget();



    void UpdatePKPlayerList(QList<Player> playerList);
    void UpdatePlayerListTitle(bool isPlayer);

    void showPkPlayerListWidget(bool isShow);
	
	void     setSongInfoDisplay(Song song);
    void     setScoresMap(Score sentenceScore, Score sumScore);

private slots:
    QRect    randomArea(QRect);
    void     handlePropClicked(int num, Player player);
    void     handleShowScore(int currentIndex);
signals:
    void     propClicked(const Player &player);
    friend   class PKRoomController;
};

#endif // PKROOMWIDGET_H
