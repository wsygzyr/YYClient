#ifndef EXCEPTIONCONTROLLER_H
#define EXCEPTIONCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QTimer>

#define EXCEPTIONCONTROLLER_DEBUG  1

#if EXCEPTIONCONTROLLER_DEBUG
#include <QDebug>
#define ExceptionControllerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define ExceptionControllerDebug(format,...)
#endif

#include <QMutex>
#include <QThread>
#include <QList>
#include <QTimer>
#include "UI/ExceptionWidget/ExceptionWidget.h"
#include "Modules/HttpUpload/HttpUpLoad.h"

class AudienceMainWidget;
class HostMainWidget;

class ExceptionWidgetShowController : public QObject
{
    Q_OBJECT
public:
    explicit ExceptionWidgetShowController();

    void    setMainWidget(AudienceMainWidget* mainWidget);
    void    setMainWidget(HostMainWidget* mainWidget);
private:
    AudienceMainWidget              *audienceMainWidget;
    HostMainWidget                  *hostMainWidget;
public slots:
    void    handleExceptionControllerOnShowExceptionDisplayWidget(
                    QString errorString,
                    ExceptionWidget::eButtonType buttonType,
                    bool mainWidgetIsNeedClose);

    void    handleExceptionWidgetOnClose();
};

class ExceptionController : public QObject
{
    Q_OBJECT
private:
    explicit ExceptionController();
    ~ExceptionController();
public:
/*
 *  Single instance
 */
    static ExceptionController *getInstance();
    static void destroy();

    enum eExceptionNo
    {
        //1 networkError
            //001
        //2 resourceEror
        //3 MIC Error
        //4 Micphone Error
        //5 internal Error
        //host lost connection
        eHostLostConnectionExcep        = 1001,
        //player lost connection
        eAudienceLostConnectionExcep    = 1002,
        //resource download failed
        eResourceDownLoadFailedExcep    = 2001,
        //singChange failed
        eSingChangeFailedExcep          = 3001,
        //host initiative move singer down MIC
        eHostMoveSingerDownMICExcpep    = 3002,
        //pullup Micphone
        ePlayerPullUpMicphoneExcep      = 4001,
        //repetition login
        ePlayerRepetitionLoginExcep     = 5001,
        //room progress error
        eRoomProgressErrorExcep         = 5002
    };
private:

    HttpUpLoad                      *httpUpLoad;
    ExceptionWidgetShowController   *exceptionWidgetShowController;
public:
    friend class ExceptionWidgetShowController;
private:
    static QAtomicPointer<ExceptionController> instance;
    static QMutex instanceMutex;

    QThread                     *thread;
    QList<eExceptionNo>         waitToDealExceptionList;
    int                         timerID;

    QString                         errorString;
    ExceptionWidget::eButtonType    buttonType;
    bool                            mainWidgetIsNeedClose;
    bool                            isEmit;


protected:
    void timerEvent(QTimerEvent *);

private slots:
    void    handleThreadOnStarted();
    void    handleTimerOnShowTime();

signals:
    void    onSingChangeException();
    void    onShowExceptionDisplayWidget(QString errorString,
                                         ExceptionWidget::eButtonType buttonType,
                                         bool mainWidgetIsNeedClose);
public:
    void    setExceptionNo(eExceptionNo exceptionNo);

    void    setMainWidget(AudienceMainWidget* mainWidget);
    void    setMainWidget(HostMainWidget* mainWidget);
};

#endif // EXCEPTIONCONTROLLER_H
