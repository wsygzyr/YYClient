#include "AudienceInvitePKWidget.h"
#include "ui_AudienceInvitePKWidget.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include <QCheckBox>
#include <QPainter>
#include <QBitmap>
#include <Windows.h>
#include "Modules/StateController/StateController.h"
AudienceInvitePKWidget::AudienceInvitePKWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudienceInvitePKWidget)
{
    ui->setupUi(this);
    //set icon of application in the taskbar
    this->setWindowIcon(QIcon("://image/AppIcon/icon_PK.ico"));

    configUI();

    pixmap = new QPixmap("://image/AudienceInvitePKWidget/bg_invite.png");
    this->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint
                   | Qt::WindowSystemMenuHint | Qt::WindowMinimizeButtonHint
                   | Qt::WindowMaximizeButtonHint );
    ::BringWindowToTop((HWND)this->winId());
    info = PkInfoManager::getInstance();


     bMouseLeftBtnDown = false;
}

void AudienceInvitePKWidget::configUI()
{
    AudienceInvitePKWidgetDebug("AudienceInvitePKWidget configUI");
    for(int i = 0 ; i < ui->PropListWidget->columnCount() ; i++)
    {
        ui->PropListWidget->setColumnWidth(i , 80);
        ui->PropListWidget->setRowHeight(0 , 70);
    }
    setWindowFlags(Qt::FramelessWindowHint);

    for(int row = 0;row < ui->PropListWidget->rowCount(); row++)
    {
        for(int column = 0;column < ui->PropListWidget->columnCount(); column++)
        {
            QTableWidgetItem *item =  ui->PropListWidget->takeItem(row,column);
            if(item)
            {
                delete item;
                item = NULL;
            }
        }
    }
}

void AudienceInvitePKWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, *pixmap);//绘制图像

    QWidget::paintEvent(event);
}

AudienceInvitePKWidget::~AudienceInvitePKWidget()
{
    delete ui;
    if(pixmap)
    {
        delete pixmap;
        pixmap = NULL;
    }
}


void AudienceInvitePKWidget::configRewardList(int index)
{
    AudienceInvitePKWidgetDebug("AudienceInvitePKWidget configRewardList");
    if(info->getVictoryRewardList().size() == 6 )
    {
        if(index == 4 || index == 5)
        {
            configRewardItem(index);
        }
    }else  if(info->getVictoryRewardList().size() == 5)
    {
        if(index == 1 || index == 4)
        {
            configRewardItem(index);
        }
    }else
    {
        if(index == 1 || index == 2)
        {
            configRewardItem(index);
        }
    }
    if(index == 0)
    {
        configRewardItem(index);
    }
}

void AudienceInvitePKWidget::configRewardItem(int index)
{
    AudienceInvitePKWidgetDebug("AudienceInvitePKWidget configRewardItem");
    QString labelStr = QString("<img src=\"://image/AudienceInvitePKWidget/reward%1.png\"/><br/><font color=\"#ffffff\">%2</font>").arg(info->getVictoryRewardList()[index].getID())
            .arg(info->getVictoryRewardList()[index].getName());

            QLabel *label = new QLabel(labelStr);
            QFont font;
            font.setPixelSize(12);
            font.setWeight(75);
            font.setBold(false);
            font.setFamily("宋体");
            label->setFont(font);
            label->setAlignment(Qt::AlignHCenter);
            ui->PropListWidget->setCellWidget(0 ,info->getVictoryRewardList().size() - index - 1    , label);
}

//to update the controls' display info in UI
void AudienceInvitePKWidget::setUIControlsInfo( Song song,
                                               int acceptedPlayers)
{
    AudienceInvitePKWidgetDebug("AudienceInvitePKWidget setUIControlsInfo");
    int sectionCount = song.getSectionCount();
    int width = 97;
    QString songName = song.getShortCutSongName(ui->InviteSongInfoLabel->font() , width);
    QString songStr = QString("歌曲《%1》   斗歌人数:%2人").arg(songName).arg(sectionCount);
    ui->InviteSongInfoLabel->setText(songStr);
    songStr = QString("已有<font color=\"#f9d12a\">%1</font>人要玩").arg(acceptedPlayers);
    ui->InviteNumberInfoLabel->setText(songStr);

    for(int i = info->getVictoryRewardList().size() - 1; i >= info->getVictoryRewardList().size() - 4; i--)
    {
        configRewardItem(i);
    }
}

void AudienceInvitePKWidget::show()
{

    QWidget::show();
}

void AudienceInvitePKWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(event->y()>30||event->x()>601)
        {
            event->ignore();
            return;
        }
        this->windowPos = this->pos();
        this->mousePos = event->globalPos();
        this->dPos = mousePos - windowPos;
        this->bMouseLeftBtnDown = true;
    }
    event->ignore();
}

void AudienceInvitePKWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (bMouseLeftBtnDown)
    {
        this->move(event->globalPos() - this->dPos);
    }
}

void AudienceInvitePKWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        bMouseLeftBtnDown = false;
    }
    event->ignore();
}

bool AudienceInvitePKWidget::winEvent(MSG *message, long *result)
{
    if(message->message == WM_SYSCOMMAND && message->wParam == SC_CLOSE)
    {
        this->hide();
        return true;
    }
    return false;
}
void AudienceInvitePKWidget::on_JoinBtn_clicked()
{
    YYNotificationCenter* yync = YYNotificationCenter::getInstance();
    yync->sendAudienceJoinGame();

    StateController *stateCtrl = StateController::getInstance();
    if(stateCtrl->sendStateAcceptInvite())
    {
        this->hide();
    }
    else
    {
        AudienceInvitePKWidgetDebug("AudienceInvitePKWidget accepte pkinvite failed");
    }
}



