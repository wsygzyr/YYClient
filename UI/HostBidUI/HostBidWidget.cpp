#include "HostBidWidget.h"
#include "ui_HostBidWidget.h"

HostBidWidget::HostBidWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostBidWidget)
{
    ui->setupUi(this);
    pkInfo = PkInfoManager::getInstance();
    nnc = NodeNotificationCenter::getInstance();
    timer = new QTimer(this);
    expectIncome = 0;
//    connect(this,SIGNAL(onHostStopBid()), nnc, SIGNAL(onUploadLog()));
}

HostBidWidget::~HostBidWidget()
{
    delete ui;
}

void HostBidWidget::updateBidInfo(int biderNumber, Prop leastProp , Prop expectIncome)
{
    HostBidWidgetDebug("HostBidWidget updateBidInfo");
    int count = leastProp.getUseNumber();
    this->expectIncome = expectIncome.getUseNumber();
    ui->candidateLb->setText(QString::fromLocal8Bit("<html><head/><body><p><span style=\" font-weight:400;\">已有 "
                                                    "</span><span style=\" font-family:微软雅黑; font-size:18px; color:#F6EE82;\">%1</span>"
                                                "<span style=\" font-weight:400;\"> 人出价，预计入围价 </span>"
                                                "<span style=\" font-weight:400; color:#f2c608;\">%2</span>"
                                                "<span style=\" font-weight:400;\"> 斗歌豆</span></p><p><br/></p></body></html>").arg(biderNumber).arg(count));

//    ui->incomeLb->setText(QString::fromLocal8Bit("<html><head/><body><p>竞拍已进行 <span style=\" color:#f2c608;\">")
//            + curTimeStr +
//            QString::fromLocal8Bit("</span> , 预计带来</p><p  style=\"line-height:10px;\">收益 <span style=\" color:#f2c608;\">%1</span> 斗歌豆</p><p></p></body></html>").arg(this->expectIncome));

}

void HostBidWidget::startBid(int bidCount)
{
    HostBidWidgetDebug("HostBidWidget startBid");
    timer->stop();
    timer->setInterval(1000);
    timer->start();
    connect(timer , SIGNAL(timeout()) , this , SLOT(handleTimerOnTimeOut()));
    expectIncome = 0;
    configUI();
    Prop leastProp ,expectIncome;
    leastProp.setUseNumber(bidCount);
    expectIncome.setUseNumber(0);
    updateBidInfo(0 , leastProp , expectIncome);
}

void HostBidWidget::configUI()
{
    HostBidWidgetDebug("HostBidWidget configUI");
    QString songName = pkInfo->getCurrentSong().getName();
    int     playerNum = pkInfo->getCurrentSong().getSectionCount();

    HostBidWidgetDebug("song section id:%d",playerNum);

    QString titleStyleSheet = QString("border-image:url(://image/HostBidUI/title_%1.png);").arg(playerNum - 1);
    ui->douTitleLb->setStyleSheet(titleStyleSheet);

    ui->songInfoLb->setText(QString::fromLocal8Bit("歌曲 《") +songName +
                            QString::fromLocal8Bit("》    斗歌人数：%1人").arg(playerNum));

    ui->talkTextLb->setText(QString::fromLocal8Bit("香喷喷的美妞甩卖啦，只要9块9!"));

    ui->countDownWidget->hide();
    ui->countDownBtn->hide();

}

void HostBidWidget::timerEvent(QTimerEvent *)
{
    static int initTime = 5;
    ui->countDownNumLb->setText(QString("%1").arg(initTime - 1));
    ui->countDownBtn->setText(QString("           %1").arg(initTime - 1));

    initTime--;
    if(initTime == 0)
    {
        this->killTimer(timerId);
        QString titleStyleSheet = QString("border-image:url(://image/HostBidUI/title_jiesuaning.png);");
        ui->countDownTextLb->setStyleSheet(titleStyleSheet);
        ui->countDownNumLb->hide();
        initTime = 5;
    }
}


void HostBidWidget::on_StartTimeBtn_clicked()
{
    ui->StartTimeBtn->hide();
    ui->douTitleLb->hide();
    ui->countDownWidget->show();
    ui->countDownBtn->show();
    timerId = this->startTimer(1000);
    emit onHostStopBid();

}

void HostBidWidget::handleTimerOnTimeOut()
{
    static int totalSec = 0;
    int lastMinute = totalSec / 60;
    int lastSec    = totalSec % 60;
    if(lastMinute == 59 && lastSec == 59)
    {
        totalSec = 0;
        timer->stop();
        return;
    }
    totalSec++;
    if(totalSec >= 30)
    {
        ui->talkTextLb->setText(QString::fromLocal8Bit("卖萌有奇效，一般人我不告诉ta~"));
    }
    if(totalSec >= 180)
    {
        ui->talkTextLb->setText(QString::fromLocal8Bit("时间有点长了奥，大家会不会等不急了？"));
    }
    int curMinute = totalSec / 60;
    int curSec    = totalSec % 60;


    QString curMinStr = (curMinute >= 10) ? QString("%1").arg(curMinute) : QString("0%1").arg(curMinute);
    QString curSecStr = (curSec >= 10 ) ? QString("%1").arg(curSec) : QString("0%1").arg(curSec);
    QString curTimeStr = curMinStr + ":" + curSecStr;
   // HostBidWidgetDebug("%s" , QString("curtimeStr:" + curTimeStr).toUtf8().data());
    ui->incomeLb->setText(QString::fromLocal8Bit("<html><head/><body><p>竞拍已进行 <span style=\" color:#f2c608;\">")
            + curTimeStr +
            QString::fromLocal8Bit("</span> , 预计带来</p><p  style=\"line-height:10px;\">收益 <span style=\" color:#f2c608;\">%1</span> 斗歌豆</p><p></p></body></html>").arg(expectIncome));


}
