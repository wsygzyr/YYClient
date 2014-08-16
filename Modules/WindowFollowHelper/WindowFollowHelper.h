#ifndef WINDOWFOLLOWHELPER_H
#define WINDOWFOLLOWHELPER_H

#include <QObject>
#include <Windows.h>
#include <QWidget>
#include "Modules/ConfigHelper/ConfigHelper.h"

#define DEBUG_WINDOW_FOLLOW_HELPER  1

#if DEBUG_WINDOW_FOLLOW_HELPER
#include <QDebug>
#define WindowFollowHelperDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define WindowFollowHelperDebug(format,...)
#endif
class WindowFollowHelper : public QObject
{
    Q_OBJECT
public:
    explicit WindowFollowHelper(QObject *parent = 0);
    void setFollowWidget(QWidget*);
    void startFollow();
    void stopFollow();
    QPoint getYYParentPos();
    QPoint getSelfPos();
    QRect  getYYParentRect();
    void raiseUpYYWindow();
signals:

public slots:

private:
    QWidget *widget;
    HWND    PluginWindow;
    HWND    YYParentWindow;
    ConfigHelper *cfg;

    int     lastShowCmd;
    void   timerEvent(QTimerEvent *);
    bool   checkYYIsUnderPlugin();
};

#endif // WINDOWFOLLOWHELPER_H
