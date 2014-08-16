#ifndef HOSTBIDCONTROLLER_H
#define HOSTBIDCONTROLLER_H

#include <QObject>
#include "UI/HostBidUI/HostBidWidget.h"
#include "DataType/Player/Player.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include <Modules/NodeNotificationCenter/NodeNotificationCenter.h>
#include "Modules/ResManager/ResManager.h"
#include "Modules/StateController/StateController.h"
#define DEBUG_HOST_BID_CONTROLLER  1

#if DEBUG_HOST_BID_CONTROLLER
#include <QDebug>
#define HostBidControllerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define HostBidControllerDebug(format,...)
#endif

//class StateController;

class HostBidController : public QObject
{
    Q_OBJECT
public:
    explicit HostBidController(QObject *parent = 0);
    void     setHostBidWidget(HostBidWidget*);
    void     startBid(int bidCount);
signals:
    void     onHelpBtnClicked();
    void     onCloseBtnClicked();
    void     onMinBtnClicked();
private slots:
    void     handleStateControllerOnUpdateBidInfo(int biderNumber, Prop leastProp, Prop expectIncome );
    void     handleBidWidgetOnHostStopBid();

private:
    HostBidWidget  *bidWidget;
    NodeNotificationCenter *NodeNC;
    ResManager     *resManager;
    PkInfoManager  *pkInfo;
    StateController *stateCtrl;
};

#endif // HOSTBIDCONTROLLER_H
