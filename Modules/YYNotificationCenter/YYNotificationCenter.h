
#ifndef YYNOTIFICATIONCENTER_H
#define YYNOTIFICATIONCENTER_H

#include <QtCore>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QList>
#include <QRect>

#include "YYTCPPipe.h"

#include "DataType/Player/Player.h"
#include "DataType/Prop/Prop.h"
#include "PkInfoManager/PkInfoManager.h"

#define YYNOTIFICATION_DEBUG   1
#if YYNOTIFICATION_DEBUG
#include <QDebug>
#define YYNotificationCenterDebug(format, ...) qDebug("%s, LINE: %d --->"format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define YYNotificationCenterDebug  /\
/
#endif

class YYNotificationCenterMsg;
class YYNotificationCenter : public QObject
{
    Q_OBJECT

    YYNotificationCenter();
    ~YYNotificationCenter();

/*
 *  singleton
 */

private:
    static QAtomicPointer<YYNotificationCenter> instance;
    static QMutex instanceMutex;
public:
    static YYNotificationCenter *getInstance();
    static void destory();

public:
    bool start(int port);
    void stop();
    enum error
    {
        sendMessageFailed,
        recvMessageFailed,
        pipeBroken
    };



public slots:
    void sendCheckConnection(QString uid);
    void sendHostCreateGame();
    void sendAudienceJoinGame();
    void sendGameOver();
    void sendGetUserPicRequest(Player player);
    void sendUserMoneyReq(Player player, Prop prop);
    void sendUsePropRequest(Prop prop);
    void sendSetSingerList(QList<Player>);
    void sendSwitchSinger(Player curPlayer , Player nextPlayer);
    void sendPCMBegin(int sampleRate , int channels, int bits);
    void sendPCMData(QByteArray pcmData);
    void sendPCMEnd();
    void sendPluginInfo(QString company , QString name ,QString version);
    void sendHostPKInfo(P2Y_PKInfo *);
    void sendAudiencePKInfo(const P2Y_PKInfo *);
private slots:
    void handleThreadOnStarted();
    void handleThreadOnFinished();
    void handlePipeOnDisconnected();
    void hanldePipeOnRecvMsg(QString , QByteArray);

private:
    void parseMsgOnShow(QByteArray &msg);
    void parseMsgOnMove(QByteArray &msg);
    void parseMsgOnQuit();
    void parseMsgOnHostInfo(QByteArray &msg);
    void parseMsgOnGetUserPicResponse(QByteArray &msg);
    void parseMsgOnUsePropResponse(QByteArray &msg);
    void parseMsgOnUserMoneyResponse(QByteArray &msg);
    void parseMsgOnSwitchSinger(QByteArray &msg);
    void parseMsgOnTimeStamp(QByteArray &msg);
    void parseMsgOnSetVolume(QByteArray &msg);
    void parseMsgOnGetPluginInfo();
    void parseMicDown(QByteArray msg);

signals:
    void onError(YYNotificationCenter::error error);

    void onShow(bool);
    void onMove(QRect);
    void onQuit();
    void onHostInfo(Player);
    void onGetUserPicResponse(Player, int retcode);
    void onUsePropResponse(Prop, int retcode , QString ,QString);
    void onUserMoneyResponse(Player , Prop , int );
    void onSwitchSingerResponse(QList<Player>);
    void onTimeStamp(Player , float time);
    void onSetVolume(int left , int right , bool bMute);
    void onGetPluginInfo();
    void onMicTakenDown(QList<Player>);

protected:
    void timerEvent(QTimerEvent *);
private:
    YYTCPPipe *pipe;
    int port;
    enum ePipeState
    {
        PipeUnconnected = 0,
        PipeConnecting,
        PipeConnected,
        PipeError
    };
    QThread *thread;
    // add by xianli, volatile may solve the "while(1)" bug in method "start"
    volatile ePipeState pipeState;
    int timerId;
    QLinkedList<YYNotificationCenterMsg> *sendMsgList;

};

#endif // YYNOTIFICATIONCENTER_H
