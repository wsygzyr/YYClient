#ifndef SONG_H
#define SONG_H

#include <QtCore>
#include <QUrl>


#define DEBUG_SONG  1

#if DEBUG_SONG
#include <QDebug>
#define SongDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define SongDebug(format,...)
#endif

class Song
{

public:
    Song();

private:
    QString     songID;
    QString     name;
    QString     singer;
    QUrl        lyricUrl;
    QUrl        accompanyUrl;
    QUrl        sectionUrl;

    QString     lyricFileName;
    QString     lyricFilePath;
    QString     accompanyFileName;
    QString     accompanyFilePath;
    QString     sectionFileName;
    QString     sectionFilePath;
    qint32      sectionCount;

    QByteArray  sectionXmlData;
    QByteArray  lyricXmlData;

    char        firstLetter;
public:
    bool Song::setValueFromJSON(QString strJSON);


    void clearSectionXmlData();
    void clearLyricXmlData();
    void addSectionXmlData(QByteArray data);
    void addLyricXmlData(QByteArray data);


    QString getName() const;
    void setName(const QString &value);

    QString getSinger() const;
    void setSinger(const QString &value);

    QUrl getLyricUrl() const;
    void setLyricUrl(const QUrl &value);

    QUrl getAccompanyUrl() const;
    void setAccompanyUrl(const QUrl &value);

    QUrl getSectionUrl() const;
    void setSectionUrl(const QUrl &value);

    QString getLyricFileName() const;

    QString getLyricFilePath() const;

    QString getAccompanyFileName() const;

    QString getAccompanyFilePath() const;
    void    setAccompanyFilePath(const QString& path);

    QString getSectionFileName() const;

    QString getSectionFilePath() const;

    qint32 getSectionCount() const;
    void setSectionCount(const qint32 &value);

    QByteArray getSectionXmlData() const;
    void setSectionXmlData(const QByteArray &value);

    QByteArray getLyricXmlData() const;
    void setLyricXmlData(const QByteArray &value);
    QString getSongID() const;
    void setSongID(const QString &value);

    QString getShortCutSongName(QFont , int& width);
    char getFirstLetter() const;
    void setFirstLetter(char value);
};

#endif // SONG_H
