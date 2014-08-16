#ifndef HOSTBIDWIDGET_H
#define HOSTBIDWIDGET_H

#include <QWidget>
#include <Modules/PkInfoManager/PkInfoManager.h>
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"

#define DEBUG_HOST_BID_WIDGET  1

#if DEBUG_HOST_BID_WIDGET
#include <QDebug>
#define HostBidWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define HostBidWidgetDebug(format,...)
#endif

class HostBidController;
namespace Ui {
class HostBidWidget;
}

class HostBidWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HostBidWidget(QWidget *parent = 0);
    ~HostBidWidget();

    void updateBidInfo(int biderNumber, Prop leastProp, Prop expectIncome);
    void reset();
    void startBid(int bidCount);
private:
    void configUI();
private:
    Ui::HostBidWidget *ui;
    int timerId;
    QTimer *timer;
    PkInfoManager *pkInfo;
    NodeNotificationCenter *nnc;
    int    expectIncome;
    friend class HostBidController;
    void timerEvent(QTimerEvent *);
private slots:
    void on_StartTimeBtn_clicked();
    void handleTimerOnTimeOut();
signals:
    void onHostStopBid();



};

#endif // HOSTBIDWIDGET_H
