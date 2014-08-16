#include "BidResultWidget.h"
#include "ui_BidResultWidget.h"

#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/StateController/StateController.h"

BidResultWidget::BidResultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BidResultWidget)
{
    ui->setupUi(this);

    pkInfo = PkInfoManager::getInstance();
    stateCon = StateController::getInstance();

    tipsTimer = new QTimer(this);
    tipsTimer->setInterval(5000);
    QObject::connect(tipsTimer, SIGNAL(timeout()), this, SLOT(handleTipsTimerOnTimeOut()));

    tipsList.append(QString("正在结账中，请注意扣费弹窗..."));
    tipsList.append(QString("演唱中若声音较小，可以试试麦克风增强奥～"));
    tipsList.append(QString("使用道具可以影响当前演唱者的得分"));
    tipsList.append(QString("唱歌当然要准备好耳麦啦～"));


    QObject::connect(stateCon, SIGNAL(onStateBidSettlement(int)), this, SLOT(handleThisOnStateBidSettlement(int)));

    // 中途进入加载页面
    time = 0;
    singerNumTime = 0;
    downloadResTime = 0;
    pkSingerNumRec = 0;
    progressBarLabelMoveCount = 0;
    tipsCount = 0;
    downloadResOk = false;

    // progress bar setting
    ui->progressBar->setValue(0);
    ui->progressBar->setRange(0,100);
    ui->processBarLabel->setGeometry(6, 191, 38, 38);

    pkSingerNum = pkInfo->getCurrentSong().getSectionCount() - 1;
    tipsCount = 0;
}

BidResultWidget::~BidResultWidget()
{
    delete ui;
}

void BidResultWidget::setBidResultWidgetInfo()
{
    time = 0;
    singerNumTime = 0;
    downloadResTime = 0;
    pkSingerNumRec = 0;
    progressBarLabelMoveCount = 0;
    tipsCount = 0;
    downloadResOk = false;

    // progress bar setting
    ui->progressBar->setValue(0);
    ui->progressBar->setRange(0,100);
    ui->processBarLabel->setGeometry(6, 191, 38, 38);

    pkSingerNum = pkInfo->getCurrentSong().getSectionCount() - 1;
    BidResultWidgetDebug("SectionCount is %d", pkInfo->getCurrentSong().getSectionCount());
    BidResultWidgetDebug("pkSingerNum is %d", pkSingerNum);

    timeID = startTimer(100);

    tipsCount = 0;
    tipsTimer->start();
}

void BidResultWidget::setProgressBarValue(int value)
{
    ui->processBarLabel->setGeometry(ui->processBarLabel->x() + (value - time) * ui->progressBar->width() / 100,
                                     ui->processBarLabel->y(), ui->processBarLabel->width(), ui->processBarLabel->height());
    ui->progressBar->setValue(value);
}

void BidResultWidget::killTimeID()
{
    killTimer(timeID);
    tipsTimer->stop();
}

void BidResultWidget::handleThisOnStateBidSettlement(int value)
{
    BidResultWidgetDebug("handleThisOnStateBidSettlement value %d", value);
    pkSingerNumRec = value;
    if (0 == pkSingerNumRec)
    {
        singerNumTime = 0;
    }
    else
    {
        singerNumTime = pkSingerNumRec * SINGER_NUM_REC_TIME / (pkInfo->getCurrentSong().getSectionCount() - 1);
    }
}

void BidResultWidget::handleTipsTimerOnTimeOut()
{
    ui->tipsLabel->setText(tipsList[tipsCount++ % tipsList.size()]);
}

void BidResultWidget::timerEvent(QTimerEvent *event)
{
    BidResultWidgetDebug("BidResultWidgetDebug 100ms");

    if (0 == pkInfo->getCurPkPlayerDownloadState(0) && !downloadResOk)
    {
        BidResultWidgetDebug("host download ok, pkInfo->getCurPkPlayerDownloadState(0) = %d",
                             pkInfo->getCurPkPlayerDownloadState(0));
        downloadResOk = true;
        downloadResTime = DOWNLOAD_RES_TIME;
    }

    if ((time < 99) && (time < downloadResTime + singerNumTime))
    {
        ++time;
        ui->progressBar->setValue(time);
        if (progressBarLabelMoveCount++ % 2)
        {
            ui->processBarLabel->setGeometry(ui->processBarLabel->x() + ui->progressBar->width() / 100,
                                         ui->processBarLabel->y(), ui->processBarLabel->width(), ui->processBarLabel->height());
        }
        else
        {
            ui->processBarLabel->setGeometry(ui->processBarLabel->x() + 1 + ui->progressBar->width() / 100,
                                         ui->processBarLabel->y(), ui->processBarLabel->width(), ui->processBarLabel->height());
        }
    }

    BidResultWidgetDebug("time is %d, downloadResTime is %d, singerNumTime is %d", time, downloadResTime, singerNumTime);
}

