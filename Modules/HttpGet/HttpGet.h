#ifndef HTTPGET_H
#define HTTPGET_H

#include <QObject>
#include <QtNetwork>
#include <QLinkedList>
#include <QTimer>
#include <QThread>
#include "DataType/Song/Song.h"
#include "DataType/Prog/Prog.h"
#include "DataType/Singer/Singer.h"
#include "DataType/MainPageInfo/MainPageInfo.h"

#define HTTPGETDEBUG    1

#if HTTPGETDEBUG
#include <QDebug>
#define HttpGetDebug(format, ...) qDebug("%s, LINE: %d --->"format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define HttpGetDebug /\
/
#endif

//this struct used before httpget request
class HttpGetInfo
{
public:
    QUrl getUrl() const;
    void setUrl(const QUrl &value);
public:
    enum InfoType
    {
        MAIN_PAGE_INFO = 0,
        SONG_LIST_INFO,
        FEED_BACK_INFO,
        SINGER_LIST_INFO,
        SINGER_SONG_LIST_INFO,
        PROG_LIST_INFO,
        PROG_SONG_LIST_INFO,
        SONG_PATH_INFO
    };
    InfoType getType() const;
    void setType(const InfoType &value);


    QVariant getExtraData() const;
    void setExtraData(const QVariant &value);

private:
    InfoType type;
    QUrl url;
    QVariant extraData;
};

class HttpGet : public QObject
{
    Q_OBJECT
public:
    explicit HttpGet(QObject *parent = 0);
    ~HttpGet();
    bool request(const HttpGetInfo& getTypeInfo);          // interface
private:
    void parseMainPageInfo(QByteArray array);
    void parseSongListInfo(QByteArray array);
    void parseFeedBackInfo(QByteArray array);
    void parseSingerListInfo(QByteArray array);
    void parseSingerSongListInfo(QByteArray array);
    void parseProgListInfo(QByteArray array);
    void parseProgSongListInfo(QByteArray array);
    void parseSongPathInfo(QByteArray reply, QString id);

signals:
    void onSongList(bool ,QList<Song> songList);
    void onRewardList(bool ,QList<VictoryReward> songList);
    void onMainPageInfo(bool ,MainPageInfo mainPageInfo);
    void onFeedBackInfo(bool);

    void onSingerListInfo(bool , QList<Singer>);
    void onSingerSongListInfo(bool , QList<Song>);
    void onProgListInfo(bool , QList<Prog>);
    void onProgSongListInfo(bool , QList<Song>);
    void onSongPathInfo(bool , Song);
private slots:
    void handleTimeout();
    void handleFinish(QNetworkReply *reply);
    void handleSSlErrors(QNetworkReply *reply, const QList<QSslError> &errorList);
//    void handleGetSongPathTimeout();
private:
    QTimer                      *timer;
 //   QTimer                      *getSongPathTimer;
    QNetworkAccessManager       *httpStream;
    QNetworkReply               *httpReply;
    QLinkedList<HttpGetInfo>    *requestList;
    HttpGetInfo                 currentHttpGetInfo;
    bool                        bHttpGetting;
};

#endif // HTTPGET_H
