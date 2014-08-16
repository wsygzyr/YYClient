#ifndef AUDIENCEMAINWIDGET_H
#define AUDIENCEMAINWIDGET_H
#include <QStackedWidget>
#include <QMouseEvent>
#include "UI/AudienceBidWidget/AudienceBidWidget.h"
#include "UI/AudienceInvitePKWidget/AudienceInvitePKWidget.h"
#include "UI/AudienceInvitePKWidget/AudienceInviteController.h"
#include "UI/BidResultWidget/BidResultWidget.h"
#include "UI/BidResultWidget/BidResultFailedWidget.h"
#include "UI/PKRoomUI/PKRoomController.h"
#include "UI/PKRoomUI/PKRoomWidget.h"
#include "UI/PKResultUI/PKResultController.h"
#include "UI/PKResultUI/PKResultWidget.h"
#include "UI/AudienceErrorWidget/AudienceErrorWidget.h"
#include "UI/FeedBackWidget/FeedBackWidget.h"
#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/SingEngine/SingEngine.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "UI/HelpWidget/HelpWidget.h"
#include "Modules/WindowFollowHelper/WindowFollowHelper.h"
#include "UI/BidResultWidget/BidResultController.h"
#include "Modules/StateController/StateController.h"


#define DEBUG_AUDIENCE_MAIN_WIDGET  1



#if DEBUG_AUDIENCE_MAIN_WIDGET
#include <QDebug>
#define AudienceMainWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define AudienceMainWidgetDebug(format,...)
#endif

#define FIRST_SHOW_GEOMETRY_X           450
#define FIRST_SHOW_GEOMETRY_Y           200
#define GEOMETRY_WIDTH                  594
#define GEOMETRY_HEIGHT                 266
#define AUDIENCE_INVITE_WIDGET_WIDTH    402
#define AUDIENCE_INVITE_WIDGET_HEIGHT   247

class AudienceMainWidget:public QStackedWidget
{
    Q_OBJECT
public:
    explicit AudienceMainWidget(QWidget *Parent = 0);
    ~AudienceMainWidget();
    //data
private:
    AudienceInvitePKWidget    *audienceInvitePKWidget;
    AudienceInviteController  *audienceInviteController;
    AudienceBidWidget         *audienceBidWidget;
    AudienceErrorWidget       *audienceErrorWidget;
    BidResultWidget           *bidResultWidget;
    BidResultController       *bidResultController;
    BidResultFailedWidget     *bidResultFailedWidget;
    PKRoomWidget              *pkRoomWidget;
    PKRoomController          *pkRoomController;
    PKResultWidget            *pkResultWidget;
    PKResultController        *pkResultController;
    FeedBackWidget            *feedBackWidget;
    HelpWidget                *helpWidget;
    YYNotificationCenter      *yync;
    NodeNotificationCenter    *NodeNC;
    PkInfoManager             *info;
    P2Y_PKInfo                *pKInfo;
    WindowFollowHelper        *followHelper;
    StateController           *stateCtrl;

private:
    void configUI();
    void configSlots();
public slots:
    void handleYYNotificationCenterOnShow(bool bShow);
    void handleYYNotificationCenterOnMove(QRect rect);
    void handleYYNotificationCenterOnQuit();
    void handleYYNotificationCenterOnGetPluginInfo();
    void handleYYNotificationCenterOnError(YYNotificationCenter::error);
    void handleYYNotificationCenterOnHostInfo(Player player);

    void handleNodeNotificationCenterOnError(NodeNotificationCenter::error);
    //void handleNodeNotificationCenterOnLogin(int retCode);
    void handleNodeNotificationCenterOnReconnNotify(int); 
    //for OnInvitePK slots
    void handleStateControllerOnStatePKInvite(Player host,
                                                Song song,
                                                Prop bidProp,
                                                QList<VictoryReward> VictoryRewardList,
                                                int acceptedPlayers);

    void handleStateControllerOnStateBidStart();
    void handleNodeNotificationCenterOnBidEnd();

    //for BidResult widget
    void handleNodeNotificationCenterOnBidResult(QList<Player>, QList<Prop>);
    //for pkroom
    void handleStateControllerOnStatePKStart(QList<Player>);
    //for audiencePKResult slots
    void handleStateControllerOnStatePKResult(QList<Score>, int , Prop, QList<int>);

    void handleStateControllerOnStateErrorEvent(int);
    void handleStateControllerOnStateBidFailed();

    void handleAudienceErrorWidgetOnQuit();
    void handlePkResultControllerOnAdvice();
    void handleAudienceBidWidgetOnHelpBtnClicked();
    void handleStateControllerOnStateLoginResult(int);
    void handleAudienceInvitePKWidgetOnAcceptInviteEvent();
    void handleStateControllerOnStateBidSettlement(int);
    void handleStateControllerOnStateSingReady(QList<SingReadyResult>);
	void handleAudienceInviteWidgetClose();

    void handleAudienceBidWidgetClose();
    void handleAudienceBidWidgetMin();

    void handleBidResultWidgetClose();
    void handleBidResultWidgetMin();

    void handlePkResultWidgetClose();
    void handlePkResultWidgetMin();

private:
    QPoint      windowPos;
    QPoint      mousePos;
    QPoint      dPos;
    bool        bMouseLeftBtnDown;
    QPoint      pointMove;
    void    mousePressEvent(QMouseEvent *event);
    void    mouseMoveEvent(QMouseEvent *event);
    void    mouseReleaseEvent(QMouseEvent *event);
    bool    winEvent(MSG *message, long *result);
};

#endif // AUDIENCEMAINWIDGET_H
