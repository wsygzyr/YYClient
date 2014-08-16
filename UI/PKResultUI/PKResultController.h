#ifndef PKRESULTCONTROLLER_H
#define PKRESULTCONTROLLER_H

#include <QObject>
#include "UI/PKResultUI/PKResultWidget.h"
#include "DataType/Player/Player.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/StateController/StateController.h"

#define DEBUG_PKRESULT_CONTROLLER  1

#if DEBUG_PKRESULT_CONTROLLER
#include <QDebug>
#define PKResultControllerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define PKResultControllerDebug(format,...)
#endif

//class StateController;

class PKResultController : public QObject
{
    Q_OBJECT
public:
    explicit PKResultController(QObject *parent = 0);
    void     setPkResultWidget(PKResultWidget*);

    void     startAni();
signals:
    void     onSelectVictoryReward(VictoryReward);
private:
    PKResultWidget *pkResultWidget;
    NodeNotificationCenter *NodeNC;
    ConfigHelper *cfg;
    PkInfoManager *info;
    StateController *stateCtrl;

signals:
    void     onQuit();
    void     onAdvice();
    void     onCloseBtnClicked();
    void     onMinBtnClicked();
private slots:
    void     handleStateControllerOnStatePKResult(QList<Score>, int, Prop, QList<int>);
    void     handleStateControllerOnStateSelectReward(VictoryReward);
    void     handlePKResultWidgetOnQuit();
    void     handlePKResultWidgetOnSendVic(VictoryReward);

};

#endif // PKRESULTCONTROLLER_H
