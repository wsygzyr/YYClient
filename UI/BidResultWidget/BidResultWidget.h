#ifndef BIDRESULTWIDGET_H
#define BIDRESULTWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QList>

#include "DataType/Player/Player.h"
#include "Modules/HttpDownload/HttpDownload.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/StateController/StateController.h"
#define DEBUG_BID_RESULT_WIDGET  1

#if DEBUG_BID_RESULT_WIDGET
#include <QDebug>
#define BidResultWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define BidResultWidgetDebug(format,...)
#endif

#define COUNT_DOWN_TIME    3000

#define SINGER_NUM_REC_TIME  90
#define DOWNLOAD_RES_TIME  10

namespace Ui {
class BidResultWidget;
}

class BidResultController;
class BidResultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BidResultWidget(QWidget *parent = 0);
    ~BidResultWidget();
    void setBidResultWidgetInfo();
//    void setBidResultWidgetInfo(QList<Player> playerList,QList<Prop> propList);
    void setProgressBarValue(int value);
    void killTimeID();

private:
    Ui::BidResultWidget *ui;
    PkInfoManager *pkInfo;
    StateController *stateCon;
    int timeID;
    int time;
    int singerNumTime;
    int downloadResTime;
    qint64 pkSingerNumRec;
    qint64 pkSingerNum;
    bool downloadResOk;
    int progressBarLabelMoveCount;
    QTimer *tipsTimer;
    QList<QString> tipsList;
    int tipsCount;

private slots:
    void handleThisOnStateBidSettlement(int);
    void handleTipsTimerOnTimeOut();

protected:
    void timerEvent(QTimerEvent* event);

    friend class BidResultController;
};

#endif // BIDRESULTWIDGET_H
