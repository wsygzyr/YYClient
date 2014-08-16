#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>

#include <QMutex>
#include <QList>

#include "DataType/Prop/Prop.h"
#include "DataType/VictoryReward/VictoryReward.h"
#include <Windows.h>
#define CONFIG_HELPER_DEBUG   1
#if CONFIG_HELPER_DEBUG
#include <QDebug>
#define ConfigHelperDebug(format, ...) qDebug("%s, LINE: %d --->"format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define ConfigHelperDebug  /\
/
#endif

class ConfigHelper : public QObject
{
    Q_OBJECT

private:
    ConfigHelper();
    ~ConfigHelper();

/*
 *  Single instance
 */
private:
    static QAtomicPointer<ConfigHelper> instance;
    static QMutex instanceMutex;
public:
    static ConfigHelper *getInstance();
    static void destroy();

/*
 *  fetch application configurations
 */
private:
    /*
     * Data get from config.json
     */
    QString nodeServerIP;
    int nodeServerPort;
    QString statusServerIP;
    int statusServerPort;

    QString uploadPrefix;
    QString songSearchPrefix;
    QString mainPagePrefix;
    QString hotSongListPrefix;
    QString rewardListPrefix;
    QString feedBackPrefix;
    QString httpDownloadPrefix;
    QString singerListPrefix;
    QString singerSongListPrefix;
    QString progListPrefix;
    QString progSongListPrefix;
    QString songPathPrefix;
    QString downloadFileSavePath;

    QList<Prop> propList;
    QList<VictoryReward> vicList;
    /*
     * Data input from YY
     */
    int     yyClientPort;
    QString pluginPath;
    HWND    yyHwnd;
    QString logFileName;

    QString yyCheckGUID;

    bool    bUseDebugLogPath;

public:
    QString getNodeServerIP();
    int getNodeServerPort();

    QString getStatusServerIP();
    int getStatusServerPort();

    QString getUploadPrefix();
    QString getSongSearchPrefix() const;
    QString getMainPagePrefix() const;
    QString getHotSongListPrefix() const;
    QString getRewardListPrefix() const;
    QString getFeedBackPrefix() const;
    QString getHttpDownloadPrefix();
    QString getDownloadFileSavePath();

    QList<Prop> getPropList() const;
    Prop        getPropForID(int ID) const;

    QList<VictoryReward> getVictoryRewardList() const;
    VictoryReward        getVictoryRewardForID(int ID) const;

    QString getPluginPath() const;
    void setPluginPath(const QString &value);

    QString getLogFileName() const { return logFileName; }
    void setLogFileName(const QString &value) { logFileName = value;}

    int getYYClientPort() const;
    void setYYClientPort(int value);

    QString getYYCheckGUID() const;
    void setYYCheckGUID(const QString &value);


    bool getBUseDebugLogPath() const;
    void setBUseDebugLogPath(bool value);

    QString getSingerListPrefix() const;
    void setSingerListPrefix(const QString &value);
    QString getSingerSongListPrefix() const;
    void setSingerSongListPrefix(const QString &value);
    QString getProgListPrefix() const;
    void setProgListPrefix(const QString &value);
    QString getProgSongListPrefix() const;
    void setProgSongListPrefix(const QString &value);
    QString getSongPathPrefix() const;
    void setSongPathPrefix(const QString &value);

    HWND getYYHwnd() const;
    void setYYHwnd(const HWND &value);
};

#endif // CONFIG_H
