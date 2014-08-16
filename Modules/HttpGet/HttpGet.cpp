#include "HttpGet.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/JsonPrase/cJSON.h"


HttpGet::HttpGet(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<MainPageInfo>("MainPageInfo");
    qRegisterMetaType<QList<Singer>>("QList<Singer>");
    qRegisterMetaType<QList<Prog>>("QList<Prog>");

    bHttpGetting = false;
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    timer->start(100);  // 100 microseconds

 //   getSongPathTimer = new QTimer(this);
 //   QObject::connect(getSongPathTimer, SIGNAL(timeout()), this, SLOT(handleGetSongPathTimeout()));


    requestList = new QLinkedList<HttpGetInfo>;

    httpStream = new QNetworkAccessManager(this);

    QObject::connect(httpStream, SIGNAL(sslErrors ( QNetworkReply *, const QList<QSslError>&)), this, SLOT(handleSSlErrors(QNetworkReply *, const QList<QSslError>&)));

    QObject::connect(httpStream, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleFinish(QNetworkReply*)));
}

HttpGet::~HttpGet()
{
    requestList->clear();
    delete requestList;
}

#include "Modules/ihouAuth/des/DES.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
void HttpGet::handleTimeout()
{
        //HttpGetDebug("timeout");

    if (requestList->count() == 0 || !PkInfoManager::getInstance()->getIsSyncToServer())
    {
       // HttpGetDebug("request count is %d , SyncToServer is %s" , requestList->count() , PkInfoManager::getInstance()->getIsSyncToServer() ? "true" : "false");
        return;
    }
    if(bHttpGetting)
    {
        HttpGetDebug("Get another http request");
        return;
    }
    bHttpGetting = true;
    currentHttpGetInfo = requestList->takeFirst();

//    if(currentHttpGetInfo.getType() == HttpGetInfo::InfoType::SONG_PATH_INFO)
//    {
//        getSongPathTimer->start(5000);
//    }

    QUrl url = currentHttpGetInfo.getUrl();
    HttpGetDebug("request url: %s", url.toString().toUtf8().data());



    /*
     *  cipher = DES（1970-01-01T00:00:00后的毫秒数 + “_”+ GUID + “_”+ “iHouPlugin!”）
     */
    qint64 millisecond = QDateTime::currentMSecsSinceEpoch() + PkInfoManager::getInstance()->getMillisecondsDiff();
    QString plain = QString::number(millisecond) + "_" + QUuid::createUuid().toString() + "_" + "iHouPlugin!";
    HttpGetDebug("plain is: %s", plain.toUtf8().data());
    char cipherBuf[1024];
    int cipherLen = DES_Encrypt(plain.toUtf8().data(), cipherBuf, "ihouihou");
    QString cipher = QString(QByteArray(cipherBuf, cipherLen));
    HttpGetDebug("cipher is: %s", cipher.toUtf8().data());

    QNetworkRequest netRequest(url);
    netRequest.setRawHeader(QString("cipher").toUtf8(), cipher.toUtf8());

    httpStream->get(netRequest);
}

void HttpGet::handleFinish(QNetworkReply *reply)
{
    if(!reply)
    {
        HttpGetDebug("reply is NULL");
        return;
    }

    QByteArray replyByteArray = reply->readAll();
    HttpGetDebug("reply is %s" , replyByteArray.data());
    reply->deleteLater();
    reply = NULL;

    switch(currentHttpGetInfo.getType())
    {
    case HttpGetInfo::InfoType::MAIN_PAGE_INFO:
        parseMainPageInfo(replyByteArray);
        break;
    case HttpGetInfo::InfoType::SONG_LIST_INFO:
        parseSongListInfo(replyByteArray);
        break;
    case HttpGetInfo::InfoType::FEED_BACK_INFO:
        parseFeedBackInfo(replyByteArray);
        break;
    case HttpGetInfo::InfoType::SINGER_LIST_INFO:
        parseSingerListInfo(replyByteArray);
        break;
    case HttpGetInfo::InfoType::SINGER_SONG_LIST_INFO:
        parseSingerSongListInfo(replyByteArray);
        break;
    case HttpGetInfo::InfoType::PROG_LIST_INFO:
        parseProgListInfo(replyByteArray);
        break;
    case HttpGetInfo::InfoType::PROG_SONG_LIST_INFO:
        parseProgSongListInfo(replyByteArray);
        break;
    case HttpGetInfo::InfoType::SONG_PATH_INFO:
        parseSongPathInfo(replyByteArray , currentHttpGetInfo.getExtraData().value<QString>());
        break;
    }

    bHttpGetting = false;
}

void HttpGet::handleSSlErrors(QNetworkReply *reply, const QList<QSslError> & errorList)
{
    HttpGetDebug("ssl errors!");
    for (int i = 0; i < errorList.count(); i++)
    {
        HttpGetDebug("error info:%s",errorList[i].errorString().toUtf8().data());
    }
}

/*
void HttpGet::handleGetSongPathTimeout()
{
    getSongPathTimer->stop();
    Song song;
    emit onSongPathInfo(false , song);
}
*/

bool HttpGet::request(const HttpGetInfo &getTypeInfo)
{
    HttpGetDebug("url is: %s", getTypeInfo.getUrl().toString().toUtf8().data());

    if(!getTypeInfo.getUrl().isValid())
    {
        HttpGetDebug("Error:URL is invalid.");
        return false;
    }
    if(getTypeInfo.getUrl().scheme() != "http")
    {
        HttpGetDebug("Error:URL must start with 'http:'");
        return false;
    }

    if(getTypeInfo.getUrl().path().isEmpty())
    {
        HttpGetDebug("Error:URL's path is empty.");
        return false;
    }
    requestList->append(getTypeInfo);
    return true;
}

void HttpGet::parseMainPageInfo(QByteArray reply)
{
    MainPageInfo mainPageInfo;
    //**********************************
    cJSON *jsMainPage = cJSON_Parse(reply.data());
    {
        if (!jsMainPage)
        {
            HttpGetDebug("error: parse JSON failed!");
            return;
        }

        cJSON *jsRetCode = cJSON_GetObjectItem(jsMainPage , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpGetDebug("mainPageInfo get error,retcode is %s" , retCode.toUtf8().data());
            emit onMainPageInfo(false , mainPageInfo);
            cJSON_Delete(jsMainPage);
            return;
        }
        cJSON *jsData = cJSON_GetObjectItem(jsMainPage , "data");
        {
            cJSON *jsWeekHeat = cJSON_GetObjectItem(jsData, "thisWeekHeat");
            {
                mainPageInfo.setThisWeekHeat(jsWeekHeat->valueint);
                HttpGetDebug("thisWeekHeat is: %d", mainPageInfo.getThisWeekHeat());
            }
            cJSON *jsTotalIncome = cJSON_GetObjectItem(jsData, "thisMonthIncome");
            {
                mainPageInfo.setThisMonthIncome(jsTotalIncome->valuedouble);
                HttpGetDebug("thisMonthIncome is: %f", mainPageInfo.getThisMonthIncome());
            }


/*
            cJSON *jsDailyPaperIncomeList = cJSON_GetObjectItem(jsData, "dailyPaperIncome");
            {
                QList<DailyPaperIncome> dpiList;
                for (int i = 0; i < cJSON_GetArraySize(jsDailyPaperIncomeList); i++)
                {

                    cJSON *jsDailyPaperIncome = cJSON_GetArrayItem(jsDailyPaperIncomeList, i);
                    {
                        DailyPaperIncome dpi;
                        cJSON *jsHost = cJSON_GetObjectItem(jsDailyPaperIncome, "host");
                        {
                            Player host;
                            host.setID(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "hostID")->valuestring).toInt());
                            host.setName(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "hostName")->valuestring));
                            dpi.setHost(host);

                            HttpGetDebug("host.getID() is: %d", host.getID());
                            HttpGetDebug("host.getName() is: %s", host.getName().toUtf8().data());

                        }

                        dpi.setPkTimes(cJSON_GetObjectItem(jsDailyPaperIncome, "pkTimes")->valueint);
                        dpi.setIncome(cJSON_GetObjectItem(jsDailyPaperIncome, "income")->valuedouble);

                        HttpGetDebug("dpi.getPkTimes() is: %d", dpi.getPkTimes());
                        HttpGetDebug("dpi.getIncome() is: %f", dpi.getIncome());

                        dpiList.append(dpi);


                    }
                }
                mainPageInfo.setDailyPaperIncomeList(dpiList);
            }
*/

            cJSON *jsDailyPaperHeatList = cJSON_GetObjectItem(jsData, "dailyPaperHeat");
            {
                QList<DailyPaperHeat> dphList;
                for (int i = 0; i < cJSON_GetArraySize(jsDailyPaperHeatList); i++)
                {
                    cJSON *jsDailyPaperHeat = cJSON_GetArrayItem(jsDailyPaperHeatList, i);
                    {
                        DailyPaperHeat dph;
                        cJSON *jsHost = cJSON_GetObjectItem(jsDailyPaperHeat, "host");
                        {
                            Player host;
                            host.setID(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "hostID")->valuestring).toInt());
                            host.setName(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "hostName")->valuestring));
                            dph.setHost(host);

                            HttpGetDebug("host.getID() is: %d", host.getID());
                            HttpGetDebug("host.getName() is: %s", host.getName().toUtf8().data());

                        }

                        dph.setHeat(cJSON_GetObjectItem(jsDailyPaperHeat, "heat")->valueint);
                        HttpGetDebug("dph.getHeat() is: %d", dph.getHeat());

                        dphList.append(dph);
                    }
                }
                mainPageInfo.setDailyPaperHeatList(dphList);
            }

            cJSON *jsDailyPaperPropList = cJSON_GetObjectItem(jsData, "dailyPaperProp");
            {
                QList<DailyPaperProp> dppList;
                for (int i = 0; i < cJSON_GetArraySize(jsDailyPaperPropList); i++)
                {
                    cJSON *jsDailyPaperProp = cJSON_GetArrayItem(jsDailyPaperPropList, i);
                    {
                        DailyPaperProp dpp;
                        cJSON *jsHost = cJSON_GetObjectItem(jsDailyPaperProp, "host");
                        {
                            Player host;
                            host.setID(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "hostID")->valuestring).toInt());
                            host.setName(QString::fromUtf8(cJSON_GetObjectItem(jsHost, "hostName")->valuestring));
                            dpp.setHost(host);

                            HttpGetDebug("host.getID() is: %d", host.getID());
                            HttpGetDebug("host.getName() is: %s", host.getName().toUtf8().data());
                        }

                        Prop prop;
                        prop.setID(QString::fromUtf8(cJSON_GetObjectItem(jsDailyPaperProp, "propID")->valuestring).toInt());
                        prop.setUseNumber(cJSON_GetObjectItem(jsDailyPaperProp, "times")->valueint);
                        dpp.setProp(prop);

                        HttpGetDebug("dpp.getProp().getID() is: %d", dpp.getProp().getID());
                        HttpGetDebug("dpp.getProp).getUseNumber() is: %d", dpp.getProp().getUseNumber());

                        dppList.append(dpp);
                    }
                }
                mainPageInfo.setDailyPaperPropList(dppList);
            }
        }
    }

    cJSON_Delete(jsMainPage);
    //**********************************
    emit onMainPageInfo(true , mainPageInfo);
}

void HttpGet::parseSongListInfo(QByteArray reply)
{
    QList<Song> songList;

    /*
    [{
       "id" :3,
       "songId" :"003",
       "name" :"爱转角(高潮版)",
       "letter" :"a",
       "singer" :"罗志祥",
       "pinyin" :"aizhuanjiao",
       "lyric" :"/lyric/爱转角(高潮版).xml",
       "accompany" :"/acc/爱转角(高潮版).mp3",
       "section" :"/lyric/爱转角(高潮版).section.xml"
    }, {
       "id" :2,
       "songId" :"002",
       "name" :"爱我别走(高潮版)",
       "letter" :"a",
       "singer" :"张震岳",
       "pinyin" :"aiwobiezou",
       "lyric" :"/lyric/爱我别走(高潮版).xml",
       "accompany" :"/acc/爱我别走(高潮版).mp3",
       "section" :"/lyric/爱我别走(高潮版).section.xml"
    }]
    */

    cJSON *jsSongList = cJSON_Parse(reply.data());
    {
        char *strJSON = cJSON_Print(jsSongList);
        HttpGetDebug("reply is: %s", QString(strJSON).toUtf8().data());
        free(strJSON);

        if (NULL == jsSongList)
        {
            HttpGetDebug("error: jsSongList is NULL!");
            return;
        }
        cJSON *jsRetCode = cJSON_GetObjectItem(jsSongList , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpGetDebug("SongListInfo get error,retcode is %s" , retCode.toUtf8().data());
            emit onSongList(false , songList);
            cJSON_Delete(jsSongList);
            return;
        }
        cJSON *jsData = cJSON_GetObjectItem(jsSongList , "data");
        for (int i = 0; i < cJSON_GetArraySize(jsData); i++)
        {
            cJSON *jsSong = cJSON_GetArrayItem(jsData, i);
            {
                char *strSong = cJSON_PrintUnformatted(jsSong);
                QString strJSON = QString::fromUtf8(strSong);
                free(strSong);
                Song song;
                song.setValueFromJSON(strJSON);
                songList.append(song);
            }
        }

        cJSON_Delete(jsSongList);
    }
    emit onSongList(true , songList);
}

void HttpGet::parseFeedBackInfo(QByteArray reply)
{
    cJSON *jsFeedBack = cJSON_Parse(reply.data());
    {
        char *strJSON = cJSON_Print(jsFeedBack);
        HttpGetDebug("reply is: %s", strJSON);
        free(strJSON);

        if (NULL == jsFeedBack)
        {
            HttpGetDebug("error: jsSongList is NULL!");
            return;
        }
        cJSON *jsRetCode = cJSON_GetObjectItem(jsFeedBack , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpGetDebug("SongListInfo get error,retcode is %s" , retCode.toUtf8().data());
            emit onFeedBackInfo(false);
            cJSON_Delete(jsFeedBack);
            return;
        }
        cJSON_Delete(jsFeedBack);
    }
    emit onFeedBackInfo(true);
}

void HttpGet::parseSingerListInfo(QByteArray reply)
{
    QList<Singer> singerList;
    cJSON *jsSingerList = cJSON_Parse(reply.data());
    {
        char* strJSON = cJSON_Print(jsSingerList);
        HttpGetDebug("reply is %s" , strJSON);
        free(strJSON);

        if(NULL == jsSingerList)
        {
            HttpGetDebug("error: jsSingerList is NULL!");
            return;
        }
        cJSON *jsRetCode = cJSON_GetObjectItem(jsSingerList , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpGetDebug("SingerListInfo get error,retcode is %s" , retCode.toUtf8().data());
            emit onSingerListInfo(false , singerList);
            cJSON_Delete(jsSingerList);
            return;
        }

        cJSON *jsData = cJSON_GetObjectItem(jsSingerList , "data");
        for (int i = 0; i < cJSON_GetArraySize(jsData); i++)
        {
            cJSON *jsSinger = cJSON_GetArrayItem(jsData, i);
            {
                Singer singer;
                singer.setSingNum(QString::fromUtf8(cJSON_GetObjectItem(jsSinger , "no")->valuestring));
                singer.setSingerName(QString::fromUtf8(cJSON_GetObjectItem(jsSinger , "name")->valuestring));
                singer.setFirstLetter(*(QString::fromUtf8(cJSON_GetObjectItem(jsSinger , "firstLetter")->valuestring).toUpper().toUtf8().data()));
                singer.setType(cJSON_GetObjectItem(jsSinger , "type")->valueint);

                singerList.append(singer);
            }
        }

        cJSON_Delete(jsSingerList);
    }
    emit onSingerListInfo(true , singerList);
}

void HttpGet::parseSingerSongListInfo(QByteArray reply)
{
    QList<Song> songList;
    cJSON *jsSongList = cJSON_Parse(reply.data());
    {
        char* strJSON = cJSON_Print(jsSongList);
        HttpGetDebug("reply is %s" , strJSON);
        free(strJSON);

        if(NULL == jsSongList)
        {
            HttpGetDebug("error: jsSingerSongList is NULL!");
            return;
        }
        cJSON *jsRetCode = cJSON_GetObjectItem(jsSongList , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpGetDebug("SingerSongListInfo get error,retcode is %s" , retCode.toUtf8().data());
            emit onSingerSongListInfo(false , songList);
            cJSON_Delete(jsSongList);
            return;
        }

        cJSON *jsData = cJSON_GetObjectItem(jsSongList , "data");
        for (int i = 0; i < cJSON_GetArraySize(jsData); i++)
        {
            cJSON *jsSong = cJSON_GetArrayItem(jsData, i);
            {
                Song song;
                song.setSongID(QString::fromUtf8(cJSON_GetObjectItem(jsSong , "no")->valuestring));
                song.setName(QString::fromUtf8(cJSON_GetObjectItem(jsSong , "name")->valuestring));
                song.setFirstLetter(*(QString::fromUtf8(cJSON_GetObjectItem(jsSong , "firstLetter")->valuestring).toUpper().toUtf8().data()));
                song.setSectionCount(cJSON_GetObjectItem(jsSong , "sectionCount")->valueint);

                songList.append(song);
            }
        }

        cJSON_Delete(jsSongList);
    }
    emit onSingerSongListInfo(true , songList);
}

void HttpGet::parseProgListInfo(QByteArray reply)
{
    QList<Prog> progList;
    cJSON *jsProgList = cJSON_Parse(reply.data());
    {
        char* strJSON = cJSON_Print(jsProgList);
        HttpGetDebug("reply is %s" , strJSON);
        free(strJSON);

        if(NULL == jsProgList)
        {
            HttpGetDebug("error: jsProgList is NULL!");
            return;
        }
        cJSON *jsRetCode = cJSON_GetObjectItem(jsProgList , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpGetDebug("ProgListInfo get error,retcode is %s" , retCode.toUtf8().data());
            emit onProgListInfo(false , progList);
            cJSON_Delete(jsProgList);
            return;
        }

        cJSON *jsData = cJSON_GetObjectItem(jsProgList , "data");
        for (int i = 0; i < cJSON_GetArraySize(jsData); i++)
        {
            cJSON *jsProg = cJSON_GetArrayItem(jsData, i);
            {
                Prog prog;
                prog.setColumnNum(QString::fromUtf8(cJSON_GetObjectItem(jsProg , "no")->valuestring));
                prog.setColumnName(QString::fromUtf8(cJSON_GetObjectItem(jsProg , "name")->valuestring));

                progList.append(prog);
            }
        }

        cJSON_Delete(jsProgList);
    }
    emit onProgListInfo(true , progList);
}

void HttpGet::parseProgSongListInfo(QByteArray reply)
{
    QList<Song> songList;
    cJSON *jsSongList = cJSON_Parse(reply.data());
    {
        char* strJSON = cJSON_Print(jsSongList);
        HttpGetDebug("reply is %s" , strJSON);
        free(strJSON);

        if(NULL == jsSongList)
        {
            HttpGetDebug("error: jsProgSongList is NULL!");
            return;
        }
        cJSON *jsRetCode = cJSON_GetObjectItem(jsSongList , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpGetDebug("SingerSongListInfo get error,retcode is %s" , retCode.toUtf8().data());
            emit onProgSongListInfo(false , songList);
            cJSON_Delete(jsSongList);
            return;
        }

        cJSON *jsData = cJSON_GetObjectItem(jsSongList , "data");
        for (int i = 0; i < cJSON_GetArraySize(jsData); i++)
        {
            cJSON *jsSong = cJSON_GetArrayItem(jsData, i);
            {
                Song song;
                song.setSongID(QString::fromUtf8(cJSON_GetObjectItem(jsSong , "no")->valuestring));
                song.setName(QString::fromUtf8(cJSON_GetObjectItem(jsSong , "name")->valuestring));
                song.setFirstLetter(*(QString::fromUtf8(cJSON_GetObjectItem(jsSong , "firstLetter")->valuestring).toUpper().toUtf8().data()));
                song.setSinger(QString::fromUtf8(cJSON_GetObjectItem(jsSong , "singername")->valuestring));
                song.setSectionCount(cJSON_GetObjectItem(jsSong , "sectionCount")->valueint);

                songList.append(song);
            }
        }

        cJSON_Delete(jsSongList);
    }
    emit onProgSongListInfo(true , songList);
}

void HttpGet::parseSongPathInfo(QByteArray reply, QString id)
{
 //   getSongPathTimer->stop();
    Song song;
    cJSON *jsPathList = cJSON_Parse(reply.data());
    {
        char* strJSON = cJSON_Print(jsPathList);
        HttpGetDebug("reply is %s" , strJSON);
        free(strJSON);

        if(NULL == jsPathList)
        {
            HttpGetDebug("error: jsProgSongList is NULL!");
            return;
        }
        cJSON *jsRetCode = cJSON_GetObjectItem(jsPathList , "retCode");
        QString retCode = QString::fromUtf8(jsRetCode->valuestring);
        if(retCode != "0000")
        {
            HttpGetDebug("SingerSongListInfo get error,retcode is %s" , retCode.toUtf8().data());
            emit onSongPathInfo(false , song);
            cJSON_Delete(jsPathList);
            return;
        }
        cJSON *jsData = cJSON_GetObjectItem(jsPathList , "data");
        {
            song.setSongID(id);
            song.setAccompanyUrl(QUrl(ConfigHelper::getInstance()->getHttpDownloadPrefix() + QString::fromUtf8(cJSON_GetObjectItem(jsData , "audioPath")->valuestring)));
            song.setLyricUrl(QUrl(ConfigHelper::getInstance()->getHttpDownloadPrefix() + QString::fromUtf8(cJSON_GetObjectItem(jsData , "lyricPath")->valuestring)));
            song.setSectionUrl(QUrl(ConfigHelper::getInstance()->getHttpDownloadPrefix() + QString::fromUtf8(cJSON_GetObjectItem(jsData , "sectionPath")->valuestring)));
        }
        cJSON_Delete(jsPathList);
    }
    emit onSongPathInfo(true , song);
}

QUrl HttpGetInfo::getUrl() const
{
    return url;
}

void HttpGetInfo::setUrl(const QUrl &value)
{
    url = value;
}

HttpGetInfo::InfoType HttpGetInfo::getType() const
{
    return type;
}

void HttpGetInfo::setType(const InfoType &value)
{
    type = value;
}
QVariant HttpGetInfo::getExtraData() const
{
    return extraData;
}

void HttpGetInfo::setExtraData(const QVariant &value)
{
    extraData = value;
}




