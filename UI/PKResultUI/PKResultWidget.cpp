#include "PKResultWidget.h"
#include "ui_PKResultWidget.h"
#include <QSignalMapper>
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "QHoverWidget.h"

#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/Animator/Animator.h"
#include <QSound>
#include "Modules/StateController/StateController.h"
PKResultWidget::PKResultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PKResultWidget)
{
    ui->setupUi(this);
    connect(ui->RewardTableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(handleRewardTableWidgetClicked(int, int)));
    info = PkInfoManager::getInstance();
    RewardBackGroundMask = new RotateWidget(QString("://image/PKResultUI/img_blight.png") , ui->RewardBackGroundMask);
    quitAudienceTimerId = 0;

}

PKResultWidget::~PKResultWidget()
{
    delete ui;
}

void PKResultWidget::updateResult(QList<Player> playerList, int heat, Prop propIncome)
{
    PKResultWidgetDebug("PKResultWidget updateResult");
    reset();
    firstPricePlayer = playerList[0];
    if(firstPricePlayer.getID() == info->getMe().getID() && !info->getMe().getIsHost())
    {
        time = 0;
        AutoSelectReward();
    }

    PKResultWidgetDebug("updateResult id:%d",playerList[0].getID());

    updateRankList(playerList);
    updateRewardAndTip(playerList, heat, propIncome);
}

void PKResultWidget::updateSelectedVictoryReward(VictoryReward vic)
{
    PKResultWidgetDebug("PKResultWidget updateSelectedVictoryReward");
    QList<VictoryReward> vicList = info->getVictoryRewardList();
    for(int i = 0; i < vicList.size(); i++ )
    {
        if(vic.getID() == vicList[i].getID())
        {
            vic = vicList[i];
            break;
        }
    }
    if(firstPricePlayer.getID() != info->getMe().getID())
    {
        ui->RewardTableWidget->hide();
        ui->waitSelectedTextLabel->hide();
        setRewardBg(true);
        int hostNameLen = 60;
        if(info->getMe().getIsHost())
        {
            ui->seclectedTipLabel->setText(QString("<font color = \"#ffd217\">%1</font> 选择了<font color = \"#ffd217\"> %2 </font>，<br/>快去表演吧，大家都等不及了！").arg(firstPricePlayer.getShortCutName(ui->seclectedTipLabel->font(), hostNameLen)).arg(vic.getName()));
            ui->quitButton->setStyleSheet(QString("QPushButton:hover{border-image: url(://image/PKResultUI/bt_qubiaoyan_hover.png);}"
                                                  "QPushButton{border-image: url(://image/PKResultUI/bt_qubiaoyan.png);}"));

        }else
        {
            ui->seclectedTipLabel->setText(QString("<font color = \"#ffd217\">%1</font> 选择了 <font color = \"#ffd217\">%2</font>，<br/>速速围观主播的表演！！").arg(firstPricePlayer.getShortCutName(ui->seclectedTipLabel->font(), hostNameLen)).arg(vic.getName()));

            ui->quitButton->setStyleSheet(QString("QPushButton:hover{border-image: url(://image/PKResultUI/bt_weiguan_hover.png);}"
                                                  "QPushButton{border-image: url(://image/PKResultUI/bt_weiguan.png);}"));

        }

        ui->seclectedLabel->setStyleSheet(QString("border-image: url(://image/PKResultUI/img_reword0%1_big.png);").arg(vic.getID()));
        ui->quitButton->show();
        ui->seclectedLabel->show();
        ui->seclectedTipLabel->show();
    }
}

void PKResultWidget::startAni(QString path)
{
    PKResultWidgetDebug("PKResultWidget startAni");
    ConfigHelper *cfg = ConfigHelper::getInstance();
    Animator *ani = new Animator;
    ani->Animate(path , "flame" ,this ,this->geometry() ,25 , true);
    QSound::play(cfg->getPluginPath() + "Sound\\PropSound\\cheer.wav" );

}

void PKResultWidget::reset()
{
    PKResultWidgetDebug("PKResultWidget reset");
    setRewardBg(false);
    for(int row = 0;row < ui->RankListWidget->rowCount(); row++)
    {
        for(int column = 0;column < ui->RankListWidget->columnCount(); column++)
        {
            QTableWidgetItem *item =  ui->RankListWidget->takeItem(row,column);
            if(item)
            {
                delete item;
                item = NULL;
            }
        }
    }

    for(int row = 0;row < ui->RewardTableWidget->rowCount(); row++)
    {
        for(int column = 0;column < ui->RewardTableWidget->columnCount(); column++)
        {
            QTableWidgetItem *item =  ui->RewardTableWidget->takeItem(row,column);
            if(item)
            {
                delete item;
                item = NULL;
            }
        }
    }
}

void PKResultWidget::timerEvent(QTimerEvent *event)
{
    //audience auto quit when audience not to click "quit" button wait for 10s
    if(quitAudienceTimerId == event->timerId())
    {
        PKResultWidgetDebug("timer: audidence quit");
        killTimer(quitAudienceTimerId);
        emit audienceQuit();
        PKResultWidgetDebug("audidence quit");
    }

    if (time >= AUTO_SELECT_REWARD_TIME)
    {
        killTimer(timeID);
        VictoryReward vic = info->getVictoryRewardList()[0];
        emit sendVictory(vic);

        {
            ui->RewardTableWidget->hide();
            ui->waitSelectedTextLabel->hide();

            ui->quitButton->setStyleSheet(QString("QPushButton:hover{border-image: url(://image/PKResultUI/bt_zuodeng_hover.png);}"
                                                  "QPushButton{border-image: url(://image/PKResultUI/bt_zuodeng.png);}"));

            ui->seclectedLabel->setStyleSheet(QString("border-image: url(://image/PKResultUI/img_reword0%1_big.png);").arg(info->getVictoryRewardList()[0].getID()));
            setRewardBg(true);
            ui->seclectedTipLabel->setText(QString("恭喜你获得了<font color = \"#f9de58\">%1</font>，快让主播<br/>为你表演吧！").arg(info->getVictoryRewardList()[0].getName()));
            ui->quitButton->show();
            ui->seclectedLabel->show();
            ui->seclectedTipLabel->show();
        }
        return ;
    }
    time += 1000;
    QString lbStr = QString("<html><head/><body><p><span style=\" font-size:14px; font-weight:600;"
                            "color:#fffffd;\">恭喜你获得胜利，想要我做<br/>什么呢？  "
                            "</span><span style=\"color:#f27ef6;\">%1s后将自动选择</span></p></body></html>")
                            .arg((AUTO_SELECT_REWARD_TIME - time)/1000);

    ui->waitSelectedTextLabel->setText(lbStr);
}

void PKResultWidget::AutoSelectReward()
{
    timeID = startTimer(1000);
}

void PKResultWidget::updateRankList(const QList<Player> &playerList)
{
    PKResultWidgetDebug("PKResultWidget updateRankList");
    // ranklist
    ui->RankListWidget->setRowCount(playerList.size());
    ui->RankListWidget->setColumnCount(4);
    for(int row = 0; row < playerList.size(); row++)
    {
        ui->RankListWidget->setRowHeight(row,30);
        for(int column = 0;column < 4;column++)
        {
            QLabel *label =new QLabel();
            int hostNameLen = 65;
            switch(column)
            {
            case 0:
                ui->RankListWidget->setColumnWidth(column,24);
                label->setStyleSheet(QString("border-image: url(:/image/PKResultUI/img_%1.png);").arg(row+1));
                ui->RankListWidget->setCellWidget(row, column, label);
                break;
            case 1:
                ui->RankListWidget->setColumnWidth(column,160);
                label->setText(QString("  " + playerList[row].getShortCutName(label->font(), hostNameLen)));
                ui->RankListWidget->setCellWidget(row, column, label);
                break;
            case 2:
                ui->RankListWidget->setColumnWidth(column,70);
                label->setText(QString("<font color=\"#f9d32b\">%1</font>分      ").arg((int)playerList[row].getSumScore().getTotalScore()));
                ui->RankListWidget->setCellWidget(row, column, label);
                break;
            case 3:
                ui->RankListWidget->setColumnWidth(column,21);
                if(row == 0)
                {
                    label->setStyleSheet(QString("border-image: url(:/image/PKResultUI/img_win.png);"));
                    ui->RankListWidget->setCellWidget(row, column, label);
                }else
                {
                    delete label;
                    label = NULL;
                }
                break;
            default:
                break;
            }
        }
    }
}


void PKResultWidget::updateRewardAndTip(const QList<Player> &playerList, int heat, Prop propIncome)
{
    PKResultWidgetDebug("PKResultWidget updateRewardAndTip");
    info = PkInfoManager::getInstance();
    bool bWinner = false;

    if(info->getMe().getID() == playerList[0].getID())
    {
        bWinner = true;
    }
    //host win
    if(playerList[0].getID() == info->getHostPlayer().getID())
    {
        ui->RewardTableWidget->hide();
        ui->waitSelectedTextLabel->hide();
        setRewardBg(true);
        ui->seclectedLabel->setStyleSheet(QString("border-image: url(://image/PKResultUI/img_HostRewardDefault.png);"));
        QString rightLabelStr,leftLabelStr;
        if(info->getMe().getIsHost())
        {
            rightLabelStr = QString("你获得了胜利！下次要让着点奥<br/>，说句话鼓励下大家吧~");
            leftLabelStr = QString("本场热度 <font color = \"#f9de58\">%1</font>").arg(heat);

            ui->quitButton->setStyleSheet(QString("QPushButton:hover{border-image: url(://image/PKResultUI/bt_guanbi_hover.png);}"
                                                  "QPushButton{border-image: url(://image/PKResultUI/bt_guanbi.png);}"));
        }
        else
        {
            int hostNameLen =60;
            rightLabelStr = QString("主播获得了胜利，下次要使劲捣乱<br/>奥，这样才能看跳舞嘛~");
            leftLabelStr = QString("<font color = \"#f9de58\">%1</font>获得了胜利！").arg(playerList[0].getShortCutName(ui->winnerTextLabel->font(),hostNameLen));
            ui->quitButton->setStyleSheet(QString("QPushButton:hover{border-image: url(://image/PKResultUI/bt_likai_hover.png);}"
                                                  "QPushButton{border-image: url(://image/PKResultUI/bt_likai.png);}"));

            //start  timer ,wait for 10s if audience not to close
            quitAudienceTimerId = startTimer(10*1000);
        }
        ui->seclectedTipLabel->setText(rightLabelStr);
        ui->seclectedTipLabel->show();
        ui->winnerTextLabel->setText(leftLabelStr);
        ui->winnerTextLabel->show();
        ui->quitButton->show();
        ui->seclectedLabel->show();
    }
    else  //pkPlayer win, not host win
    {
        ui->quitButton->hide();
        ui->seclectedLabel->hide();
        ui->seclectedTipLabel->hide();

        QString rightLabelStr,leftLabelStr;
        int hostRightNameLen = 74;
        if(info->getMe().getIsHost())
        {
            rightLabelStr = QString("等待 <font color = \"#f9de58\">%1</font> 挑选获胜<br/>奖励！").arg(playerList[0].getShortCutName(ui->waitSelectedTextLabel->font(), hostRightNameLen));
            leftLabelStr = QString("本场热度 <font color = \"#f9de58\">%1</font>").arg(heat);
        }
        else
        {
            int hostLeftNameLen = 60;
            leftLabelStr = QString("<font color = \"#f9de58\">%1</font> 获得了胜利！").arg(playerList[0].getShortCutName(ui->winnerTextLabel->font(), hostLeftNameLen));
            if(bWinner)
            {
                rightLabelStr =  QString("恭喜你获得胜利，想让主播<br/>做什么呢？");
            }
            else
            {
                rightLabelStr =  QString("坐等主播被<font color = \"#f9de58\"> %1 </font><br/>调戏！").arg(playerList[0].getShortCutName(ui->waitSelectedTextLabel->font(), hostRightNameLen));
            }
        }
        ui->waitSelectedTextLabel->setText(rightLabelStr);
        ui->waitSelectedTextLabel->show();
        ui->winnerTextLabel->setText(leftLabelStr);
        ui->winnerTextLabel->show();
        ui->RewardTableWidget->show();
        // rewardtablewidget

        QList<VictoryReward> vicList = info->getVictoryRewardList();
        for(int row = 0; row < 2; row++)
        {
            ui->RewardTableWidget->setRowHeight(row, 75);
            for(int column = 0; column < 3; column++)
            {
                ui->RewardTableWidget->setColumnWidth(column, 60);

                QString text;
                if(row*3+column+1 > vicList.size())
                {
                    text = "敬请期待";
                }else
                {
                    text= vicList[row*3+column].getName();
                }

                QWidget *widget = new QWidget();
                QHoverWidget *picLabel = new QHoverWidget(row,column,widget);
                picLabel->setGeometry(5,5,48,48);

                QLabel *label = new QLabel(widget);
                QFont font;
                font.setPixelSize(12);
                font.setWeight(75);
                font.setBold(false);
                font.setFamily("宋体");
                label->setFont(font);
                label->setAlignment(Qt::AlignHCenter);
                label->setText(text);
                label->setStyleSheet(QString("color:#c05dc4;"));
                label->setGeometry(0,60,60,15);
                ui->RewardTableWidget->setCellWidget(row , column , widget);
            }
        }
        // rewardtablewidget
    }
}

void PKResultWidget::setRewardBg(bool isLight)
{
    PKResultWidgetDebug("PKResultWidget setRewardBg");
    if(isLight)
    {
        ui->RewardBackGround->setStyleSheet("border-image: url(://image/PKResultUI/bg_window.png);");
        RewardBackGroundMask->startRotate();
        RewardBackGroundMask->show();
    }
    else
    {
        ui->RewardBackGround->setStyleSheet("border-image: url(://image/PKResultUI/bg_window.png);");
        RewardBackGroundMask->hide();
        RewardBackGroundMask->stopRotate();
    }
}

void PKResultWidget::handleRewardTableWidgetClicked(int row, int column)
{
    PKResultWidgetDebug("PKResultWidget handleRewardTableWidgetClicked");
    PKResultWidgetDebug("seclected vic");
    if(firstPricePlayer.getID() == info->getMe().getID() && !info->getMe().getIsHost() && time <= AUTO_SELECT_REWARD_TIME - 1000)
    {
        if(row*3+column+1 > info->getVictoryRewardList().size())
        {
            return;
        }

        PKResultWidgetDebug("winner seclected vic");
        ui->RewardTableWidget->hide();
        ui->waitSelectedTextLabel->hide();

        ui->quitButton->setStyleSheet(QString("QPushButton:hover{border-image: url(://image/PKResultUI/bt_zuodeng_hover.png);}"
                                              "QPushButton{border-image: url(://image/PKResultUI/bt_zuodeng.png);}"));

        ui->seclectedLabel->setStyleSheet(QString("border-image: url(://image/PKResultUI/img_reword0%1_big.png);").arg(info->getVictoryRewardList()[row*3+column].getID()));
        setRewardBg(true);
        ui->seclectedTipLabel->setText(QString("恭喜你获得了<font color = \"#f9de58\">%1</font>，快让主播<br/>为你表演吧！").arg(info->getVictoryRewardList()[row*3+column].getName()));
        ui->quitButton->show();
        ui->seclectedLabel->show();
        ui->seclectedTipLabel->show();

        VictoryReward vic = info->getVictoryRewardList()[row*3+column];
        this->killTimer(timeID);
        emit sendVictory(vic);
    }
}



