#ifndef SONGMENUWIDGET_H
#define SONGMENUWIDGET_H

#define SONGMENUWIDGET_DEBUG  1

#if SONGMENUWIDGET_DEBUG
#include <QDebug>
#define SongMenuWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define SongMenuWidgetDebug(format,...)
#endif

#include "SongMenuControl.h"

#include <QPushButton>
#include <QList>
#include <QMap>
#include <QTableWidgetItem>
#include <QHBoxLayout>
#include <QTimer>
#include <QAbstractListModel>

#include "DataType/Song/Song.h"
#include "HttpGet/HttpGet.h"
#include "DataType/Singer/Singer.h"
#include "DataType/Prog/Prog.h"

enum eSingerType
{
    eManSinger = 1,
    eWomanSinger = 2,
    eSingerGroup = 4,
    eForeignSinger = 8
};

namespace Ui {
class SongMenuWidget;
}

class SongMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SongMenuWidget(QWidget *parent = 0);
    ~SongMenuWidget();

private:
    Ui::SongMenuWidget *ui;

private:
    void init();
    //clear QMap<letter, QList<SongNum/SingerNum>>
    void clearLetterIndexNumList();
    //shet letter index state(can use or cannot use)
    void setLetterIndexState();

    void changeSingerColumnWidget(int type);    //singer type
    void setSearchResultOrSingerSongsWidget(QString keys);

private slots:
    void handleLetterIndexButtonOnClicked(char);
    void handleRecommendColumnButtonOnClicked(QString currentColumn);


    void handleDecideSongButtonOnClicked(QString songNum);
    void handleDecideSingerButtonOnClicked(QString singerNum);

private slots:
    void handleSearchButtonOnClicked();
    void handleRecommendButtonOnClicked();
    void handleSingerButtonOnClicked();
    void handleRetryButtonOnClicked();

    void handleManSingerButtonOnClicked();
    void handleWomanSingerButtonOnClicked();
    void handleSingerGroupButtonOnClicked();
    void handleForeignSingerButtonOnClicked();

    //refresh widget
    //refresh commendResult widget
    void handleCommendWidgetOnRefreshData();
    //refresh search Result Or singerSong Result
    void handleSearchResultWidgetOnRefreshData();
    //refresh singer widget
    void handleSingerWidgetOnRefreshData();

private slots:
    //get info from http
    void handleHttpGetOnProgListInfo(bool,QList<Prog>);
    void handleHttpGetOnProgSongListInfo(bool,QList<Song>);
    void handleHttpGetOnSingerListInfo(bool,QList<Singer>);
    void handleHttpGetOnSingerSongListInfo(bool,QList<Song>);
    void handleHttpGetOnSongList(bool,QList<Song>);
private:

    QMap<QString, Prog*>                recommendColumn;    //recommend Column info QMap<columnNum, info>
    QMap<QString, Song*>                allSongs;   //all songs QMap<songNum, songInfo>
    QMap<QString, Singer*>              allSingers; //all singers QMap<singerNum, singerInfo>

    QMap<char, QList<QString>*>          letterIndexNumList; //QMap<letter, QList<songNum/singerNum>>
    QList<LetterIndexPushButton*>        letterIndexButtonList;
    QList<RecommendColumnsPushButton*>   columnsButtonList;

    QList<Song*>               singerAllSongs;

private:
    QPushButton                 *firstClassifyButton;

    QPushButton                 *priorTabButton;
    QPushButton                 *priorClassifyButton;
    QPushButton                 *priorLetterIndexButton;

    //current columns button get song
    RecommendColumnsPushButton  *currentGetSongButton;
    QString                     searchKey;


    LetterIndexPushButton       *allLetterIndexButton;

    bool                        isSearchState;
private:
    HttpGet                     *httpGet;
    int                         dataIndex;
    int                         singerDataIndex;
    QList<QString>              *dataList;
    int                         singerType;

    QList<QString>              singerNumSortList;

    bool                        hasGetInfoFromHttp;//true : get success
    int                         timeCount;          //200ms counts
    HttpGetInfo                 currentHttpGetInfo;
    QTimer                      *loadingTimer;  //use ju hua zhuan
    int                         loadingPixmapIndex;

    bool                        getInfoFromHttpIsEmpty;
private:
    void setLoadingWidget();        //get info from server ,need loading widget
private slots:
    void handleLoadingTimerOnTimerOut();

protected:
    void timerEvent(QTimerEvent *e);

public:
    void show();
signals:
    void onQuit();
    void onSelectedSong(Song* song);
};

#endif // SONGMENUWIDGET_H
