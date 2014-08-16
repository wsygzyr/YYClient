#ifndef HOSTINVITEPAGECONTROLLER_H
#define HOSTINVITEPAGECONTROLLER_H

#include <QObject>
#include "Modules/HttpGet/HttpGet.h"
#include <QStringListModel>
#include "DataType/Song/Song.h"
#include "UI/HostInviteUI/HostInviteWidget.h"
#include <QSettings>
#include "UI/HelpWidget/HelpWidget.h"
#define DEBUG_HOST_INVITE_CONTROLLER  1

#if DEBUG_HOST_INVITE_CONTROLLER
#include <QDebug>
#define HostInviteControllerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define HostInviteControllerDebug(format,...)
#endif

class HostInviteController : public QObject
{
    Q_OBJECT
public:
    explicit HostInviteController(QObject *parent = 0);
    ~HostInviteController();
    void     setInviteWidget(HostInviteWidget*);
    void     startInvite();
private:
    void     setRewardToIni(const QList<VictoryReward>& vicList);

    void     getHistorySongFromIni();
    void     setHistorySongToIni();
private:
    HttpGet             *httpSearcher;
    QList<Song>         searchSongList;
    QList<Song>         historySongList;
    HostInviteWidget    *inviteWidget;

    bool                bSelectingSong;
    Song                selectedSong;
    int                 bidCount;
    bool                bGetSongPathError;
private slots:
    void     onSearcherReply(bool retCode, QList<Song> songList);
    void     handleHttpGetOnSongPathInfo(bool , Song);

    void     onBidCountChanged(QString);
    void     onStartBid();
    void     onSearchLineChanged(QString );
    void     onSongSelected(QModelIndex);

    void     onSongSelectWidgetOnSelectedSong(Song*);

    void     handleInviteWidgetReturnBtnOnClicked();
    void     handleInviteWidgetNextBtnOnClicked();
    void     handleInviteWidgetPlusBtnOnClicked();
    void     handleInviteWidgetMinusBtnOnClicked();

signals:
    void     onHostSendPkInvite(int);
    void     onReturnToHomePage();
    void     onHelpBtnClicked();
    void     onShowSongSelect();
    void     onCloseBtnClicked();
    void     onMinBtnClicked();
};

#endif // HOSTINVITEPAGECONTROLLER_H
