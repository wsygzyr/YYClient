#ifndef LOGHELPERWIDGET_H
#define LOGHELPERWIDGET_H

#include <QMutex>
#include <QTextBrowser>
#include <QFile>
#include <QThread>
#include "Modules/ConfigHelper/ConfigHelper.h"
class LogHelperWidget : public QObject
{
    Q_OBJECT

private:
    LogHelperWidget();
    ~LogHelperWidget();

private:
    static QAtomicPointer<LogHelperWidget> instance;
    static QMutex instanceMutex;



public:
    static LogHelperWidget* getInstance();
    static void destory();


};


#endif // LOGHELPERWIDGET_H
