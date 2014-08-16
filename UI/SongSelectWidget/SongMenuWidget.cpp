#include "SongMenuWidget.h"
#include "ui_SongMenuWidget.h"
#include <QStyledItemDelegate>
#include "Modules/ConfigHelper/ConfigHelper.h"

#define ALLSONGLETTER   '0'

SongMenuWidget::SongMenuWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SongMenuWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    httpGet = new HttpGet();
    connect(httpGet, SIGNAL(onProgListInfo(bool,QList<Prog>)),
            this, SLOT(handleHttpGetOnProgListInfo(bool,QList<Prog>)));
    connect(httpGet, SIGNAL(onProgSongListInfo(bool,QList<Song>)),
            this, SLOT(handleHttpGetOnProgSongListInfo(bool,QList<Song>)));
    connect(httpGet, SIGNAL(onSingerListInfo(bool,QList<Singer>)),
            this, SLOT(handleHttpGetOnSingerListInfo(bool,QList<Singer>)));
    connect(httpGet, SIGNAL(onSingerSongListInfo(bool,QList<Song>)),
            this, SLOT(handleHttpGetOnSingerSongListInfo(bool,QList<Song>)));
    connect(httpGet, SIGNAL(onSongList(bool,QList<Song>)),
            this, SLOT(handleHttpGetOnSongList(bool,QList<Song>)));
    loadingTimer = new QTimer;
    priorClassifyButton = NULL;
    priorLetterIndexButton = NULL;
    priorTabButton = NULL;
    firstClassifyButton = NULL;
    connect(loadingTimer, SIGNAL(timeout()), this, SLOT(handleLoadingTimerOnTimerOut()));
    init();
}

SongMenuWidget::~SongMenuWidget()
{
    delete ui;
    delete httpGet;
    loadingTimer->deleteLater();
    for(int i = 0; i< this->children().size(); i++)
    {
        this->children().at(i)->dumpObjectInfo();
        SongMenuWidgetDebug("%p",  this->children().at(i));
    }
    allLetterIndexButton->deleteLater();
    //delete letter button
    for (int i = 0; i < letterIndexButtonList.length(); i++)
    {
        letterIndexButtonList.at(i)->deleteLater();
    }
    //delete allProgs
    QMap<QString, Prog*>::iterator it1 = recommendColumn.begin();
    for (it1;it1 != recommendColumn.end();it1++)
    {
        delete it1.value();
    }
    //delete allSongs
    QMap<QString, Song*>::iterator it2 = allSongs.begin();
    for (it2;it2 != allSongs.end();it2++)
    {
        delete it2.value();
    }
    //delete allProgs
    QMap<QString, Singer*>::iterator it3 = allSingers.begin();
    for (it3;it3 != allSingers.end();it3++)
    {
        delete it3.value();
    }
    //delete letter index numList
    QMap<char, QList<QString>*>::iterator it4 = letterIndexNumList.begin();
    for (it4;it4 != letterIndexNumList.end();it4++)
    {
        delete it4.value();
    }
    //delete singer allSong
    for (int i = 0; i < singerAllSongs.length(); i ++)
    {
        delete singerAllSongs.at(i);
    }
}

void SongMenuWidget::init()
{
    //init search
    priorTabButton = ui->pushButton_recommend;
    //init stackWidget
    ui->page_recommend->show();
    ui->page_singer->hide();
    ui->page_recommendResult->show();
    ui->page_singerResult->hide();

    ui->widget_singerAllSong->hide();
    ui->widget_cacheing->hide();
    ui->label_hint->hide();
    ui->pushButton_retry->hide();
    ui->label_loading->hide();
    isSearchState = false;

    QObject::connect(ui->pushButton_close, SIGNAL(clicked()),
                     this, SIGNAL(onQuit()));
    QObject::connect(ui->pushButton_search, SIGNAL(clicked()),
                     this, SLOT(handleSearchButtonOnClicked()));
    QObject::connect(ui->pushButton_recommend, SIGNAL(clicked()),
                     this, SLOT(handleRecommendButtonOnClicked()));
    QObject::connect(ui->pushButton_singer, SIGNAL(clicked()),
                     this, SLOT(handleSingerButtonOnClicked()));
    QObject::connect(ui->pushButton_retry, SIGNAL(clicked()),
                     this, SLOT(handleRetryButtonOnClicked()));
    QObject::connect(ui->lineEdit_search, SIGNAL(returnPressed()),
                     this, SLOT(handleSearchButtonOnClicked()));

    //init letterIndex Button widget
    allLetterIndexButton = new LetterIndexPushButton(ALLSONGLETTER, QString("ALL"));
    ui->horizontalLayout->addWidget(allLetterIndexButton);
    priorLetterIndexButton = allLetterIndexButton;

    allLetterIndexButton->setEnabled(false);
    allLetterIndexButton->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                                        "border-width:0;"
                                        "border-style:outset;"
                                        "font: 8pt \"Arial\";"
                                        "color: rgb(124, 103, 134);}");
    connect(allLetterIndexButton, SIGNAL(onClicked(char)), this, SLOT(handleLetterIndexButtonOnClicked(char)));
    for (char letter = 'A'; letter <= 'Z'; letter ++)
    {
        LetterIndexPushButton *button = new LetterIndexPushButton(letter, QString(letter), this);
        button->setEnabled(false);
        button->setLetterEnabled(false);
        ui->horizontalLayout->addWidget(button);
        letterIndexButtonList.append(button);
        connect(button, SIGNAL(onClicked(char)), this, SLOT(handleLetterIndexButtonOnClicked(char)));
        QList<QString> *songNumList = new QList<QString>;
        letterIndexNumList.insert(letter, songNumList);
    }
    SongMenuWidgetDebug("init letterIndex Button widget success!");

    //step 1. get recommend column info
    setLoadingWidget();
    HttpGetInfo httpGetInfo;
    httpGetInfo.setType(HttpGetInfo::PROG_LIST_INFO);
    httpGetInfo.setUrl(QUrl(ConfigHelper::getInstance()->getProgListPrefix()));
    currentHttpGetInfo = httpGetInfo;
    httpGet->request(httpGetInfo);

    //init singer widget
    connect(ui->pushButton_manSinger, SIGNAL(clicked()), this, SLOT(handleManSingerButtonOnClicked()));
    connect(ui->pushButton_womanSinger, SIGNAL(clicked()), this, SLOT(handleWomanSingerButtonOnClicked()));
    connect(ui->pushButton_singerGroup, SIGNAL(clicked()), this, SLOT(handleSingerGroupButtonOnClicked()));
    connect(ui->pushButton_foreignSinger, SIGNAL(clicked()), this, SLOT(handleForeignSingerButtonOnClicked()));
}

void SongMenuWidget::handleLetterIndexButtonOnClicked(char letter)
{
    QPushButton* button = (QPushButton*)sender();
    if (priorLetterIndexButton == button)
    {
        SongMenuWidgetDebug("already in current index");
        return;
    }
    LetterIndexPushButton* lButton = (LetterIndexPushButton*)priorLetterIndexButton;
    lButton->setLostFocus();
    lButton = (LetterIndexPushButton*)button;
    lButton->setGetFocus();
    priorLetterIndexButton = button;
    //clear show widget
    if (priorTabButton == ui->pushButton_recommend)
    {
        while(ui->listWidget_recommendResult->count() > 0)
        {
            QListWidgetItem *listItem = ui->listWidget_recommendResult->takeItem(0);
            ui->listWidget_recommendResult->removeItemWidget(listItem);
            delete listItem;
        }
        if (ALLSONGLETTER == letter)
        {
            //if Letter index is "ALL"
            //get songNumList from current columns
            RecommendColumnsPushButton* rButton = (RecommendColumnsPushButton*)priorClassifyButton;
            dataIndex = 0;
            dataList = &rButton->songNumList;
        }
        else
        {
            //letter is 'A' - 'Z'
            //get songNumList from letterIndexNumList
            dataIndex = 0;
            dataList = letterIndexNumList.value(letter);
        }
        handleCommendWidgetOnRefreshData();
    }
    if (priorTabButton == ui->pushButton_singer)
    {
        if (ALLSONGLETTER == letter)
        {
            //show all singers
            if (priorClassifyButton == ui->pushButton_manSinger)
            {
                changeSingerColumnWidget(eManSinger);
            }
            if (priorClassifyButton == ui->pushButton_womanSinger)
            {
                changeSingerColumnWidget(eWomanSinger);
            }
            if (priorClassifyButton == ui->pushButton_singerGroup)
            {
                changeSingerColumnWidget(eSingerGroup);
            }
            if (priorClassifyButton == ui->pushButton_foreignSinger)
            {
                changeSingerColumnWidget(eForeignSinger);
            }
        }
        else
        {
            while(ui->listWidget_singerResult->count() > 0)
            {
                QListWidgetItem *listItem = ui->listWidget_singerResult->takeItem(0);
                ui->listWidget_singerResult->removeItemWidget(listItem);
                delete listItem;
            }
            for (int i = 0; i < letterIndexNumList.value(letter)->length();)
            {
                int displayCount = 0;
                QList<QString>   singNumList;
                QList<QString>   singNameList;
                if (letterIndexNumList.value(letter)->length() - i >= 4)
                {
                    displayCount = 4;
                }
                else
                {
                    displayCount = letterIndexNumList.value(letter)->length() - i;
                }
                for (int j = i; j < i + displayCount; j ++)
                {
                    Singer *singer = allSingers.value(letterIndexNumList.value(letter)->at(j));
                    singNumList.append(singer->getSingNum());
                    singNameList.append(singer->getSingerName());
                }


                QListWidgetItem *listItem = new QListWidgetItem(ui->listWidget_singerResult);
                listItem->setSizeHint(QSize(420, 34));
                SingerWidget *widget = new SingerWidget(singNumList,
                                                        singNameList,
                                                        displayCount,
                                                        (0 == i),
                                                        letter);
                ui->listWidget_singerResult->setItemWidget(listItem, widget);
                connect(widget, SIGNAL(onDecideSingerClicked(QString)),
                        this, SLOT(handleDecideSingerButtonOnClicked(QString)));
                i += displayCount;
            }
        }
    }
}

void SongMenuWidget::handleRecommendColumnButtonOnClicked(QString currentColumn)
{
    SongMenuWidgetDebug("handleRecommendColumnButtonOnClicked");
    QPushButton* button = (QPushButton*)sender();
    if (priorClassifyButton == button)
    {
        SongMenuWidgetDebug("already in current column");
        return;
    }
    {
        ui->label_DataIsEmpty->hide();
        ui->widget_cacheing->hide();
        ui->widget_cacheing->setGeometry(22, 62, 557, 183);
        ui->label_hint->setGeometry(178, 82, 160, 23);
        ui->pushButton_retry->setGeometry(334, 82, 35, 23);
        ui->label_loading->setGeometry(263, 86, 30, 30);
    }
    priorClassifyButton->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bt.png);color: rgb(255, 255, 255);}");
    button->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bthover.png);color: rgb(255, 255, 255);}");
    priorClassifyButton = button;
    //not in this page
    RecommendColumnsPushButton* rButton = (RecommendColumnsPushButton*)button;
    //change in this page
    //step 1. get Recommend Column Songs
    for (int i = 0; i < letterIndexButtonList.length(); i ++)
    {
        letterIndexButtonList.at(i)->setEnabled(false);
        letterIndexButtonList.at(i)->setLetterEnabled(false);
    }
    allLetterIndexButton->setEnabled(false);
    allLetterIndexButton->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                                        "border-width:0;"
                                        "border-style:outset;"
                                        "font: 8pt \"Arial\";"
                                        "color: rgb(124, 103, 134);}");
    //clear show widget
    while(ui->listWidget_recommendResult->count() > 0)
    {
        QListWidgetItem *listItem = ui->listWidget_recommendResult->takeItem(0);
        ui->listWidget_recommendResult->removeItemWidget(listItem);
        delete listItem;
    }
    if (false == rButton->isGet)
    {
        currentGetSongButton = rButton;
        SongMenuWidgetDebug("start get songs");

        setLoadingWidget();
        HttpGetInfo httpGetInfo;
        httpGetInfo.setType(HttpGetInfo::PROG_SONG_LIST_INFO);
        httpGetInfo.setUrl(QUrl(ConfigHelper::getInstance()->getProgSongListPrefix() + currentColumn));
        currentHttpGetInfo = httpGetInfo;
        httpGet->request(httpGetInfo);
        SongMenuWidgetDebug("start get songs over");
        return;
    }
    //step 2. show Recommend Column Songs
    clearLetterIndexNumList();
    //add songs
    SongMenuWidgetDebug("rButton has %d songs", rButton->songNumList.length());
    dataIndex = 0;
    dataList = &rButton->songNumList;
    //close other column button
    for (int i = 0; i < columnsButtonList.length(); i ++)
    {
        if (columnsButtonList.at(i) != rButton)
        {
            columnsButtonList.at(i)->setEnabled(false);
        }
    }
    handleCommendWidgetOnRefreshData();
    for (int i = 0; i < rButton->songNumList.length(); i ++)
    {
        Song* song = allSongs.value(rButton->songNumList.at(i));
        if (song->getFirstLetter() >= 'A' && song->getFirstLetter() <= 'Z')
        {
            letterIndexNumList.value(song->getFirstLetter())->append(song->getSongID());
        }
    }
    //step 3. show letter index
    SongMenuWidgetDebug("show letter index");
    setLetterIndexState();
    priorLetterIndexButton = allLetterIndexButton;
}

void SongMenuWidget::handleDecideSongButtonOnClicked(QString songID)
{
    SongMenuWidgetDebug("num is %s song decided", songID.toUtf8().data());
    if (allSongs.contains(songID))
    {
        emit onSelectedSong(allSongs.value(songID));
        emit onQuit();
        return;
    }
    for (int i = 0; i < singerAllSongs.length(); i ++)
    {
        if (songID == singerAllSongs.at(i)->getSongID())
        {
            emit onSelectedSong(singerAllSongs.at(i));
            emit onQuit();
            return;
        }
    }
    SongMenuWidgetDebug("decide song failed ,because songsList has't this song");
}

void SongMenuWidget::handleDecideSingerButtonOnClicked(QString singerNum)
{
    SongMenuWidgetDebug("%s singer is clicked", singerNum);
    searchKey = allSingers.value(singerNum)->getSingerName();

    ui->widget_singerAllSong->show();
    while(ui->listWidget_singerAllSong->count() > 0)
    {
        QListWidgetItem *listItem = ui->listWidget_singerAllSong->takeItem(0);
        ui->listWidget_singerAllSong->removeItemWidget(listItem);
        delete listItem;
    }
    while(singerAllSongs.count() > 0)
    {
        Song* song = singerAllSongs.at(0);
        singerAllSongs.removeAt(0);
        delete song;
    }

    setLoadingWidget();
    HttpGetInfo httpGetInfo;
    httpGetInfo.setType(HttpGetInfo::SINGER_SONG_LIST_INFO);
    httpGetInfo.setUrl(QUrl(ConfigHelper::getInstance()->getSingerSongListPrefix() + singerNum));
    currentHttpGetInfo = httpGetInfo;
    httpGet->request(httpGetInfo);
}

void SongMenuWidget::clearLetterIndexNumList()
{
    for (char letter = 'A'; letter <= 'Z'; letter ++)
    {
        letterIndexNumList.value(letter)->clear();
    }
}

void SongMenuWidget::setLetterIndexState()
{
    allLetterIndexButton->setEnabled(true);
    allLetterIndexButton->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                                        "border-width:0;"
                                        "border-style:outset;"
                                        "font: 75 10pt \"Aharoni\";"
                                        "color: rgb(246, 229, 108);}");
    SongMenuWidgetDebug("setLetterIndexState");
    for (char letter = 'A'; letter <= 'Z'; letter ++)
    {
        if (letterIndexNumList.value(letter)->isEmpty())
        {
            letterIndexButtonList.value(letter - 'A')->setEnabled(false);
            letterIndexButtonList.value(letter - 'A')->setLetterEnabled(false);
        }
        else
        {
            letterIndexButtonList.value(letter - 'A')->setEnabled(true);
            letterIndexButtonList.value(letter - 'A')->setLetterEnabled(true);
        }
    }
}

void SongMenuWidget::changeSingerColumnWidget(int type)
{
    //show mansinger Info
    //step 1. get singers
    if (allSingers.isEmpty())
    {
        SongMenuWidgetDebug("get all singers info");
        setLoadingWidget();
        HttpGetInfo httpGetInfo;
        httpGetInfo.setType(HttpGetInfo::SINGER_LIST_INFO);
        httpGetInfo.setUrl(QUrl(ConfigHelper::getInstance()->getSingerListPrefix()));
        currentHttpGetInfo = httpGetInfo;
        httpGet->request(httpGetInfo);
        return;
    }
    //step 2. show singers
    SongMenuWidgetDebug("show %d singers", type);

    clearLetterIndexNumList();
    //clear Singer widget
    while(ui->listWidget_singerResult->count() > 0)
    {
        QListWidgetItem *listItem = ui->listWidget_singerResult->takeItem(0);
        ui->listWidget_singerResult->removeItemWidget(listItem);
        delete listItem;
    }

    singerDataIndex = 0;
    singerType = type;
    //disable button
    ui->pushButton_manSinger->setEnabled(false);
    ui->pushButton_womanSinger->setEnabled(false);
    ui->pushButton_singerGroup->setEnabled(false);
    ui->pushButton_foreignSinger->setEnabled(false);
    handleSingerWidgetOnRefreshData();
    for (int i = 0; i < singerNumSortList.length(); i ++)
    {
        Singer *singer = allSingers.value(singerNumSortList.at(i));
        SongMenuWidgetDebug("singer name %s , type %d" , singer->getSingerName().toUtf8().data() , singer->getType());
        if ((singer->getType() & type) == type)
        {
            if (singer->getFirstLetter() >= 'A' && singer->getFirstLetter() <= 'Z')
            {
                letterIndexNumList.value(singer->getFirstLetter())->append(singer->getSingNum());
            }
        }
    }

    //set letter index
    SongMenuWidgetDebug("show letter index");
    setLetterIndexState();
    allLetterIndexButton->setGetFocus();
    priorLetterIndexButton = allLetterIndexButton;
}

void SongMenuWidget::setSearchResultOrSingerSongsWidget(QString keys)
{
    ui->widget_cacheing->hide();
    ui->pushButton_retry->hide();
    ui->label_hint->hide();
    priorTabButton->setStyleSheet("color: rgb(255, 255, 255);"
                                    "background-image: url(:/image/SongSelectUI/nav.png);"
                                    "background-color: rgba(0,0,0,0);"
                                    "border-width:0;"
                                    "border-style:outset");
    ui->label_showSingerSongCount->setText(QString("<font color= #ffffff>   " +
                                           keys +
                                           " " +
                                           QString("共") +
                                           "</font>" +
                                           "<font color= #ff2fd1>" +
                                           QString::number(singerAllSongs.length()) +
                                           "</font>" +
                                           "<font color= #ffffff>" +
                                           QString("首") +
                                           "</font>"));
    dataIndex = 0;
    handleSearchResultWidgetOnRefreshData();
}

void SongMenuWidget::handleSearchButtonOnClicked()
{
    //搜索
    if (ui->lineEdit_search->text().isEmpty())
    {
        SongMenuWidgetDebug("search is empty!");
        return;
    }
    searchKey = ui->lineEdit_search->text();
    isSearchState = true;
    //get search songs
    ui->widget_singerAllSong->show();
    while(ui->listWidget_singerAllSong->count() > 0)
    {
        QListWidgetItem *listItem = ui->listWidget_singerAllSong->takeItem(0);
        ui->listWidget_singerAllSong->removeItemWidget(listItem);
        delete listItem;
    }
    while(singerAllSongs.count() > 0)
    {
        Song* song = singerAllSongs.at(0);
        singerAllSongs.removeAt(0);
        delete song;
    }
    setLoadingWidget();
    HttpGetInfo httpGetInfo;
    httpGetInfo.setType(HttpGetInfo::SONG_LIST_INFO);
    httpGetInfo.setUrl(QUrl(ConfigHelper::getInstance()->getSongSearchPrefix() + ui->lineEdit_search->text()));
    currentHttpGetInfo = httpGetInfo;
    httpGet->request(httpGetInfo);
}

void SongMenuWidget::handleRecommendButtonOnClicked()
{
    if (priorTabButton == ui->pushButton_recommend &&
            ui->widget_singerAllSong->isHidden())
    {
        if (!isSearchState)
        {
            SongMenuWidgetDebug("already in current page");
            return;
        }
    }
    if (isSearchState)
    {
         isSearchState = false;
    }
    {
        ui->label_DataIsEmpty->hide();
        ui->widget_cacheing->hide();
        ui->widget_cacheing->setGeometry(22, 62, 557, 183);
        ui->label_hint->setGeometry(178, 82, 160, 23);
        ui->pushButton_retry->setGeometry(334, 82, 35, 23);
        ui->label_loading->setGeometry(263, 86, 30, 30);
    }
    ui->widget_cacheing->hide();
    ui->pushButton_retry->hide();
    ui->label_hint->hide();
    allLetterIndexButton->setEnabled(false);
    allLetterIndexButton->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                                        "border-width:0;"
                                        "border-style:outset;"
                                        "font: 8pt \"Arial\";"
                                        "color: rgb(124, 103, 134);}");
    for (int i = 0; i < letterIndexButtonList.length(); i ++)
    {
        letterIndexButtonList.at(i)->setEnabled(false);
        letterIndexButtonList.at(i)->setLetterEnabled(false);
    }
    SongMenuWidgetDebug("Transition to recommend page");
    //set tab
    priorTabButton = ui->pushButton_recommend;
    ui->page_recommend->show();
    ui->page_recommendResult->show();
    ui->widget_singerAllSong->hide();
    ui->page_singer->hide();
    ui->page_singerResult->hide();
    ui->pushButton_recommend->setStyleSheet("QPushButton{color: rgb(255, 255, 255);"
                                            "background-image: url(:/image/SongSelectUI/navhover.png);"
                                            "background-color: rgba(0,0,0,0);"
                                            "border-width:0;"
                                            "border-style:outset}");
    ui->pushButton_singer->setStyleSheet("QPushButton{color: rgb(255, 255, 255);"
                                            "background-image: url(:/image/SongSelectUI/nav.png);"
                                            "background-color: rgba(0,0,0,0);"
                                            "border-width:0;"
                                            "border-style:outset}");
    //set Recommend Column
    if (priorClassifyButton)
        priorClassifyButton->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bt.png);"
                                       "color: rgb(255, 255, 255);}");
    if (firstClassifyButton)
    {
        firstClassifyButton->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bthover.png);"
                                       "color: rgb(255, 255, 255);}");

        priorClassifyButton = ui->pushButton_manSinger;
        firstClassifyButton->click();
    }
    else
    {
        setLoadingWidget();
        HttpGetInfo httpGetInfo;
        httpGetInfo.setType(HttpGetInfo::PROG_LIST_INFO);
        httpGetInfo.setUrl(QUrl(ConfigHelper::getInstance()->getProgListPrefix()));
        currentHttpGetInfo = httpGetInfo;
        httpGet->request(httpGetInfo);
    }
    priorClassifyButton = firstClassifyButton;
}

void SongMenuWidget::handleSingerButtonOnClicked()
{
    if (priorTabButton == ui->pushButton_singer && !isSearchState)
    {
        if (!ui->widget_singerAllSong->isHidden())
        {
            ui->pushButton_singer->setStyleSheet("color: rgb(255, 255, 255);"
                                            "background-image: url(:/image/SongSelectUI/navhover.png);"
                                            "background-color: rgba(0,0,0,0);"
                                            "border-width:0;"
                                            "border-style:outset");
            ui->widget_singerAllSong->hide();
        }
        SongMenuWidgetDebug("already in current page");
        return;
    }

    if (isSearchState)
    {
         isSearchState = false;
    }
    {
        ui->label_DataIsEmpty->hide();
        ui->widget_cacheing->hide();
        ui->widget_cacheing->setGeometry(22, 62, 557, 183);
        ui->label_hint->setGeometry(178, 82, 160, 23);
        ui->pushButton_retry->setGeometry(334, 82, 35, 23);
        ui->label_loading->setGeometry(263, 86, 30, 30);
    }
    ui->widget_cacheing->hide();
    ui->pushButton_retry->hide();
    ui->label_hint->hide();

    for (int i = 0; i < letterIndexButtonList.length(); i ++)
    {
        letterIndexButtonList.at(i)->setEnabled(false);
        letterIndexButtonList.at(i)->setLetterEnabled(false);
    }
    allLetterIndexButton->setEnabled(false);
    SongMenuWidgetDebug("Transition to singer page");
    //set tab
    priorTabButton = ui->pushButton_singer;
    ui->page_singer->show();
    ui->page_singerResult->show();
    ui->widget_singerAllSong->hide();
    ui->page_recommend->hide();
    ui->page_recommendResult->hide();
    ui->pushButton_singer->setStyleSheet("QPushButton{color: rgb(255, 255, 255);"
                                            "background-image: url(:/image/SongSelectUI/navhover.png);"
                                            "background-color: rgba(0,0,0,0);"
                                            "border-width:0;"
                                            "border-style:outset}");
    ui->pushButton_recommend->setStyleSheet("QPushButton{color: rgb(255, 255, 255);"
                                            "background-image: url(:/image/SongSelectUI/nav.png);"
                                            "background-color: rgba(0,0,0,0);"
                                            "border-width:0;"
                                            "border-style:outset}");
    //set Recommend Column
    if (priorClassifyButton != NULL)
    {
        priorClassifyButton->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bt.png);"
                                       "color: rgb(255, 255, 255);}");
    }
    ui->pushButton_manSinger->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bthover.png);"
                                                  "color: rgb(255, 255, 255);}");

    if (priorClassifyButton == ui->pushButton_manSinger)
    {
        priorClassifyButton = ui->pushButton_womanSinger;
    }
    //处理数据(获取男歌手数据)
    ui->pushButton_manSinger->click();
    priorClassifyButton = ui->pushButton_manSinger;
}

void SongMenuWidget::handleRetryButtonOnClicked()
{
    SongMenuWidgetDebug("handleRetryButtonOnClicked, try again");
    hasGetInfoFromHttp = false;
    timeCount = 0;
    httpGet->request(currentHttpGetInfo);
    this->startTimer(200);
    ui->widget_cacheing->show();
    ui->label_loading->show();
    loadingPixmapIndex = 1;
    loadingTimer->start(66);
    ui->label_hint->hide();
    ui->pushButton_retry->hide();
    ui->pushButton_recommend->setEnabled(false);
    ui->pushButton_singer->setEnabled(false);
    ui->pushButton_search->setEnabled(false);
    ui->lineEdit_search->setEnabled(false);
}

void SongMenuWidget::handleManSingerButtonOnClicked()
{
    if (priorClassifyButton == ui->pushButton_manSinger)
    {
        SongMenuWidgetDebug("already in current Classify");
        return;
    }
    {
        ui->label_DataIsEmpty->hide();
        ui->widget_cacheing->hide();
        ui->widget_cacheing->setGeometry(22, 62, 557, 183);
        ui->label_hint->setGeometry(178, 82, 160, 23);
        ui->pushButton_retry->setGeometry(334, 82, 35, 23);
        ui->label_loading->setGeometry(263, 86, 30, 30);
    }
    for (int i = 0; i < letterIndexButtonList.length(); i ++)
    {
        letterIndexButtonList.at(i)->setEnabled(false);
        letterIndexButtonList.at(i)->setLetterEnabled(false);
    }
    allLetterIndexButton->setEnabled(false);
    allLetterIndexButton->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                                        "border-width:0;"
                                        "border-style:outset;"
                                        "font: 8pt \"Arial\";"
                                        "color: rgb(124, 103, 134);}");
    SongMenuWidgetDebug("Transition to manSinger page");
    //set manSinger
    if (priorClassifyButton)
        priorClassifyButton->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bt.png);"
                                       "color: rgb(255, 255, 255);}");
    ui->pushButton_manSinger->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bthover.png);"
                                                  "color: rgb(255, 255, 255);}");
    priorClassifyButton = ui->pushButton_manSinger;

    //show mansinger Info
    changeSingerColumnWidget(eManSinger);
}

void SongMenuWidget::handleWomanSingerButtonOnClicked()
{
    if (priorClassifyButton == ui->pushButton_womanSinger)
    {
        SongMenuWidgetDebug("already in current Classify");
        return;
    }
    {
        ui->label_DataIsEmpty->hide();
        ui->widget_cacheing->hide();
        ui->widget_cacheing->setGeometry(22, 62, 557, 183);
        ui->label_hint->setGeometry(178, 82, 160, 23);
        ui->pushButton_retry->setGeometry(334, 82, 35, 23);
        ui->label_loading->setGeometry(263, 86, 30, 30);
    }
    for (int i = 0; i < letterIndexButtonList.length(); i ++)
    {
        letterIndexButtonList.at(i)->setEnabled(false);
        letterIndexButtonList.at(i)->setLetterEnabled(false);
    }
    allLetterIndexButton->setEnabled(false);
    allLetterIndexButton->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                                        "border-width:0;"
                                        "border-style:outset;"
                                        "font: 8pt \"Arial\";"
                                        "color: rgb(124, 103, 134);}");
    SongMenuWidgetDebug("Transition to womanSinger page");
    //set manSinger
    priorClassifyButton->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bt.png);"
                                       "color: rgb(255, 255, 255);}");
    ui->pushButton_womanSinger->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bthover.png);"
                                                  "color: rgb(255, 255, 255);}");
    priorClassifyButton = ui->pushButton_womanSinger;
    //show mansinger Info
    changeSingerColumnWidget(eWomanSinger);
}

void SongMenuWidget::handleSingerGroupButtonOnClicked()
{
    if (priorClassifyButton == ui->pushButton_singerGroup)
    {
        SongMenuWidgetDebug("already in current Classify");
        return;
    }
    {
        ui->label_DataIsEmpty->hide();
        ui->widget_cacheing->hide();
        ui->widget_cacheing->setGeometry(22, 62, 557, 183);
        ui->label_hint->setGeometry(178, 82, 160, 23);
        ui->pushButton_retry->setGeometry(334, 82, 35, 23);
        ui->label_loading->setGeometry(263, 86, 30, 30);
    }
    for (int i = 0; i < letterIndexButtonList.length(); i ++)
    {
        letterIndexButtonList.at(i)->setEnabled(false);
        letterIndexButtonList.at(i)->setLetterEnabled(false);
    }
    allLetterIndexButton->setEnabled(false);
    allLetterIndexButton->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                                        "border-width:0;"
                                        "border-style:outset;"
                                        "font: 8pt \"Arial\";"
                                        "color: rgb(124, 103, 134);}");
    SongMenuWidgetDebug("Transition to singerGroup page");
    //set manSinger
    priorClassifyButton->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bt.png);"
                                       "color: rgb(255, 255, 255);}");
    ui->pushButton_singerGroup->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bthover.png);"
                                                  "color: rgb(255, 255, 255);}");
    priorClassifyButton = ui->pushButton_singerGroup;
    //show mansinger Info
    changeSingerColumnWidget(eSingerGroup);
}

void SongMenuWidget::handleForeignSingerButtonOnClicked()
{
    if (priorClassifyButton == ui->pushButton_foreignSinger)
    {
        SongMenuWidgetDebug("already in current Classify");
        return;
    }
    {
        ui->label_DataIsEmpty->hide();
        ui->widget_cacheing->hide();
        ui->widget_cacheing->setGeometry(22, 62, 557, 183);
        ui->label_hint->setGeometry(178, 82, 160, 23);
        ui->pushButton_retry->setGeometry(334, 82, 35, 23);
        ui->label_loading->setGeometry(263, 86, 30, 30);
    }
    for (int i = 0; i < letterIndexButtonList.length(); i ++)
    {
        letterIndexButtonList.at(i)->setEnabled(false);
        letterIndexButtonList.at(i)->setLetterEnabled(false);
    }
    allLetterIndexButton->setEnabled(false);
    allLetterIndexButton->setStyleSheet("QPushButton{background-color: rgba(0,0,0,0);"
                                        "border-width:0;"
                                        "border-style:outset;"
                                        "font: 8pt \"Arial\";"
                                        "color: rgb(124, 103, 134);}");
    SongMenuWidgetDebug("Transition to foreignSinger page");
    //set manSinger
    priorClassifyButton->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bt.png);"
                                       "color: rgb(255, 255, 255);}");
    ui->pushButton_foreignSinger->setStyleSheet("QPushButton{image: url(:/image/SongSelectUI/bthover.png);"
                                                  "color: rgb(255, 255, 255);}");
    priorClassifyButton = ui->pushButton_foreignSinger;
    //show mansinger Info
    changeSingerColumnWidget(eForeignSinger);
}

void SongMenuWidget::handleCommendWidgetOnRefreshData()
{
    int startIndex = dataIndex;
    for (int i = startIndex; (i < startIndex + 1) && (i < dataList->length()); i ++)
    {
        QString num = dataList->at(i);
        Song*song = allSongs.value(num);

        QListWidgetItem *listItem = new QListWidgetItem(ui->listWidget_recommendResult);
        listItem->setSizeHint(QSize(420, 34));
        SongWidget *widget = new SongWidget(song->getSongID(),
                                            song->getName(),
                                            song->getSinger(),
                                            song->getSectionCount());
        connect(widget, SIGNAL(onDecideSongClicked(QString)),
                this, SLOT(handleDecideSongButtonOnClicked(QString)));
        ui->listWidget_recommendResult->setItemWidget(listItem, widget);
        dataIndex++;
    }
    if(dataIndex < dataList->length())
    {
        QTimer::singleShot(5 , this , SLOT(handleCommendWidgetOnRefreshData()));
    }
    else
    {
        //open other column button
        for (int i = 0; i < columnsButtonList.length(); i ++)
        {
            columnsButtonList.at(i)->setEnabled(true);
        }
    }
}

void SongMenuWidget::handleSearchResultWidgetOnRefreshData()
{
    int startIndex = dataIndex;
    for (int i = startIndex; (i < startIndex + 1) &&  i < singerAllSongs.length(); i ++)
    {
        QListWidgetItem *listItem = new QListWidgetItem(ui->listWidget_singerAllSong);
        listItem->setSizeHint(QSize(540, 34));
        SearchResultWidget *widget = new SearchResultWidget(singerAllSongs.at(i)->getSongID(),
                                                            singerAllSongs.at(i)->getName(),
                                                            singerAllSongs.at(i)->getSinger(),
                                                            singerAllSongs.at(i)->getSectionCount());
        connect(widget, SIGNAL(onDecideSongClicked(QString)),
                this, SLOT(handleDecideSongButtonOnClicked(QString)));
        ui->listWidget_singerAllSong->setItemWidget(listItem, widget);
        dataIndex++;
    }
    if(dataIndex < singerAllSongs.length())
    {
        QTimer::singleShot(5 , this , SLOT(handleSearchResultWidgetOnRefreshData()));
    }
}

void SongMenuWidget::handleSingerWidgetOnRefreshData()
{
    SongMenuWidgetDebug("handleSingerWidgetOnRefreshData");

    bool hasLetter = true;
    int displayCount = 0;
    QList<QString>   singNumList;
    QList<QString>   singNameList;
    int startIndex = singerDataIndex;
    char currentLetter = allSingers.value(singerNumSortList.at(startIndex))->getFirstLetter();

    for (int i = startIndex; i < singerNumSortList.length(); i ++)
    {
        Singer *singer = allSingers.value(singerNumSortList.at(i));
        if (singer->getFirstLetter() != currentLetter)
        {
            //重复
            break;
        }
        if ((singer->getType() & singerType) == singerType)
        {
            if (singer->getFirstLetter() >= 'A' && singer->getFirstLetter() <= 'Z')
            {
                //display
                displayCount ++;
                singNameList.append(singer->getSingerName());
                singNumList.append(singer->getSingNum());
                if (4 == displayCount)
                {
                    //显示
                    QListWidgetItem *listItem = new QListWidgetItem(ui->listWidget_singerResult);
                    listItem->setSizeHint(QSize(420, 34));
                    bool b = hasLetter;
                    if (hasLetter)
                    {
                        hasLetter = false;
                    }
                    SingerWidget *widget = new SingerWidget(singNumList,
                                                        singNameList,
                                                        displayCount,
                                                        b,
                                                        currentLetter);
                    ui->listWidget_singerResult->setItemWidget(listItem, widget);
                    connect(widget, SIGNAL(onDecideSingerClicked(QString)),
                            this, SLOT(handleDecideSingerButtonOnClicked(QString)));
                    displayCount = 0;
                    singNameList.clear();
                    singNumList.clear();
                }
            }
            else
            {
                //only record
            }
        }
        singerDataIndex++;
    }
    if (displayCount != 0)//说明里面还有数据
    {
        //显示
        QListWidgetItem *listItem = new QListWidgetItem(ui->listWidget_singerResult);
        listItem->setSizeHint(QSize(420, 34));
        SingerWidget *widget = new SingerWidget(singNumList,
                                                singNameList,
                                                displayCount,
                                                hasLetter,
                                                currentLetter);
        ui->listWidget_singerResult->setItemWidget(listItem, widget);
        connect(widget, SIGNAL(onDecideSingerClicked(QString)),
                this, SLOT(handleDecideSingerButtonOnClicked(QString)));
    }
    if (singerDataIndex < singerNumSortList.length())
    {
        QTimer::singleShot(5 , this , SLOT(handleSingerWidgetOnRefreshData()));
    }
    else
    {
        //Restore button
        ui->pushButton_manSinger->setEnabled(true);
        ui->pushButton_womanSinger->setEnabled(true);
        ui->pushButton_singerGroup->setEnabled(true);
        ui->pushButton_foreignSinger->setEnabled(true);
    }
}

void SongMenuWidget::handleHttpGetOnProgListInfo(bool isSuccess, QList<Prog> progs)
{
    SongMenuWidgetDebug("handleHttpGetOnProgListInfo");
    if (!isSuccess)
    {
        SongMenuWidgetDebug("get ProgList failed");
        return;
    }
    if (! recommendColumn.isEmpty())
    {
        SongMenuWidgetDebug("has progList again");
        return;
    }
    if (progs.isEmpty())
    {
        SongMenuWidgetDebug("has progList empty");
        return;
    }
    hasGetInfoFromHttp = true;

    for (int i = 0; i < progs.length(); i ++)
    {
        Prog *p1 = new Prog(progs.at(i).getColumnNum(), progs.at(i).getColumnName(), i);
        recommendColumn.insert(progs.at(i).getColumnNum(), p1);
    }
    SongMenuWidgetDebug("Get ProgList Success");
    //step 2. set recommend column widget
    int buttonHight = 10;
    QMap<QString, Prog*>::iterator it = recommendColumn.begin();
    for (int i = 0; it != recommendColumn.end(); i ++, it ++)
    {
        RecommendColumnsPushButton *button =
                new RecommendColumnsPushButton(it.value()->getColumnNum(), it.value()->getColumnName(), ui->page_recommend);
        button->move(10, buttonHight);
        buttonHight += 48;
        button->resize(98, 32);
        columnsButtonList.append(button);
        QObject::connect(button, SIGNAL(onClicked(QString)),
                         this, SLOT(handleRecommendColumnButtonOnClicked(QString)));
        SongMenuWidgetDebug("create %s button", it.value()->getColumnName().toUtf8().data());

        if (it.value()->getButtonOrder() == 0)
        {
            firstClassifyButton = button;
            SongMenuWidgetDebug("set firstClassifyButton");
        }
    }
    //加载初始数据
    priorClassifyButton = ui->pushButton_manSinger;
//    priorTabButton = ui->pushButton_singer;
    SongMenuWidgetDebug("loading top 100 data");
    firstClassifyButton->click();
    priorClassifyButton = firstClassifyButton;
    SongMenuWidgetDebug("loading top 100 data over");
}

void SongMenuWidget::handleHttpGetOnProgSongListInfo(bool isSuccess, QList<Song> songs)
{
    SongMenuWidgetDebug("handleHttpGetOnProgSongListInfo");
    if (!isSuccess)
    {
        SongMenuWidgetDebug("get ProgSongList failed");
        return;
    }
    if (currentGetSongButton->isGet)
    {
        SongMenuWidgetDebug("has ProgSongList again");
        return;
    }
    hasGetInfoFromHttp = true;
    if (songs.isEmpty())
    {
        getInfoFromHttpIsEmpty = true;
        SongMenuWidgetDebug("has songs empty");
        return;
    }

    currentGetSongButton->isGet = true;
    for (int i = 0; i < songs.length(); i ++)
    {
        Song* s1 = new Song();
        s1->setSongID(songs.at(i).getSongID());
        s1->setName(songs.at(i).getName());
        s1->setSinger(songs.at(i).getSinger());
        s1->setSectionCount(songs.at(i).getSectionCount());
        s1->setFirstLetter(songs.at(i).getFirstLetter());
        if (! allSongs.contains(songs.at(i).getSongID()))
        {
            allSongs.insert(songs.at(i).getSongID(), s1);
        }
        currentGetSongButton->songNumList.append(songs.at(i).getSongID());
    }
    SongMenuWidgetDebug("get ProgSongList success");

    //step 2. show Recommend Column Songs
    clearLetterIndexNumList();
    //clear show widget
    while(ui->listWidget_recommendResult->count() > 0)
    {
        QListWidgetItem *listItem = ui->listWidget_recommendResult->takeItem(0);
        ui->listWidget_recommendResult->removeItemWidget(listItem);
        delete listItem;
    }
    //add songs
    SongMenuWidgetDebug("rButton has %d songs", currentGetSongButton->songNumList.length());
    dataIndex = 0;
    dataList = &currentGetSongButton->songNumList;
    //close other column button
    for (int i = 0; i < columnsButtonList.length(); i ++)
    {
        if (columnsButtonList.at(i) != currentGetSongButton)
        {
            columnsButtonList.at(i)->setEnabled(false);
        }
    }
    handleCommendWidgetOnRefreshData();
    for (int i = 0; i < currentGetSongButton->songNumList.length(); i ++)
    {
        Song* song = allSongs.value(currentGetSongButton->songNumList.at(i));

        if (song->getFirstLetter() >= 'A' && song->getFirstLetter() <= 'Z')
            letterIndexNumList.value(song->getFirstLetter())->append(song->getSongID());
    }
    //step 3. show letter index
    SongMenuWidgetDebug("show letter index");
    setLetterIndexState();
    priorLetterIndexButton = allLetterIndexButton;
}

void SongMenuWidget::handleHttpGetOnSingerListInfo(bool isSuccess, QList<Singer> singers)
{
    SongMenuWidgetDebug("handleHttpGetOnSingerListInfo");
    if (!isSuccess)
    {
        SongMenuWidgetDebug("get SingerList failed");
        return;
    }
    if (! allSingers.isEmpty())
    {
        SongMenuWidgetDebug("has SingerList again");
        return;
    }
    hasGetInfoFromHttp = true;
    if (singers.isEmpty())
    {
        getInfoFromHttpIsEmpty = true;
        SongMenuWidgetDebug("has singers empty");
        return;
    }
    ui->widget_cacheing->hide();
    for (int i = 0; i < singers.length(); i ++)
    {
        Singer* s1 = new Singer(singers.at(i).getSingNum(), singers.at(i).getSingerName(), singers.at(i).getFirstLetter(), singers.at(i).getType());

        singerNumSortList.append(singers.at(i).getSingNum());
        allSingers.insert(singers.at(i).getSingNum(), s1);
    }
    SongMenuWidgetDebug("get SingerList success");

    changeSingerColumnWidget(eManSinger);
}

void SongMenuWidget::handleHttpGetOnSingerSongListInfo(bool isSuccess, QList<Song> singerSong)
{
    SongMenuWidgetDebug("handleHttpGetOnSingerListInfo");
    if (!isSuccess)
    {
        SongMenuWidgetDebug("get SingerSongList failed");
        return;
    }
    if (! singerAllSongs.isEmpty())
    {
        SongMenuWidgetDebug("has SingerSongList again");
        return;
    }

    hasGetInfoFromHttp = true;

    SongMenuWidgetDebug("singerSong has %d", singerSong.length());
    for (int i = 0; i < singerSong.length(); i ++)
    {
        Song* s1 = new Song();
        s1->setSongID(singerSong.at(i).getSongID());
        s1->setName(singerSong.at(i).getName());
        s1->setSinger(searchKey);
        s1->setSectionCount(singerSong.at(i).getSectionCount());
        s1->setFirstLetter(singerSong.at(i).getFirstLetter());
        singerAllSongs.append(s1);
    }
    setSearchResultOrSingerSongsWidget(searchKey);
}

void SongMenuWidget::handleHttpGetOnSongList(bool isSuccess, QList<Song> songs)
{
    SongMenuWidgetDebug("handleHttpGetOnSongList");
    if (!isSuccess)
    {
        SongMenuWidgetDebug("get searchSongList failed");
        return;
    }
    if (searchKey != ui->lineEdit_search->text())
    {
        SongMenuWidgetDebug("has searchSongList again");
        return;
    }

    hasGetInfoFromHttp = true;
    for (int i = 0; i < songs.length(); i ++)
    {
        Song* s1 = new Song();
        s1->setSongID(songs.at(i).getSongID());
        s1->setName(songs.at(i).getName());
        s1->setSinger(songs.at(i).getSinger());
        s1->setSectionCount(songs.at(i).getSectionCount());
        singerAllSongs.append(s1);
    }
    setSearchResultOrSingerSongsWidget(searchKey);
}

void SongMenuWidget::setLoadingWidget()
{
    hasGetInfoFromHttp = false;
    getInfoFromHttpIsEmpty = false;
    timeCount = 0;
    this->startTimer(200);
    ui->widget_cacheing->show();
    ui->label_loading->show();
    ui->label_DataIsEmpty->hide();
    loadingPixmapIndex = 1;
    loadingTimer->start(66);
    ui->label_hint->hide();
    ui->pushButton_retry->hide();
    ui->pushButton_recommend->setEnabled(false);
    ui->pushButton_singer->setEnabled(false);
    ui->pushButton_search->setEnabled(false);
    ui->lineEdit_search->setEnabled(false);
}

void SongMenuWidget::handleLoadingTimerOnTimerOut()
{
    if (30 == loadingPixmapIndex)
    {
        loadingPixmapIndex = 1;
    }
    QPixmap pix(QString(":/image/SongSelectUI/juhuazhuan/jz (%1)").arg(loadingPixmapIndex));
    ui->label_loading->setPixmap(pix);
    loadingPixmapIndex ++;
}

void SongMenuWidget::timerEvent(QTimerEvent *e)
{
    if (hasGetInfoFromHttp)
    {
        this->killTimer(e->timerId());
        if (! getInfoFromHttpIsEmpty)
        {
            ui->widget_cacheing->hide();
        }
        else
        {
            ui->widget_cacheing->setGeometry(130, 62, 449, 183);
            ui->label_DataIsEmpty->show();
        }
        ui->label_loading->hide();
        loadingTimer->stop();
        ui->label_hint->hide();
        ui->pushButton_retry->hide();
        ui->pushButton_recommend->setEnabled(true);
        ui->pushButton_singer->setEnabled(true);
        ui->pushButton_search->setEnabled(true);
        ui->lineEdit_search->setEnabled(true);
    }
    if (25 == timeCount)//超时5S
    {
        //扔掉请求
        this->killTimer(e->timerId());
        ui->label_loading->hide();
        if (priorTabButton == ui->pushButton_recommend)
        {
            ui->widget_cacheing->setGeometry(130, 62, 449, 183);
            ui->label_hint->setGeometry(116, 82, 160, 23);
            ui->pushButton_retry->setGeometry(274, 82, 35, 23);
            ui->label_loading->setGeometry(200, 86, 30, 30);
        }
        loadingTimer->stop();
        ui->label_hint->show();
        ui->pushButton_retry->show();
        ui->pushButton_recommend->setEnabled(true);
        ui->pushButton_singer->setEnabled(true);
        ui->pushButton_search->setEnabled(true);
        ui->lineEdit_search->setEnabled(true);
    }
    timeCount ++;
}

void SongMenuWidget::show()
{
    ui->pushButton_recommend->click();
    QWidget::show();
}
