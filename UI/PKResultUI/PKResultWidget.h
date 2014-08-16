#ifndef PKRESULTWIDGET_H
#define PKRESULTWIDGET_H

#include <QWidget>
#include "DataType/Player/Player.h"
#include "UI/TableWidget/TableWidget.h"
#include "RotateWidget.h"
#include "Modules/TempVarCollector/TempVarCollector.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/MMTimer/MMTimer.h"
#include <QTimer>

#define DEBUG_PK_RESULT_WIDGET  1

#if DEBUG_PK_RESULT_WIDGET
#include <QDebug>
#define PKResultWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define PKResultWidgetDebug(format,...)
#endif

#define AUTO_SELECT_REWARD_TIME   60000

namespace Ui {
class PKResultWidget;
}
class PKResultController;
class PKResultWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PKResultWidget(QWidget *parent = 0);
    ~PKResultWidget();
    void     updateResult(QList<Player>, int heat, Prop propIncome);

    void     updateSelectedVictoryReward(VictoryReward);
    void     startAni(QString path);
private:
    void reset();
private:
    Ui::PKResultWidget *ui;
    friend class PKResultController;
    PkInfoManager *info;
    Player   firstPricePlayer;
    RotateWidget *RewardBackGroundMask;
    QTimer *AudienceQuitTimer;

    int timeID;
    int time;
    int quitAudienceTimerId;
    void timerEvent(QTimerEvent *);
    void AutoSelectReward();
    void     updateRankList(const QList<Player>& playerList);
    void     updateRewardAndTip(const QList<Player>& playerList, int heat, Prop propIncome);
    void     setRewardBg(bool isLight);
private slots:
    void     handleRewardTableWidgetClicked(int row, int column);
signals:
    void sendVictory(VictoryReward vic);
    void audienceQuit();
};

#endif // PKRESULTWIDGET_H
