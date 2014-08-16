#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QWidget>
#include <QMovie>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>
#include "HttpDownload/HttpDownload.h"
#include "Player/Player.h"
#include "Modules/LyricParse/LyricParse.h"
#include <QMovie>
#define DEBUG_PLAYER_WIDGET  1

#if DEBUG_PLAYER_WIDGET
#include <QDebug>
#define playerWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define playerWidgetDebug(format,...)
#endif

#define WAIT_WIDGET_WIDTH  116
#define WAIT_WIDGET_HEIGHT 51
#define SING_WIDGET_WIDTH  157
#define SING_WIDGET_HEIGHT 55



namespace Ui {
class PlayerWidget;
}

class PlayerWidget:public QWidget
{
    Q_OBJECT


public:
    typedef enum
    {
        SINGING,
        WAIT
    }SingState;

    typedef enum
    {
        HEAD,
        MIDDLE,
        TAIL
    }Location;
    explicit PlayerWidget(const QString& playerName, QWidget *parent = 0 ,SingState state = WAIT ,Location loc = HEAD);
    ~PlayerWidget();

    void           changeState(const SingState &value);
    void           updateScore(int score);
private:
    void           configSingWidget(const QString& playerName);
    void           configUI(Location loc);
private:
    Player            player;
    SingState         state;
    Ui::PlayerWidget  *ui;
    QMovie            *movie;

};


#endif // PLAYERWIDGET_H
