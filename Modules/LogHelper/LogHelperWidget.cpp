#include "LogHelperWidget.h"
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QTextCodec>

QDateTime logTime;
ConfigHelper *cfg;
void customMessageHandler(QtMsgType type, const char *msg)
{   
    QString text;
    switch (type)
    {
    case QtDebugMsg:
        text = QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        text = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        text = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        text = QString("Fatal: %1").arg(msg);
        abort();
    }

    QDateTime dateTime = QDateTime::currentDateTime();

    QString logItem = QString("[" + dateTime.toString("yyyy-MM-dd") + " " +
                              dateTime.time().toString("hh:mm:ss.zzz") + "]\t");

    QString path;
    QString fileName;
    if(cfg->getBUseDebugLogPath())
    {
        path= "c:/iHouLog/";
        fileName = QString(path) +
                   QString(logTime.toString("yyyy-MM-dd-")) +
                   QString("%1").arg(logTime.time().hour()) + ("-") +
                   QString("%1").arg(logTime.time().minute()) + ("-") +
                   QString("%1").arg(logTime.time().second()) + (".log");
    }
    else
    {
        path = cfg->getPluginPath() + "log/";
        fileName = QString(path) + "iHouPlugin.log";
    }

    QFile logFile(fileName);
    logFile.open(QIODevice::WriteOnly | QIODevice::Append);
    logFile.write(logItem.toUtf8().data());
    logFile.write(QString(msg).toUtf8().data());
    logFile.write("\n\n");

    logFile.close();
}




LogHelperWidget::LogHelperWidget()
{
    cfg = ConfigHelper::getInstance();
    QString path;
    if(cfg->getBUseDebugLogPath())
        path= "c:/iHouLog";
    else
    {
        path = cfg->getPluginPath() + "log/";
    }
    QDir *dir = new QDir;
    if (!dir->exists(path))
    {
        dir->mkpath(path);
    }
    delete dir;

    QString fileName = QString(path) + "iHouPlugin.log";
    QFile logFile(fileName);
    if (logFile.exists())
    {
        //delete old
        logFile.remove();
    }

    //qInstallMsgHandler(customMessageHandler);
    logTime = QDateTime::currentDateTime();
}

LogHelperWidget::~LogHelperWidget()
{
    QString fileName = QString("c:\\iHouLog\\") +
                       QString(logTime.toString("yyyy-MM-dd-")) +
                       QString("%1").arg(logTime.time().hour()) + ("-") +
                       QString("%1").arg(logTime.time().minute()) + ("-") +
                       QString("%1").arg(logTime.time().second()) + (".log");
    QFile logFile("c:\\iHouLog\\iHouPlugin.log");
    if (logFile.exists())
    {
        //delete old
        logFile.rename(fileName);
    }
}

QAtomicPointer<LogHelperWidget> LogHelperWidget::instance = 0;
QMutex LogHelperWidget::instanceMutex;
LogHelperWidget *LogHelperWidget::getInstance()
{
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            instance = new LogHelperWidget();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void LogHelperWidget::destory()
{
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}



