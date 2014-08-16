#include "ExceptionController.h"
#include "AudienceMainWidget.h"
#include "HostMainWidget.h"

#define HOST_LOST_CONNECTION_EXCEP_TEXT         "主播离开了游戏！"
#define AUDIENCE_LOST_CONNECTION_EXCEP_TEXT     "您的网络开小差了！"
#define RESOURCE_DOWNLOAD_FAILED_EXCEP_TEXT     "您的资源下载失败，无法继续演唱！"
#define SINGCHANGE_FAILED_EXCEP_TEXT            "您切麦失败了，无法继续演唱！"
#define HOST_MOVE_SINGER_DWON_MIC_EXCEP_TEXT    "您被主播下麦了，无法继续演唱！"
#define PLAYER_PULLUP_MICPHONE_EXCEP_TEXT       "你的麦克风出现了问题，无法继续演唱！"
#define PLAYER_REPETITION_LOGIN_EXCEP_TEXT      "服务器出错！"
#define ROOM_PROGRESS_ERROR_EXCEP_TEXT          "服务器出错！"

ExceptionController::ExceptionController()
{
    thread = new QThread;
    thread->setObjectName("ExceptionThread");
    this->moveToThread(thread);
    connect(thread, SIGNAL(started()), this, SLOT(handleThreadOnStarted()));
    thread->start();
    waitToDealExceptionList.clear();

    exceptionWidgetShowController = new ExceptionWidgetShowController;
    connect(this, SIGNAL(onShowExceptionDisplayWidget(QString,ExceptionWidget::eButtonType,bool)),
            exceptionWidgetShowController, SLOT(handleExceptionControllerOnShowExceptionDisplayWidget(QString,ExceptionWidget::eButtonType,bool)));
    ExceptionControllerDebug("create ExceptionController successed!");
}

ExceptionController::~ExceptionController()
{
    thread->deleteLater();
    httpUpLoad->deleteLater();

    ExceptionControllerDebug("delete ExceptionController successed!");
}

QAtomicPointer<ExceptionController> ExceptionController::instance;
QMutex ExceptionController::instanceMutex;

ExceptionController *ExceptionController::getInstance()
{
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            ExceptionControllerDebug("create instance!");
            instance = new ExceptionController();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void ExceptionController::destroy()
{
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}

void ExceptionController::timerEvent(QTimerEvent *)
{
    if (waitToDealExceptionList.isEmpty())
    {
        return;
    }
    if (! isEmit)
    {
        return;
    }
    if (! instanceMutex.tryLock())
    {
        return;
    }
    ExceptionControllerDebug("ExceptionController timerEvent");
    eExceptionNo exceptionNo = waitToDealExceptionList.takeFirst();
    ExceptionControllerDebug("current Exception is %d", exceptionNo);
    //deal exception
    //step 1. tell stateMachine change state
    if (exceptionNo == eSingChangeFailedExcep ||
            exceptionNo == eResourceDownLoadFailedExcep ||
            exceptionNo == eHostMoveSingerDownMICExcpep ||
            exceptionNo == ePlayerPullUpMicphoneExcep)
    {
        emit onSingChangeException();
        ExceptionControllerDebug("ExceptionController tell change state");
    }

    //step 2. show exception Widget
    mainWidgetIsNeedClose = false;
    //different exception has different handle
    switch (exceptionNo)
    {
    case eHostLostConnectionExcep:
        errorString = HOST_LOST_CONNECTION_EXCEP_TEXT;
        buttonType = ExceptionWidget::eLeaveButton;
        mainWidgetIsNeedClose = true;
        break;
    case eAudienceLostConnectionExcep:
        errorString = AUDIENCE_LOST_CONNECTION_EXCEP_TEXT;
        buttonType = ExceptionWidget::eLeaveButton;
        mainWidgetIsNeedClose = true;
        break;
    case eResourceDownLoadFailedExcep:
        errorString = RESOURCE_DOWNLOAD_FAILED_EXCEP_TEXT;
        buttonType = ExceptionWidget::eKnowButton;
        break;
    case eSingChangeFailedExcep:
        errorString = SINGCHANGE_FAILED_EXCEP_TEXT;
        buttonType = ExceptionWidget::eKnowButton;
        break;
    case eHostMoveSingerDownMICExcpep:
        errorString = HOST_MOVE_SINGER_DWON_MIC_EXCEP_TEXT;
        buttonType = ExceptionWidget::eKnowButton;
        break;
    case ePlayerPullUpMicphoneExcep:
        errorString = PLAYER_PULLUP_MICPHONE_EXCEP_TEXT;
        buttonType = ExceptionWidget::eKnowButton;
        break;
    case ePlayerRepetitionLoginExcep:
        errorString = PLAYER_REPETITION_LOGIN_EXCEP_TEXT;
        buttonType = ExceptionWidget::eLeaveButton;
        mainWidgetIsNeedClose = true;
        break;
    case eRoomProgressErrorExcep:
        errorString = ROOM_PROGRESS_ERROR_EXCEP_TEXT;
        buttonType = ExceptionWidget::eLeaveButton;
        mainWidgetIsNeedClose = true;
        break;
    }

    isEmit = false;
    QTimer::singleShot(1000, this, SLOT(handleTimerOnShowTime()));

    //step 3. upload error log
    httpUpLoad->singleUpload(exceptionNo);
    httpUpLoad->errorUpload(exceptionNo);
    ExceptionControllerDebug("upload error log over");
    instanceMutex.unlock();
}

void ExceptionController::handleThreadOnStarted()
{
    timerID = startTimer(100);
    httpUpLoad = new HttpUpLoad;
    isEmit = true;
}

void ExceptionController::handleTimerOnShowTime()
{
    ExceptionControllerDebug("ExceptionController handleTimerOnShowTime");
    emit onShowExceptionDisplayWidget(errorString, buttonType, mainWidgetIsNeedClose);
    isEmit = true;
}

void ExceptionController::setExceptionNo(ExceptionController::eExceptionNo exceptionNo)
{
    instanceMutex.lock();
    this->waitToDealExceptionList.append(exceptionNo);
    instanceMutex.unlock();
}

void ExceptionController::setMainWidget(AudienceMainWidget *mainWidget)
{
    exceptionWidgetShowController->setMainWidget(mainWidget);
}

void ExceptionController::setMainWidget(HostMainWidget *mainWidget)
{
    exceptionWidgetShowController->setMainWidget(mainWidget);
}

ExceptionWidgetShowController::ExceptionWidgetShowController()
{
    audienceMainWidget = NULL;
    hostMainWidget = NULL;
}

void ExceptionWidgetShowController::setMainWidget(AudienceMainWidget *mainWidget)
{
    audienceMainWidget = mainWidget;
}

void ExceptionWidgetShowController::setMainWidget(HostMainWidget *mainWidget)
{
    hostMainWidget = mainWidget;
}

void ExceptionWidgetShowController::handleExceptionControllerOnShowExceptionDisplayWidget(QString errorString, ExceptionWidget::eButtonType buttonType, bool mainWidgetIsNeedClose)
{
    ExceptionControllerDebug("ExceptionWidgetShowController handleExceptionControllerOnShowExceptionDisplayWidget");
    QWidget *parentWidget;
    if (audienceMainWidget != NULL)
    {
        parentWidget = audienceMainWidget->currentWidget();
        if (mainWidgetIsNeedClose)
        {
            QTimer::singleShot(4000, audienceMainWidget, SLOT(hide()));
        }
    }
    else if (hostMainWidget != NULL)
    {
        parentWidget = hostMainWidget->currentWidget();
        if (mainWidgetIsNeedClose)
        {
            QTimer::singleShot(4000, hostMainWidget, SLOT(close()));
        }
    }
    ExceptionWidget *exceptionWidget = new ExceptionWidget(errorString, buttonType);
    connect(exceptionWidget, SIGNAL(onClose()), this, SLOT(handleExceptionWidgetOnClose()));
    exceptionWidget->setParent(parentWidget);
    exceptionWidget->show();
}

void ExceptionWidgetShowController::handleExceptionWidgetOnClose()
{
    ExceptionControllerDebug("ExceptionWidgetShowController handleExceptionWidgetOnClose");
    if (audienceMainWidget != NULL)
    {
        audienceMainWidget->hide();
    }
    else if (hostMainWidget != NULL)
    {
        hostMainWidget->close();
    }
}
