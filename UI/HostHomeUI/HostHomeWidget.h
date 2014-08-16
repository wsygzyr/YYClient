#ifndef HOSTHOMEWIDGET_H
#define HOSTHOMEWIDGET_H

#include <QWidget>
#include "DataType/MainPageInfo/MainPageInfo.h"

#define DEBUG_HOST_HOME_WIDGET  1

#if DEBUG_HOST_HOME_WIDGET
#include <QDebug>
#define HostHomeWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define HostHomeWidgetDebug(format,...)
#endif

namespace Ui {
class HostHomeWidget;
}
class HostHomeController;
class HostHomeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HostHomeWidget(QWidget *parent = 0);
    ~HostHomeWidget();
    void updateWidgetInfo(const MainPageInfo& mainPageInfo);

    void startLoading();
    void stopLoading();
private:
    void reset();
private:
    Ui::HostHomeWidget *ui;
    friend class HostHomeController;
    QStringList homeDataList;
    int timerId;
    int time;

    QTimer  *loadingTimer;
    int     loadingPixmapIndex;
private:
    void timerEvent(QTimerEvent *e);

private slots:
    void handleLoadingTimerOnTimeOut();
};

#endif // HOSTHOMEWIDGET_H
