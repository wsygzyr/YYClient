#include "PlayerListWidget.h"
#include "ui_PlayerListWidget.h"

#include "Modules/ConfigHelper/ConfigHelper.h"
PlayerListWidget::PlayerListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerListWidget)
{
    ui->setupUi(this);
}

PlayerListWidget::~PlayerListWidget()
{
    delete ui;
}

void PlayerListWidget::updatePlayerList(QList<Player> &playerList)
{
    playerListWidgetDebug("PlayerListWidget updatePlayerList");
    while(!playerWidgetList.empty())
    {
        delete playerWidgetList.takeFirst();
    }
    QPoint point = QPoint(0,0);
    for(int i = 0; i< playerList.size(); i++)
    {
        PlayerWidget::Location loc;
        if( i == 0)
            loc = PlayerWidget::HEAD;
        else if (i == playerList.size() - 1)
            loc = PlayerWidget::TAIL;
        else
            loc = PlayerWidget::MIDDLE;

        PlayerWidget * singWidget = new PlayerWidget(playerList[i].getName() ,this ,PlayerWidget::WAIT ,loc);
        singWidget->setGeometry(point.x() , point.y() , singWidget->width() , singWidget->height());
        point.setX(point.x() + singWidget->width());
        playerWidgetList.append(singWidget);
        singWidget->show();
    }
}

void PlayerListWidget::updateCurrentSinger(int singerIndex ,int lastSinggerIndex)
{
    playerListWidgetDebug("PlayerListWidget updateCurrentSinger %d", playerWidgetList.size());
    if( playerWidgetList.size() == 0 )
        return;

    playerWidgetList[singerIndex]->changeState(PlayerWidget::SINGING);
    if(singerIndex != 0 && lastSinggerIndex != -1)
        playerWidgetList[lastSinggerIndex]->changeState(PlayerWidget::WAIT);

}


void PlayerListWidget::updatePlayerScore(int currentIndex , Score score)
{
    playerWidgetList[currentIndex]->updateScore(score.getTotalScore());
}


