#ifndef PKROOMCONTROLLER_H
#define PKROOMCONTROLLER_H
#include <QObject>
#include <QList>
#include "Player/Player.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include "Modules/HttpDownload/HttpDownload.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "UI/PKRoomUI/PKRoomWidget.h"

#include "Modules/LyricParse/LyricParse.h"
#include "Modules/SingEngine/SingEngine.h"
#include "Modules/ResManager/ResManager.h"
#include "UI/ToastWidget/ToastWidget.h"
//#include "Modules/ExceptionController/ExceptionController.h"
#include "SingChangeErrorWidget/SingChangeErrorWidget.h"

#define DES_KEY  "ihou_key"

#define SINGER_PREPARE_SUCCESS   0
#define SINGER_PREPARE_FAILED    -1

#define DEBUG_PKROOM_CONTROLLER  1

#if DEBUG_PKROOM_CONTROLLER
#include <QDebug>
#define PKRoomControllerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define PKRoomControllerDebug(format,...)
#endif

//class StateController;
class ExceptionController;

class PKRoomController:public QObject
{
    Q_OBJECT
public:
    PKRoomController(QObject *parent = 0);
    void     setPkWidget(PKRoomWidget*);
    static void   SleepDefine(int sleepTime);
    void     DestoryDownLoadingWidget();

private:
    PkInfoManager           *pkInfo;
    YYNotificationCenter    *yync;
    NodeNotificationCenter  *nodeNC;
    StateController         *stateCtrl;
    ConfigHelper            *cfg;
    ResManager              *resManager;
    PKRoomWidget            *PkWidget;
    LyricParse              *lyricParse;
    SingEngine              *singEngine;

    ExceptionController     *exceptionController;

    bool             bFirstTimeStamp;
    bool             bCheered;
    bool             bHaveSendReady;
    bool             blResourceWidgetHaveShow;

    bool             blXmlParsed;
    bool             blSectionXmlParsed;
    bool             blPcmInit;

    static bool      boolSleep;

    enum ResourceStatus
    {
        RESFAILED = -1,
        RESSUCCESSED,
        RESDOWNLOADING
    };

    QTimer *playerListTimer;

private slots:
    void     handleStateControllerOnStatePKStart(QList<Player> playerList);
    void     handleStateControllerOnStateSingChange(int sectionIndex,QList<int>, int curSingerResStatus, quint32 myResStatus);
    void     handleStateControllerOnStateSingPrepare(int sectionIndex);
    void     handleStateControllerOnStateSingTimeOffset(float time);
    void     handleStateControllerOnStateScore(Score,Score);
    void     handleStateControllerOnStatePropConfirm(Prop prop);
    void     handleStateControllerOnStatePropActive(Player, Prop);
    void     handleStateControllerOnStatePKResult(QList<Score>, int, Prop, QList<int>);
    void     handleStateControllerOnStateErrorEvent(int);
    void     handleStateControllerOnStateUpdateHeat(int heat);
   // void     handleStateControllerOnStateSingReady(QList<SingReadyResult>);
	void     handleStateControllerOnStateSingTick();

    void     handleYYNotificationCenterOnSwitchSingerResponse(QList<Player> userList);
    void     handleYYNotificationCenterOnTimeStamp(Player player, float time);
    void     handleYYNotificationCenterOnUsePropResponse(Prop, int, QString, QString);
    void     handleYYNotificationCenterOnHostInfo(Player);

    void     handleSingEngineOnSingFinish(bool isError);
    void     handleSingEngineOnScore(Score sentenceScore, Score sumScore);
    void     handleSingEngineOnMusicTime(float musicTime);
    void     handleSingEngineOnError(SingEngine::error error);

    void     handleResouceErrorWidgetOnHide();

    void     handlePKPlaygerListWidgetHide();
    bool     ShowCurrentSingerResStatus(int curSingerResStatus, int sectionIndex);
    bool     PaseXmlAndSection();
    void     SingChangeProcess(int sectionIndex, QList<int> &recodeList);



/*
 *  ChangeSpeechProp Control
 */
private:
    int      changeSpeechPropMark;
    int      changeSpeechPropMarkForTimer;

    bool     isDownloadError;
    int      m_nCurrentIndex;

    SingChangeErrorWidget *recErrorWidget;
    SingChangeErrorWidget *recCurSingerErrorWidget;
private slots:
    void     handleThisOnChangeSpeechPropTimeout();

protected:
    void timerEvent(QTimerEvent *event);
signals:
    void onSingEngineInit(QString, QString, int, int, int, int);
    void onSingEngineUninit();
    void onSingEngineStart(int index);
    void onSingEngineStop();
    void onQuit();
//    void propClicked(int num, Player player);
};

#endif // PKROOMCONTROLLER_H
