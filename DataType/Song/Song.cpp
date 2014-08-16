#include "Song.h"

#include <Modules/ConfigHelper/ConfigHelper.h>
#include "Modules/JsonPrase/cJSON.h"
#include <QFontMetrics>
Song::Song()
{
    sectionCount = 0;
    name = "";
}
char Song::getFirstLetter() const
{
    return firstLetter;
}

void Song::setFirstLetter(char value)
{
    firstLetter = value;
}


QString Song::getSongID() const
{
    return songID;
}

void Song::setSongID(const QString &value)
{
    songID = value;
}

QString Song::getShortCutSongName(QFont font, int &width)
{
    QFontMetrics metrics(font);
    int lastWidth = 0;
    int currentWidth = 0;
    int index = 0;
    for(int i  = 0 ; i < this->name.size() ; i++)
    {
        if(metrics.width(this->name.left(i)) >= width)
        {
            index = i;
            currentWidth = metrics.width(this->name.left(i));
            break;
        }
        else
        {
            lastWidth = metrics.width(this->name.left(i));
        }
    }
    if(index == 0)
    {
        width = metrics.width(this->name);
        return this->name;
    }
    else if(width == currentWidth)
    {
        return this->name.left(index);
    }
    else
    {
        width = lastWidth;
        return this->name.left(index - 1);
    }
}


QByteArray Song::getLyricXmlData() const
{
    return lyricXmlData;
}

void Song::setLyricXmlData(const QByteArray &value)
{
    lyricXmlData = value;
}

QByteArray Song::getSectionXmlData() const
{
    return sectionXmlData;
}

void Song::setSectionXmlData(const QByteArray &value)
{
    sectionXmlData = value;
}

qint32 Song::getSectionCount() const
{
    return sectionCount;
}

void Song::setSectionCount(const qint32 &value)
{
    sectionCount = value;
}

QString Song::getSectionFilePath() const
{
    return sectionFilePath;
}

QString Song::getSectionFileName() const
{
    return sectionFileName;
}

QString Song::getAccompanyFilePath() const
{
    return accompanyFilePath;
}

void Song::setAccompanyFilePath(const QString &path)
{
    accompanyFilePath = path;
}

QString Song::getAccompanyFileName() const
{
    return accompanyFileName;
}

QString Song::getLyricFilePath() const
{
    return lyricFilePath;
}

QString Song::getLyricFileName() const
{
    return lyricFileName;
}

QUrl Song::getSectionUrl() const
{
    return sectionUrl;
}

void Song::setSectionUrl(const QUrl &value)
{
    sectionUrl = value;
}

QUrl Song::getAccompanyUrl() const
{
    return accompanyUrl;
}

void Song::setAccompanyUrl(const QUrl &value)
{
    accompanyUrl = value;
}

QUrl Song::getLyricUrl() const
{
    return lyricUrl;
}

void Song::setLyricUrl(const QUrl &value)
{
    lyricUrl = value;
}

QString Song::getSinger() const
{
    return singer;
}

void Song::setSinger(const QString &value)
{
    singer = value;
}

QString Song::getName() const
{
    return name;
}

void Song::setName(const QString &value)
{
    name = value;

    ConfigHelper *cfg = ConfigHelper::getInstance();

    lyricFileName = name + ".xml";
    lyricFilePath = cfg->getDownloadFileSavePath() + lyricFileName;

    accompanyFileName   = name + ".mp3";
    accompanyFilePath   = cfg->getDownloadFileSavePath() + accompanyFileName;

    sectionFileName     = name + ".section.xml";
    sectionFilePath     = cfg->getDownloadFileSavePath() + sectionFileName;

}

bool Song::setValueFromJSON(QString strJSON)
{
    ConfigHelper *cfg = ConfigHelper::getInstance();

    songID              = "";
    name                = "";
    singer              = "";

    lyricUrl            = QUrl("");
    lyricFileName       = "";
    lyricFilePath       = "";

    accompanyUrl        = QUrl("");
    accompanyFileName   = "";
    accompanyFilePath   = "";

    sectionUrl          = QUrl("");
    sectionFileName     = "";
    sectionFilePath     = "";

    sectionCount = 0;

    /*
    {
        "id": "1",
        "songId": "201405210001",
        "name": "朋友",
        "singer": "周华健",
        "lyric": "/lrc/朋友-周华健.xml",
        "section": "/section/朋友-周华健.section.xml",
        "accompany": "/audio/朋友-周华健.mp3",
        "sectionCount": 2,
    }
    */
    SongDebug("song reply is %s" ,strJSON.toUtf8().data() );
    cJSON *jsSong = cJSON_Parse(strJSON.toUtf8().data());
    {
        if (cJSON_GetObjectItem(jsSong, "songId") != NULL)
        {
            songID = QString::fromUtf8(cJSON_GetObjectItem(jsSong, "songId")->valuestring);
        }

        if (cJSON_GetObjectItem(jsSong, "name") != NULL)
        {
            name = QString::fromUtf8(cJSON_GetObjectItem(jsSong, "name")->valuestring);
        }

        if (cJSON_GetObjectItem(jsSong, "singer") != NULL)
        {
            singer = QString::fromUtf8(cJSON_GetObjectItem(jsSong, "singer")->valuestring);
        }

        if (cJSON_GetObjectItem(jsSong, "lyric") != NULL)
        {
            lyricUrl = QUrl(cfg->getHttpDownloadPrefix() + QString::fromUtf8(cJSON_GetObjectItem(jsSong, "lyric")->valuestring));
            lyricFileName = name + ".xml";
            lyricFilePath = cfg->getDownloadFileSavePath() + lyricFileName;
        }

        if (cJSON_GetObjectItem(jsSong, "accompany") != NULL)
        {
            accompanyUrl = QUrl(cfg->getHttpDownloadPrefix() + QString::fromUtf8(cJSON_GetObjectItem(jsSong, "accompany")->valuestring));
            accompanyFileName   = name + ".mp3";
            accompanyFilePath   = cfg->getDownloadFileSavePath() + accompanyFileName;
        }

        if (cJSON_GetObjectItem(jsSong, "section") != NULL)
        {
            sectionUrl = QUrl(cfg->getHttpDownloadPrefix() + QString::fromUtf8(cJSON_GetObjectItem(jsSong, "section")->valuestring));
            sectionFileName     = name + ".section.xml";
            sectionFilePath     = cfg->getDownloadFileSavePath() + sectionFileName;
        }

        if (cJSON_GetObjectItem(jsSong, "sectionCount") != NULL)
        {
            sectionCount = cJSON_GetObjectItem(jsSong, "sectionCount")->valueint;
        }

    }
    cJSON_Delete(jsSong);

    SongDebug("songID is: %s", songID.toUtf8().data());
    SongDebug("name is: %s", name.toUtf8().data());
    SongDebug("singer is: %s", singer.toUtf8().data());
    SongDebug("lyricUrl is: %s", lyricUrl.toString().toUtf8().data());
    SongDebug("accompanyUrl is: %s", accompanyUrl.toString().toUtf8().data());
    SongDebug("sectionUrl is: %s", sectionUrl.toString().toUtf8().data());
    SongDebug("sectionCount is: %d", sectionCount);

    return true;
}

void Song::clearSectionXmlData()
{
    this->sectionXmlData.clear();
}

void Song::clearLyricXmlData()
{
    this->lyricXmlData.clear();
}

void Song::addSectionXmlData(QByteArray data)
{
    this->sectionXmlData += data;
}

void Song::addLyricXmlData(QByteArray data)
{
    this->lyricXmlData += data;
}
