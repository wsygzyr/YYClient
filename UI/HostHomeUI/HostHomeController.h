#ifndef HOSTHOMECONTROLLER_H
#define HOSTHOMECONTROLLER_H

#include <QObject>
#include "HostHomeWidget.h"
#include "Modules/HttpGet/HttpGet.h"

#define DEBUG_HOST_HOME_CONTROLLER  1

#if DEBUG_HOST_HOME_CONTROLLER
#include <QDebug>
#define HostHomeControllerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define HostHomeControllerDebug(format,...)
#endif


class HostHomeController : public QObject
{
    Q_OBJECT
public:
    explicit HostHomeController(QObject *parent = 0);
    ~HostHomeController();
    void setHomeWidget(HostHomeWidget* widget);
signals:
    void startInvite();
    void onAdvice();
    void onHelpBtnClicked();
    void onCloseBtnClicked();
    void onMinBtnClicked();
public slots:
    void handleHttpGetOnMainPageInfo(bool, const MainPageInfo& mainPageInfo);
    void handleOnViewBtnClicked();
private:
    HostHomeWidget *homeWidget;
    HttpGet *httpGetMainPageInfo;
};

#endif // HOSTHOMECONTROLLER_H
