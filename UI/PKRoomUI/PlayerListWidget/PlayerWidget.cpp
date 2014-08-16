#include "PlayerWidget.h"
#include "ui_PlayerWidget.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include <QFile>
#include <QBitmap>
#include "Modules/LyricParse/LyricParse.h"
#define RESOURCE_PAINT_IMAGE ":/image/images/paint.gif"
#define ANIMATION_DURATION 300

PlayerWidget::PlayerWidget(const QString& playerName, QWidget *parent , SingState state,Location loc) :
    QWidget(parent),
    ui(new Ui::PlayerWidget)
{

    ui->setupUi(this);
    this->state = state;
    configSingWidget(playerName);
    configUI(loc);
    movie = new QMovie(this);
    movie->setFileName("://image/PKRoomUI/gif_music.gif");
    ui->musicLabel->setMovie(movie);
    movie->start();
}

void PlayerWidget::configSingWidget(const QString& playerName)
{
    playerWidgetDebug("PlayerWidget configSingWidget");
    Player player;
    player.setName(playerName);
    int hostNameLen = 60;
    ui->nameLabel->setText(player.getShortCutName(ui->nameLabel->font(),hostNameLen));
}

void PlayerWidget::configUI(Location loc)
{
    playerWidgetDebug("PlayerWidget configUI");
    QString bgPath;
    switch(loc)
    {
    case HEAD:
        bgPath = "://image/PKRoomUI/headLoc.png";
        break;
    case MIDDLE:
        bgPath = "://image/PKRoomUI/MiddleLoc.png";
        break;
    case TAIL:
        bgPath = "://image/PKRoomUI/TailLoc.png";
        break;
    default:
        bgPath = "://image/PKRoomUI/headLoc.png";
        break;
    }
    ui->frame->setObjectName("playerFrame");
    ui->frame->setStyleSheet(QString("#playerFrame{border-image:url(%1);}").arg(bgPath));

    ui->musicLabel->hide();
    if(loc == HEAD)
    {
        ui->musicLabel->setGeometry(ui->musicLabel->x() + 9 , ui->musicLabel->y() , ui->musicLabel->width() ,ui->musicLabel->height());
        ui->nameLabel->move(ui->nameLabel->x()+9, ui->nameLabel->y());
        ui->scoreLabel->move(ui->scoreLabel->x() + 9 , ui->scoreLabel->y());
    }

    QString nameStyleSheet = ui->nameLabel->styleSheet() + ";color: #ffffff;";
    ui->nameLabel->setStyleSheet(nameStyleSheet);

    ui->scoreLabel->setText("");

    QPixmap pix(bgPath);
    this->setGeometry(this->x() , this->y() , pix.width() , pix.height());
    ui->frame->setGeometry(0 , 0 , pix.width() , pix.height());
}



void PlayerWidget::changeState(const SingState &value)
{
     playerWidgetDebug("PlayerWidget changeState");
    state = value;
    if(state == SINGING)
    {
        ui->musicLabel->show();
        ui->nameLabel->setStyleSheet(QString::fromLocal8Bit("font:75 12px \"宋体\";color:#24feff"));
        ui->scoreLabel->show();
    }
    else
    {
        ui->musicLabel->hide();
        ui->nameLabel->setStyleSheet(QString::fromLocal8Bit("font:75 12px \"宋体\";color:#ffffff"));
    }
}


PlayerWidget::~PlayerWidget()
{
    delete ui;
}


void PlayerWidget::updateScore(int score)
{
    playerWidgetDebug("PlayerWidget updateScore");
    ui->scoreLabel->setText(QString("%1").arg(score));
}



