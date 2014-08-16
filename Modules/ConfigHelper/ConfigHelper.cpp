#include "ConfigHelper.h"

#include <QFile>
#include <Modules/JsonPrase/cJSON.h>
#include <QHostInfo>

ConfigHelper::ConfigHelper()
{
    QFile file("c:/config/config.json");
    if(!file.exists())
    {
        file.setFileName(":/config/config.json");
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        ConfigHelperDebug("cannot open config file!");
        return;
    }
    QByteArray content = file.readAll();
    QString strContent = QString::fromUtf8(content.data());
    file.close();


    ConfigHelperDebug("config file content is: %s", strContent.toUtf8().data());

    cJSON *jsConfig = cJSON_Parse(strContent.toUtf8().data());
    {
        ConfigHelperDebug("jsConfig is: @%d", jsConfig);
        cJSON *jsNodeServer = cJSON_GetObjectItem(jsConfig, "NodeServer");
        {
            QString nodeServerDomain = QString::fromUtf8(cJSON_GetObjectItem(jsNodeServer, "domain")->valuestring);
            QHostInfo info =  QHostInfo::fromName(nodeServerDomain);
            if(info.error() != QHostInfo::NoError || info.addresses().isEmpty())
            {
                nodeServerIP = QString::fromUtf8(cJSON_GetObjectItem(jsNodeServer, "IP")->valuestring);
                ConfigHelperDebug("from DNS nodeServerIP is: %s", nodeServerIP.toUtf8().data());
            }
            else
            {
                nodeServerIP = info.addresses().at(0).toString();
                ConfigHelperDebug("from config nodeServerIP is: %s", nodeServerIP.toUtf8().data());
            }


            nodeServerPort = cJSON_GetObjectItem(jsNodeServer, "port")->valueint;
            ConfigHelperDebug("nodeServerPort is: %d", nodeServerPort);
        }

        cJSON *jsStatusServer = cJSON_GetObjectItem(jsConfig, "StatusServer");
        {
            QString statusServerDomain = QString::fromUtf8(cJSON_GetObjectItem(jsStatusServer, "domain")->valuestring);
            QHostInfo info =  QHostInfo::fromName(statusServerDomain);
            if(info.error() != QHostInfo::NoError || info.addresses().isEmpty())
            {
                statusServerIP = QString::fromUtf8(cJSON_GetObjectItem(jsStatusServer, "IP")->valuestring);
                ConfigHelperDebug("from DNS statusServerIP is: %s", statusServerIP.toUtf8().data());
            }
            else
            {
                statusServerIP = info.addresses().at(0).toString();
                ConfigHelperDebug("from config statusServerIP is: %s", statusServerIP.toUtf8().data());
            }


            statusServerPort = cJSON_GetObjectItem(jsStatusServer, "port")->valueint;
            ConfigHelperDebug("statusServerPort is: %d", statusServerPort);
        }

        cJSON *jsHTTP = cJSON_GetObjectItem(jsConfig, "HTTP");
        {
            QString interfaceDomain = QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "InterfaceDomain")->valuestring);
            ConfigHelperDebug("interfaceDomain is: %s", interfaceDomain.toUtf8().data());

            songSearchPrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "songsearchprefix")->valuestring);
            ConfigHelperDebug("songSearchPrefix is: %s", songSearchPrefix.toUtf8().data());

            mainPagePrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "mainpageprefix")->valuestring);
            ConfigHelperDebug("mainpageprefix is: %s", mainPagePrefix.toUtf8().data());

            hotSongListPrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "hotsonglistprefix")->valuestring);
            ConfigHelperDebug("hotSongListPrefix is: %s", hotSongListPrefix.toUtf8().data());

            rewardListPrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "rewardlistprefix")->valuestring);
            ConfigHelperDebug("rewardListPrefix is: %s", rewardListPrefix.toUtf8().data());

            feedBackPrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "feedbackprefix")->valuestring);
            ConfigHelperDebug("feedbackPrefix is %s" , feedBackPrefix.toUtf8().data());

            httpDownloadPrefix = QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "downloadPrefix")->valuestring);
            ConfigHelperDebug("httpDownloadPrefix is: %s", httpDownloadPrefix.toUtf8().data());

            uploadPrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "uploadprefix")->valuestring);
            ConfigHelperDebug("uploadprefix is: %s", uploadPrefix.toUtf8().data());

            singerListPrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "singerlistprefix")->valuestring);
            ConfigHelperDebug("singerListPrefix is: %s", singerListPrefix.toUtf8().data());

            singerSongListPrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "singersonglistprefix")->valuestring);
            ConfigHelperDebug("singerSongListPrefix is: %s", singerSongListPrefix.toUtf8().data());

            progListPrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "proglistprefix")->valuestring);
            ConfigHelperDebug("progListPrefix is: %s", progListPrefix.toUtf8().data());

            progSongListPrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "progsonglistprefix")->valuestring);
            ConfigHelperDebug("progSongListPrefix is: %s", progSongListPrefix.toUtf8().data());

            songPathPrefix = interfaceDomain + QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "songpathprefix")->valuestring);
            ConfigHelperDebug("songPathPrefix is: %s", songPathPrefix.toUtf8().data());

            downloadFileSavePath = QString::fromUtf8(cJSON_GetObjectItem(jsHTTP, "downloadFileSavePath")->valuestring);
            ConfigHelperDebug("downloadFileSavePath is: %s", downloadFileSavePath.toUtf8().data());
        }

        cJSON *jsPropList = cJSON_GetObjectItem(jsConfig, "PropList");
        {
            for (int i = 0; i < cJSON_GetArraySize(jsPropList); i++)
            {
                cJSON *jsProp = cJSON_GetArrayItem(jsPropList, i);
                {
                    Prop prop;
                    prop.setID(cJSON_GetObjectItem(jsProp, "ID")->valueint);
                    prop.setName(QString::fromUtf8(cJSON_GetObjectItem(jsProp, "name")->valuestring));
                    prop.setPriceDesp(QString::fromUtf8(cJSON_GetObjectItem(jsProp, "priceDesp")->valuestring));
                    prop.setFuncDesp(QString::fromUtf8(cJSON_GetObjectItem(jsProp, "funcDesp")->valuestring));
                    prop.setCountdownTime(cJSON_GetObjectItem(jsProp, "countdown")->valueint);
                    prop.setIsComplexAni(cJSON_GetObjectItem(jsProp , "isComplexAni")->type == cJSON_True);
                    prop.setAniLoopCount(cJSON_GetObjectItem(jsProp , "loopCount")->valueint);

                    ConfigHelperDebug("prop%d.getID() is: %d", i, prop.getID());
                    ConfigHelperDebug("prop%d.getName() is: %s", i, prop.getName().toUtf8().data());
                    ConfigHelperDebug("prop%d.getPriceDesp() is: %s", i, prop.getPriceDesp().toUtf8().data());
                    ConfigHelperDebug("prop%d.getFuncDesp() is: %s", i, prop.getFuncDesp().toUtf8().data());
                    ConfigHelperDebug("prop%d.getCountDownTime() is: %d", i, prop.getCountdownTime());

                    propList.append(prop);
                }
            }
        }

        cJSON *jsVicList = cJSON_GetObjectItem(jsConfig , "RewardList");
        {
            for (int i = 0; i < cJSON_GetArraySize(jsVicList); i++)
            {
                cJSON *jsReward = cJSON_GetArrayItem(jsVicList, i);
                {
                    VictoryReward vic;
                    vic.setID(cJSON_GetObjectItem(jsReward, "ID")->valueint);
                    vic.setName(QString::fromUtf8(cJSON_GetObjectItem(jsReward, "name")->valuestring));
                    vic.setDesp(QString::fromUtf8(cJSON_GetObjectItem(jsReward, "desp")->valuestring));

                    ConfigHelperDebug("vic%d.getID() is: %d", i, vic.getID());
                    ConfigHelperDebug("vic%d.getName() is: %s", i, vic.getName().toUtf8().data());
                    ConfigHelperDebug("vic%d.getDesp() is: %s", i, vic.getDesp().toUtf8().data());

                    vicList.append(vic);
                }
            }
        }
        cJSON *jsLog = cJSON_GetObjectItem(jsConfig, "Log");
        {
            bUseDebugLogPath = cJSON_GetObjectItem(jsLog , "USE_DEBUG_PATH")->type == cJSON_True;
            ConfigHelperDebug("bUseDebugLogPath is: %s", bUseDebugLogPath ? "true" : "false");
        }

    }
    cJSON_Delete(jsConfig);
}

ConfigHelper::~ConfigHelper()
{

}

QAtomicPointer<ConfigHelper> ConfigHelper::instance;
QMutex ConfigHelper::instanceMutex;
ConfigHelper* ConfigHelper::getInstance()
{
    //    ConfigHelperDebug("getInstance!");
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            ConfigHelperDebug("create instance!");
            instance = new ConfigHelper();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void ConfigHelper::destroy()
{
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}
HWND ConfigHelper::getYYHwnd() const
{
    return yyHwnd;
}

void ConfigHelper::setYYHwnd(const HWND &value)
{
    yyHwnd = value;
}

QString ConfigHelper::getSongPathPrefix() const
{
    return songPathPrefix;
}

void ConfigHelper::setSongPathPrefix(const QString &value)
{
    songPathPrefix = value;
}

QString ConfigHelper::getProgSongListPrefix() const
{
    return progSongListPrefix;
}

void ConfigHelper::setProgSongListPrefix(const QString &value)
{
    progSongListPrefix = value;
}

QString ConfigHelper::getProgListPrefix() const
{
    return progListPrefix;
}

void ConfigHelper::setProgListPrefix(const QString &value)
{
    progListPrefix = value;
}

QString ConfigHelper::getSingerSongListPrefix() const
{
    return singerSongListPrefix;
}

void ConfigHelper::setSingerSongListPrefix(const QString &value)
{
    singerSongListPrefix = value;
}

QString ConfigHelper::getSingerListPrefix() const
{
    return singerListPrefix;
}

void ConfigHelper::setSingerListPrefix(const QString &value)
{
    singerListPrefix = value;
}

bool ConfigHelper::getBUseDebugLogPath() const
{
    return bUseDebugLogPath;
}

void ConfigHelper::setBUseDebugLogPath(bool value)
{
    bUseDebugLogPath = value;
}

QString ConfigHelper::getFeedBackPrefix() const
{
    return feedBackPrefix;
}




QList<VictoryReward> ConfigHelper::getVictoryRewardList() const
{
    return vicList;
}

VictoryReward ConfigHelper::getVictoryRewardForID(int ID) const
{
    VictoryReward vr;
    vr.setID(-1);
    for (int i = 0; i < vicList.size(); i++)
    {
        vr = vicList.at(i);
        if (vr.getID() == ID)
        {
            ConfigHelperDebug("find VR for ID: %d", ID);
            break;
        }
    }
    if (vr.getID() == -1)
    {
        ConfigHelperDebug("cannot find VR for ID!");
    }
    return vr;
}


QString ConfigHelper::getRewardListPrefix() const
{
    return rewardListPrefix;
}


QString ConfigHelper::getHotSongListPrefix() const
{
    return hotSongListPrefix;
}


QString ConfigHelper::getMainPagePrefix() const
{
    return mainPagePrefix;
}

QString ConfigHelper::getSongSearchPrefix() const
{
    return songSearchPrefix;
}



QList<Prop> ConfigHelper::getPropList() const
{
    return propList;
}

Prop ConfigHelper::getPropForID(int ID) const
{
    Prop prop;
    prop.setID(-1);
    for (int i = 0; i < propList.size(); i++)
    {
        prop = propList.at(i);
        if (prop.getID() == ID)
        {
            ConfigHelperDebug("find prop for ID: %d", ID);
            break;
        }
    }
    if (prop.getID() == -1)
    {
        ConfigHelperDebug("cannot find prop for ID!");
    }
    return prop;
}

QString ConfigHelper::getNodeServerIP()
{
    return nodeServerIP;
}

int ConfigHelper::getNodeServerPort()
{
    return nodeServerPort;
}

QString ConfigHelper::getStatusServerIP()
{
    return statusServerIP;
}

int ConfigHelper::getStatusServerPort()
{
    return statusServerPort;
}

QString ConfigHelper::getUploadPrefix()
{
    return uploadPrefix;
}

QString ConfigHelper::getYYCheckGUID() const
{
    return yyCheckGUID;
}

void ConfigHelper::setYYCheckGUID(const QString &value)
{
    yyCheckGUID = value;
}

int ConfigHelper::getYYClientPort() const
{
    return yyClientPort;
}

void ConfigHelper::setYYClientPort(int value)
{
    yyClientPort = value;
}


QString ConfigHelper::getPluginPath() const
{
    return pluginPath;
}

void ConfigHelper::setPluginPath(const QString &value)
{
    pluginPath = value;
}


/*
*  fetch application configurations
*/


QString ConfigHelper::getHttpDownloadPrefix()
{
    return httpDownloadPrefix;
}

QString ConfigHelper::getDownloadFileSavePath()
{
    return downloadFileSavePath;
}
