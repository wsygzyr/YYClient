#ifndef HOSTINVITEWIDGET_H
#define HOSTINVITEWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QStringListModel>
#include <DataType/Song/Song.h>
#include "UI/TableWidget/TableWidget.h"
#include "PkInfoManager/PkInfoManager.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "ui/RewardEditWidget/RewardEditWidget.h"

#define DEBUG_HOST_INVITE_WIDGET  1

#if DEBUG_HOST_INVITE_WIDGET
#include <QDebug>
#define HostInviteWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define HostInviteWidgetDebug(format,...)
#endif

namespace Ui {
class HostInviteWidget;
}
class HostInviteController;
class HostInviteWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HostInviteWidget(QWidget *parent = 0);
    ~HostInviteWidget();

private:
    Ui::HostInviteWidget        *ui;
    PkInfoManager               *info;
    ConfigHelper                *cfg;
    RewardEditWidget            *rewardEditWidget;
    QList<VictoryReward>        localVicList;

private:
    QList<VictoryReward> getRewardFromIni();

    void    configReward();
    QString getToolTipStr(QString name , QString desc);
    void    mousePressEvent(QMouseEvent *);
    bool    eventFilter(QObject *, QEvent *);

    //change selectSong/setBidCount/setRewards title
    void    changeTitleState();
    void    clearTitleState();
public:
    friend class HostInviteController;
    enum ePage
    {
        SELECT_SONG_PAGE        = 0,
        SET_BID_COUNT_PAGE      = 1,
        SET_REWARDS_PAGE        = 2
    };

public slots:
    void    updatePlayerNumber(int playerNum);
    void    updateSearchLineText(const QString value);
    void    updateStringListWidget(QStringList& value , bool isShow);
    void    configUI();
    void    reset();
public slots:
    void    handleRewardEditWidgetOnCancel();
    void    handleRewardEditWidgetOnDelete(int index);
    void    handleRewardEditWidgetOnSave(int index, QString RewardName, QString RewardDesp);

    void    handleVRBtn4OnClicked(bool bCheck);
    void    handleVRBtn5OnClicked(bool bCheck);

    void    handleEditBtnOfVR4OnClicked();
    void    handleEditBtnOfVR5OnClicked();

    void    handleNextBtnOnClicked();
    void    handleReturnBtnOnClicked();

    void    handleSongListWidgetOnEnterd(QModelIndex index);
public:
    QList<VictoryReward> getLocalVicList();

signals:
    void    onSearchHotSongList(QString);
};

#endif // HOSTINVITEWIDGET_H
