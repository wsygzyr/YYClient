#ifndef AUDIENCEBIDWIDGET_H
#define AUDIENCEBIDWIDGET_H

#include <QWidget>
#include <QStringListModel>
#include "DataType/Player/Player.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include <QMouseEvent>
#include "Modules/StateController/StateController.h"
#include <QThread>
#define DEBUG_AUDIENCE_BID_WIDGET  1

#if DEBUG_AUDIENCE_BID_WIDGET
#include <QDebug>
#define AudienceBidWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define AudienceBidWidgetDebug(format,...)
#endif

//class StateController;

namespace Ui {
class AudienceBidWidget;
}

class AudienceBidWidget : public QWidget
{
    Q_OBJECT
    typedef enum
    {
        BID_START,
        BID_MAKE_UP
    }BidStatus;
public:
    explicit AudienceBidWidget(QWidget *parent = 0);
    ~AudienceBidWidget();
    void setUIControlsInfo(const QString& songName,
                           int playerNum);

private:
    Ui::AudienceBidWidget *ui;
    QStringListModel *AudienceBidingPriceListModel;
    QStringList *AudienceBidingInfoList;
    NodeNotificationCenter *NodeNC;
    YYNotificationCenter   *yync;
    PkInfoManager          *pkInfo;
    ConfigHelper *cfg;
    BidStatus              status;
    QTimer                 *aniTimer;
    StateController        *stateCtrl;

	QTimer                 *balanceTimer;
    int  biderNum;
    int  currentUsed;      //for plusBtn
    int  totalUsed;       //prop used by me
    int  leastUsed;      //leastProp Count from host
    int  lastCandidateUsed; //last candidate count
    int  candidateUsed;      //candidate count

    int  timerId;
    int  countDownSecond;

    ResManager *resManager;

    void timerEvent(QTimerEvent *);
    void configUI();
    void mousePressEvent(QMouseEvent *);
    bool eventFilter(QObject *, QEvent *);
    void changeNoticeText();

public slots:
    void handleStateControllerOnStateBidInfo(int, Prop, Prop expectIncome);
    void handleStateControllerOnStateBidStop(int);
    void handleStateControllerOnStateBidConfirm(Prop prop);

    void handleYYNotificationCenterOnUsePropResponse(Prop, int, QString, QString);
    void handleYYNotificationCenterOnUserMoneyResponse(Player , Prop , int);

    void handleLineEditOnTextChanged(QString);
    void startBid();
    void endBid();
private slots:

    void handleAniTimerOnTimeOut();
    void handleBalanceTimerOnTimeOut();

    void on_plusBtn_clicked();
    void on_addBidBtn_clicked();
    void on_bidBtn_clicked();
    void on_minusBtn_clicked();
	void on_okButton_clicked();
	void on_cancelButton_clicked();
	
signals:
    void onHelpBtnClicked();
    void onCloseBtnClicked();
    void onMinBtnClicked();
private:
    void resetBidBtn();
    void changeBidBtnState();
    bool checkTotalUsed();
    void sendCheckUserBalance();
};

#endif // AUDIENCEBIDWIDGET_H
