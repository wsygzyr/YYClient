#ifndef HOSTMAINWIDGET_H
#define HOSTMAINWIDGET_H
#include <QStackedWidget>
#include <QObject>
#include <QMouseEvent>
#include "UI/PKRoomUI/PKRoomWidget.h"
#include "UI/PKRoomUI/PKRoomController.h"
#include "UI/HostInviteUI/HostInviteWidget.h"
#include "UI/HostInviteUI/HostInviteController.h"
#include "UI/HostBidUI/HostBidWidget.h"
#include "UI/HostBidUI/HostBidController.h"
#include "UI/PKResultUI/PKResultWidget.h"
#include "UI/PKResultUI/PKResultController.h"
#include "UI/BidResultWidget/BidResultWidget.h"
#include "UI/BidResultWidget/BidResultFailedWidget.h"
#include "UI/BidResultWidget/BidResultController.h"
#include "UI/HostHomeUI/HostHomeWidget.h"
#include "UI/HostHomeUI/HostHomeController.h"
#include "UI/FeedBackWidget/FeedBackWidget.h"
#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/StateController/StateController.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include "Modules/WindowFollowHelper/WindowFollowHelper.h"
#include "UI/SongSelectWidget/SongMenuWidget.h"
#include "Modules/StateController/StateController.h"
#define DEBUG_HOST_MAIN_WIDGET  1

#if DEBUG_HOST_MAIN_WIDGET
#include <QDebug>
#define HostMainWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define HostMainWidgetDebug(format,...)
#endif

class HostMainWidget:public QStackedWidget
{
    Q_OBJECT
public:
    HostMainWidget(QWidget* parent = 0);
    ~HostMainWidget();

public slots:
    void     handleYYNotificationCenterOnMove(QRect);
    void     handleYYNotificationCenterOnShow(bool);
    void     handleYYNotificationCenterOnQuit();
    void     handleYYNotificationCenterOnGetPluginInfo();
    void     handleYYNotificationCenterOnHostInfo(Player player);
    void     handleYYNotificationCenterOnError(YYNotificationCenter::error);

    void     handleNodeNotificationCenterOnError(NodeNotificationCenter::error);
    void     handleNodeNotificationCenterOnReconnNotify(int); //sunly
    void     handleStateControllerOnStateBidStart();
    void     handleStateControllerOnStateBidSettlement(int);
    void     handleStateControllerOnStateSingReady(QList<SingReadyResult>);
    void     handleStateControllerOnStatePKStart(QList<Player>);
    void     handleStateControllerOnStatePKResult(QList<Score>, int, Prop, QList<int>);
    void     handleStateControllerOnStateLoginResult(int);
    void     handleStateControllerOnStateBidFailed();

    void     handleHostInviteControllerOnHostSendPkInvite(int);
    void     handleHostInviteControllerOnReturnToHomePage();
    void     handleHostInviteControllerOnHelpBtnClicked();
    void     handleHomeControllerOnShowSongSelect();

    void     handleHomeControllerOnStartInvite();
    void     handleHomeControllerOnAdvice();
    void     handleHomeControllerOnHelpBtnClicked();

    void     handleBidResultFailedPageOnQuit();
    void     handlePkResultControllerOnAdvice();

    void     handleHostBidControllerOnHelpBtnClicked();

    void     handlePkResultPageOnQuit();

    void     handleSongSelectPageOnQuit();
    void     handleHostOnMinBtnClicked();
    void     handleHostInviteWidgetOnMinBtnClicked();

private:

    void configMainPage();

    /*
     * HostHomeWidget
     */
    HostHomeWidget *homePage;
    HostHomeController *homeController;
    void configHomePage();

    /*
     * HostInviteWidget
     */
    HostInviteWidget *invitePage;
    HostInviteController *inviteController;
    void configInvitePage();

    /*
     * HostBidWidget
     */
    HostBidWidget    *bidPage;
    HostBidController *bidController;
    void configBidPage();
    /*
     * BidResult
     */
    BidResultWidget        *bidResulsPage;
    BidResultController    *bidResultController;
    void configBidResultPage();
    /*
     * BidResultFailedWidget
     */
    BidResultFailedWidget  *bidResultFailedPage;
    void configBidResultFailedPage();
    /*
     * PKRoom
     */
    PKRoomWidget     *PkPage;
    PKRoomController *PkController;
    void configPkPage();

    /*
     * PKResultWidget
     */
    PKResultWidget   *pkResultPage;
    PKResultController *pkResultController;
    void configPkResultPage();

    /*
     *  FeedBackWidget
     */
    FeedBackWidget *feedBackWidget;
    void configFeedbackPage();
    /*
     *  HelpWidget
     */
    HelpWidget          *helpWidget;
    void configHelpPage();
    /*
     * SongSelectWidget
     */
    SongMenuWidget      *songMenuWidget;
    void configSongSelectPage();
    /*
     * YYNotificationCenter
     */
    YYNotificationCenter *yync;

    /*
     * NodeNotificationCenter
     */
    NodeNotificationCenter *NodeNC;
    /*
     * PkInfoManager
     */
    PkInfoManager        *info;

    P2Y_PKInfo *pKInfo;

    WindowFollowHelper   *followHelper;

    StateController         *stateCtrl;
    void configFollowHelper();
private:


    QPoint      windowPos;
    QPoint      mousePos;
    QPoint      dPos;
    bool        bMouseLeftBtnDown;
    QPoint      pointMove;

    void    mousePressEvent(QMouseEvent *event);
    void    mouseMoveEvent(QMouseEvent *event);
    void    mouseReleaseEvent(QMouseEvent *event);

public:
    void    showHostMainWidget();
};

#endif // HOSTMAINWIDGET_H
