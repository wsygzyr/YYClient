#include "AudienceBidWidget.h"
#include "ui_AudienceBidWidget.h"

#include "UI/ToastWidget/ToastWidget.h"
#include "AudioHelper/AudioHelper.h"

#include <QMessageBox>
#include <QSound>
QString noBidTalkText = QString::fromLocal8Bit("你想好让我做什么了吗？人家有点怕怕的~");
QString firstBidLessThanHundredTalkText = QString::fromLocal8Bit("就出这么点，哼！你一点也不喜欢我~");
QString addBidBecomeCandidateTalkText = QString::fromLocal8Bit("你终于入围了，好期待我们的斗歌呀!");
QString lostCandidateTalkText = QString::fromLocal8Bit("奈何总有人横刀夺爱…");
QString firstBidBecomeCandidateTalkText = QString::fromLocal8Bit("一招制胜！世界是我们的了!");
QString addBidNoBecomeCandidateTalkText = QString::fromLocal8Bit("漂亮，可惜还差那么一点点~");
QString overMaxBidText = QString::fromLocal8Bit("您的出价太高了~");

QString noBidNoticeText(int candidate)
{
    return QString::fromLocal8Bit("<html><head/><body><p>偷偷告诉你，<span style=\" color:#f2c608;\">"
                                  "%1</span> 斗歌豆就能干掉他们!"
                                  "</p><p><br/></p></body></html>").arg(candidate);
}
QString bidNoBecomeCandidateNoticeText(int used, int candidate)
{
    return QString::fromLocal8Bit("<html><head/><body><p>已出价 <span style=\" color:#f2c608;\">"
                                  "%1</span> 斗歌豆，预计入围价 <span style=\" color:#f2c608;\">"
                                  "%2</span> 斗歌豆</p><p><br/></p></body></html>").arg(used).arg(candidate);
}
QString bidBecomeCandidateNoticeText(int used)
{
    return QString::fromLocal8Bit("<html><head/><body><p>已出价 <span style=\" color:#f2c608;\">"
                                  "%1</span> 斗歌豆，成功入围!</p><p><br/></p></body></html>").arg(used);
}

QString bidNumberText(int number)
{
    return QString::fromLocal8Bit("<html><head/><body><p><span style=\" font-weight:400;\">"
                                  "已有 </span><span style=\" font-weight:400; color:#f2c608;\">"
                                  "%1</span><span style=\" font-weight:400;\">"
                                  " 人出价，真英雄，不旁观！</span><br/></p></body></html>").arg(number);
}

AudienceBidWidget::AudienceBidWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudienceBidWidget)
{
    ui->setupUi(this);

    yync = YYNotificationCenter::getInstance();
    QObject::connect(yync, SIGNAL(onUsePropResponse(Prop, int, QString, QString)), this, SLOT(handleYYNotificationCenterOnUsePropResponse(Prop, int, QString, QString)));
	QObject::connect(yync , SIGNAL(onUserMoneyResponse(Player,Prop,int)) , this , SLOT(handleYYNotificationCenterOnUserMoneyResponse(Player,Prop,int)));
    stateCtrl = StateController::getInstance();

    NodeNC = NodeNotificationCenter::getInstance();
    QObject::connect(stateCtrl, SIGNAL(onStateBidInfo(int,Prop,Prop)), this, SLOT(handleStateControllerOnStateBidInfo(int,Prop,Prop)));
    QObject::connect(stateCtrl, SIGNAL(onStateBidStop(int)), this, SLOT(handleStateControllerOnStateBidStop(int)));
    QObject::connect(stateCtrl, SIGNAL(onStateBidConfirm(Prop)), this, SLOT(handleStateControllerOnStateBidConfirm(Prop)));

    pkInfo = PkInfoManager::getInstance();

    connect(ui->lineEdit , SIGNAL(textChanged(QString)) , this, SLOT(handleLineEditOnTextChanged(QString)));
    ui->lineEdit->installEventFilter(this);

    QIntValidator  *validator = new QIntValidator (0, 10000000, ui->lineEdit );
    ui->lineEdit->setValidator( validator );
    candidateUsed = 100;
    currentUsed = 30;
    totalUsed = 0;

    aniTimer = new QTimer(this);
    connect(aniTimer , SIGNAL(timeout()) , this , SLOT(handleAniTimerOnTimeOut()));
    aniTimer->setInterval(500);

    balanceTimer = new QTimer(this);
    balanceTimer->setInterval(10 * 1000);
    balanceTimer->setSingleShot(true);
    connect(balanceTimer , SIGNAL(timeout()) , this , SLOT(handleBalanceTimerOnTimeOut()));

    cfg = ConfigHelper::getInstance();

    ConfigHelper *cfg = ConfigHelper::getInstance();
    Prop prop = cfg->getPropForID(PROP_ID_PKBEAN);
    //   ui->douPicLb->setToolTip(prop.getPriceDesp() + "\n" + prop.getFuncDesp());
    ui->douPicLb->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">%1</span></p></body></html>").arg(prop.getPriceDesp() + "\n" + prop.getFuncDesp()));


    connect(ui->helpBtn , SIGNAL(clicked()) , this , SIGNAL(onHelpBtnClicked()));
	connect(ui->closeBtn , SIGNAL(clicked()) , this , SIGNAL(onCloseBtnClicked()));
    connect(ui->minBtn , SIGNAL(clicked()) , this , SIGNAL(onMinBtnClicked()));

    ui->miWidget->hide();

    resManager = ResManager::getInstance();

}

AudienceBidWidget::~AudienceBidWidget()
{
    delete ui;
}

void AudienceBidWidget::timerEvent(QTimerEvent *)
{
    countDownSecond--;
    if(countDownSecond <= 0)
    {
        this->endBid();
        this->killTimer(timerId);
    }
    else
    {
        ui->countDownNumLb->setText(QString("%1").arg(countDownSecond));
    }
}

void AudienceBidWidget::startBid()
{
    AudienceBidWidgetDebug("AudienceBidWidget startBid");
    leastUsed = pkInfo->getLeastBidProp().getUseNumber();
    candidateUsed = leastUsed;
    lastCandidateUsed = leastUsed;
    status = BID_START;

    QString songName = pkInfo->getCurrentSong().getName();
    int     playerNum = pkInfo->getCurrentSong().getSectionCount();
    AudienceBidWidgetDebug("playerNum is %d" , playerNum);
    QString titleStyleSheet = QString("border-image:url(://image/AudienceBidWidget/title_%1.png);").arg(playerNum - 1);
    ui->douTitleLb->setStyleSheet(titleStyleSheet);
    ui->douTitleLb->show();
    ui->songInfoLb->setText(QString::fromLocal8Bit("歌曲 《") +songName +
                            QString::fromLocal8Bit("》    斗歌人数：%1人").arg(playerNum));

    ui->candidateTextLb->setText(bidNumberText(0));
    ui->totalBidTextLb->setText(noBidNoticeText(candidateUsed));
    ui->talkTextLb->setText(noBidTalkText);

    ui->lineEdit->setText(QString::fromLocal8Bit("%1(底价)").arg(leastUsed));

    ui->lineEdit->setStyleSheet("border-image: url(:/image/AudienceBidWidget/bg_number.png);"
                                "color:white");
    ui->lineEdit->setTextMargins(11 , 0 , 0 ,0);
    ui->minusBtn->setStyleSheet("border-image: url(://image/AudienceBidWidget/bt_jian_disable.png);");
    ui->minusBtn->setDisabled(true);
    ui->bidBtn->show();
    ui->addBidBtn->hide();

    ui->countDownWidget->show();
    ui->countDownWidget->hide();

    ui->addBidBtn->setEnabled(true);
    ui->bidBtn->setEnabled(true);
    currentUsed = leastUsed;
    totalUsed = 0;

    ui->aniLabel->hide();

    ui->countDownTextLb->setStyleSheet("border-image: url(:/image/HostBidUI/daoji_text.png);");



}

void AudienceBidWidget::endBid()
{
    ui->addBidBtn->setDisabled(true);
    ui->bidBtn->setDisabled(true);

    QString titleStyleSheet = QString("border-image:url(://image/AudienceBidWidget/title_jiesuaning.png);");
    ui->douTitleLb->hide();
    ui->countDownWidget->show();
    ui->countDownTextLb->setStyleSheet(titleStyleSheet);
    ui->countDownTextLb->show();
    ui->countDownNumLb->hide();
}


void AudienceBidWidget::mousePressEvent(QMouseEvent* event)
{
    if (!ui->lineEdit->geometry().contains(event->pos()))
    {
        ui->lineEdit->clearFocus();
        if (status != BID_MAKE_UP && currentUsed == leastUsed)
        {
            ui->lineEdit->setText(QString::fromLocal8Bit("%1(底价)").arg(leastUsed));
        }
    }

    event->ignore();
}

bool AudienceBidWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lineEdit)
    {
        if(event->type() == QEvent::FocusIn)
        {
            if (status == BID_START)
            {
                ui->lineEdit->setText(QString("%1").arg(currentUsed));
            }

        }
    }
    return QWidget::eventFilter(watched,event);     // 最后将事件交给上层对话框
}

void AudienceBidWidget::changeNoticeText()
{
    AudienceBidWidgetDebug("AudienceBidWidget changeNoticeText");
    if(totalUsed == 0)
    {
        ui->totalBidTextLb->setText(noBidNoticeText(candidateUsed));

    }
    else if(totalUsed >= candidateUsed)
    {
        ui->totalBidTextLb->setText(bidBecomeCandidateNoticeText(totalUsed));
    }
    else if( totalUsed < candidateUsed)
    {
        ui->totalBidTextLb->setText(bidNoBecomeCandidateNoticeText(totalUsed , candidateUsed));
    }
}



void AudienceBidWidget::handleStateControllerOnStateBidInfo(int biderNumber, Prop leastProp, Prop expectIncome)
{
    AudienceBidWidgetDebug("AudienceBidWidget handleStateControllerOnStateBidInfo");
    this->biderNum = biderNumber;  
    ui->candidateTextLb->setText(bidNumberText(biderNumber));
    candidateUsed = leastProp.getUseNumber();
    changeNoticeText();
    if(totalUsed >= lastCandidateUsed && totalUsed < candidateUsed)
    {
        ui->talkTextLb->setText(lostCandidateTalkText);
    }
    lastCandidateUsed = candidateUsed;
    AudienceBidWidgetDebug("biderNumber is: %d, leastProp.getUseNumber() is: %d", biderNumber, leastProp.getUseNumber());
}

void AudienceBidWidget::handleStateControllerOnStateBidStop(int countdownSecond)
{
    AudienceBidWidgetDebug("AudienceBidWidget handleStateControllerOnStateBidStop");
    AudienceBidWidgetDebug("countdownSecond is: %ds", countdownSecond);
    this->countDownSecond = countdownSecond;

    timerId = this->startTimer(1000);

    ui->douTitleLb->hide();

    ui->countDownNumLb->setText(QString("%1").arg(countDownSecond));
    ui->countDownWidget->show();
    ui->countDownNumLb->show();

}

void AudienceBidWidget::handleStateControllerOnStateBidConfirm(Prop prop)
{
    AudienceBidWidgetDebug("handleStateControllerOnStateBidConfirm");

    yync->sendUsePropRequest(prop);
}

void AudienceBidWidget::handleYYNotificationCenterOnUsePropResponse(Prop prop,
                                                                    int retcode,
                                                                    QString orderNumber,
                                                                    QString signature)
{
    AudienceBidWidgetDebug("handleNodeNotificationCenterOnUsePropResponse");
    if (prop.getID() != PROP_ID_PKBEAN)
    {
        return;
    }

    //NodeNC->sendBidConfirm(prop, retcode, orderNumber, signature);
    stateCtrl->sendStateBidConfirm(prop, retcode, orderNumber, signature);
}

void AudienceBidWidget::handleYYNotificationCenterOnUserMoneyResponse(Player player, Prop prop, int result)
{
    balanceTimer->stop();
    if(player.getID() != pkInfo->getMe().getID())
    {
        AudienceBidWidgetDebug("Player's id(%d) is not equal Me's id(%d)." , player.getID() ,pkInfo->getMe().getID());
        return;
    }
    if(result != 0)
    {
        AudienceBidWidgetDebug("result is not zero , result is %d",result);
        resetBidBtn();
        ToastWidget *toast = new ToastWidget(this);
        toast->setText(QString::fromLocal8Bit("您的余额不足"));
        toast->setGeometry((this->width() - toast->width()) / 2 , (this->height() - toast->height() - 40) , toast->width() , toast->height());
        toast->show();
        return;
    }
    else
    {
        AudienceBidWidgetDebug("enough money~~");
        changeBidBtnState();
        NodeNC->sendBid(prop);
        resManager->addDownLoadResRequest(pkInfo->getCurrentSong().getAccompanyFileName(),
                                 pkInfo->getCurrentSong().getAccompanyUrl() ,
                                 ConfigHelper::getInstance()->getPluginPath() +  pkInfo->getCurrentSong().getAccompanyFilePath());

    }

}

void AudienceBidWidget::handleLineEditOnTextChanged(QString text)
{
    bool ok;
    int used = text.toInt(&ok);
    if(ok)
    {
        currentUsed = used;
        AudienceBidWidgetDebug("currentUsed is %d" , currentUsed);
    }
}

void AudienceBidWidget::handleAniTimerOnTimeOut()
{
    ui->aniLabel->hide();
    aniTimer->stop();

    // audioPlayer->stop();
}

void AudienceBidWidget::handleBalanceTimerOnTimeOut()
{
    resetBidBtn();
    ToastWidget *toast = new ToastWidget(this);
    toast->setText(QString::fromLocal8Bit("连接超时，请重试"));
    toast->setGeometry((this->width() - toast->width()) / 2 , (this->height() - toast->height() - 40) , toast->width() , toast->height());
    toast->show();
}

void AudienceBidWidget::on_plusBtn_clicked()
{
    //1 .Me can minus now
    ui->minusBtn->setStyleSheet("QPushButton:hover{border-image:url(:/image/AudienceBidWidget/bt_jian_hover.png);}"
                               "QPushButton{border-image: url(:/image/AudienceBidWidget/bt_jian.png);}");
    ui->minusBtn->setDisabled(false);
    //2 .change the lineEdit text
    currentUsed += 5;
    if(currentUsed > 999999999)
    {
        currentUsed = 999999999;
    }
    ui->lineEdit->setText(QString("%1").arg(currentUsed));
}

void AudienceBidWidget::on_minusBtn_clicked()
{
    currentUsed -= 5;
    if (status == BID_START)
    {
        if(currentUsed <= leastUsed)
        {

            //disable the minusBtn
            ui->minusBtn->setStyleSheet("border-image: url(://image/AudienceBidWidget/bt_jian_disable.png);");
            ui->lineEdit->setText(QString::fromLocal8Bit("%1(底价)").arg(leastUsed));
            currentUsed = leastUsed;
            AudienceBidWidgetDebug("currentUsed:%d" , currentUsed);
            return;
        }
    }
    if(currentUsed <= 0)
        currentUsed = 0;
    ui->lineEdit->setText(QString("%1").arg(currentUsed));
}



void AudienceBidWidget::resetBidBtn()
{
    if(status == BID_START)
    {
        ui->bidBtn->setEnabled(true);
    }
    else
    {
        ui->addBidBtn->setEnabled(true);
    }

}

void AudienceBidWidget::changeBidBtnState()
{
    Prop prop;
    prop.setID(PROP_ID_PKBEAN);
    prop.setUseNumber(currentUsed);
    pkInfo->setBidUsedProp(prop);
    //1. start the ani
    ui->aniLabel->setText(QString("%1").arg(currentUsed));
    ui->aniLabel->show();
    aniTimer->stop();
    aniTimer->start();
    QSound::play(cfg->getPluginPath() + QString("Sound\\PropSound\\bid.wav"));
    totalUsed += currentUsed;
    //2. change the bid text
    changeNoticeText();
    if(status == BID_START)
    {
        //1. Me can minus now
        ui->minusBtn->setStyleSheet("QPushButton:hover{border-image:url(:/image/AudienceBidWidget/bt_jian_hover.png);}"
                                    "QPushButton{border-image: url(:/image/AudienceBidWidget/bt_jian.png);}");
        ui->minusBtn->setDisabled(false);
        //2. plus the total
        //3 .change the backgroud and cursonPosition of lineEdit
        ui->lineEdit->setStyleSheet("border-image:url(://image/AudienceBidWidget/bg_number_jia.png);"
                                    "color:white");

        ui->lineEdit->setTextMargins(36 , 0 , 0 ,0);

        //4 .show addBidBtn
        ui->bidBtn->hide();
        ui->addBidBtn->show();

        //5. change the talk text
        if(totalUsed < 100 && totalUsed < candidateUsed)
        {
            ui->talkTextLb->setText(firstBidLessThanHundredTalkText);
        }
        if(totalUsed >= candidateUsed)
        {
            ui->talkTextLb->setText(firstBidBecomeCandidateTalkText);
        }
        //6 .change the status to BID_MAKE_UP
        status = BID_MAKE_UP;
        //7 .change the currentUsed , Me can plus any number now
        ui->lineEdit->setText("10");
        currentUsed = 10;
    }
    else
    {
        ui->lineEdit->setText(QString("%1").arg(currentUsed));
        if(totalUsed >= candidateUsed)
        {
            ui->talkTextLb->setText(addBidBecomeCandidateTalkText);
        }
        else
        {
            ui->talkTextLb->setText(addBidNoBecomeCandidateTalkText);
        }
    }
}

bool AudienceBidWidget::checkTotalUsed()
{
    int tempTotal = totalUsed;
    tempTotal += currentUsed;
    if(tempTotal > 1000000)
    {
        ToastWidget *toast = new ToastWidget(this);
        toast->setText(overMaxBidText);
        toast->setGeometry((this->width() - toast->width()) / 2 , (this->height() - toast->height() - 40) , toast->width() , toast->height());
        toast->show();
        return false;
    }
    return true;
}

void AudienceBidWidget::sendCheckUserBalance()
{
    Prop prop;
    prop.setID(PROP_ID_PKBEAN);
    prop.setUseNumber(totalUsed + currentUsed);
    yync->sendUserMoneyReq(pkInfo->getMe() , prop);
    balanceTimer->start();
}


void AudienceBidWidget::on_bidBtn_clicked()
{

    if (AudioHelper::AudioInputIsOK())
    {
        //1 .whether the currentUsed more than leaseUsed
        if(currentUsed < leastUsed)
        {
            ToastWidget *toast = new ToastWidget(this);
            toast->setText("出价低于底价");
            toast->setGeometry((this->width() - toast->width()) / 2 , (this->height() - toast->height() - 40) , toast->width() , toast->height());
            toast->show();
            return;
        }
        //2. check the total
        if(!checkTotalUsed())
        {
            return;
        }
        if(currentUsed != 0)
        {
            sendCheckUserBalance();
        }
        else
        {
            Prop prop;
            prop.setID(PROP_ID_PKBEAN);
            prop.setUseNumber(0);
            changeBidBtnState();
            NodeNC->sendBid(prop);
            resManager->addDownLoadResRequest(pkInfo->getCurrentSong().getAccompanyFileName(),
                                     pkInfo->getCurrentSong().getAccompanyUrl() ,
                                     ConfigHelper::getInstance()->getPluginPath() +  pkInfo->getCurrentSong().getAccompanyFilePath());
        }
    }
    else
    {
        ui->miWidget->show();
    }
}

void AudienceBidWidget::on_addBidBtn_clicked()
{
    if(currentUsed == 0)
        return;

    if(!checkTotalUsed())
    {
        return;
    }

    sendCheckUserBalance();
}



void AudienceBidWidget::on_okButton_clicked()
{
    if (AudioHelper::AudioInputIsOK())
    {
        ui->miWidget->hide();
    }
    else
    {
        ToastWidget *toast = new ToastWidget(this);
        toast->setText("请插入耳麦");
        toast->setGeometry((this->width() - toast->width()) / 2 , (this->height() - toast->height() - 40) , toast->width() , toast->height());
        toast->show();
    }
}

void AudienceBidWidget::on_cancelButton_clicked()
{
    ui->miWidget->hide();
}



