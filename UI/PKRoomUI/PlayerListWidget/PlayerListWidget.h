#ifndef PLAYERLISTWIDGET_H
#define PLAYERLISTWIDGET_H

#include <QWidget>
#include <QList>
#include <QPoint>

#include "Player/Player.h"
#include "PlayerWidget.h"

#include <QParallelAnimationGroup>
#include "Modules/PkInfoManager/PkInfoManager.h"
#define WIDGET_MARGIN   1

#define DEBUG_PLAYER_LIST_WIDGET  1

#if DEBUG_PLAYER_LIST_WIDGET
#include <QDebug>
#define playerListWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define playerListWidgetDebug(format,...)
#endif

namespace Ui {
class PlayerListWidget;
}

class PlayerListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerListWidget(QWidget *parent = 0);
    ~PlayerListWidget();

    void updatePlayerList(QList<Player> &playerList);
    void updateCurrentSinger(int singerIndex, int lastSinggerIndex);
    void updatePlayerScore(int currentIndex, Score);

private:
    QList<PlayerWidget*> playerWidgetList;
private:
    Ui::PlayerListWidget *ui;

};

#endif // PLAYERLISTWIDGET_H
