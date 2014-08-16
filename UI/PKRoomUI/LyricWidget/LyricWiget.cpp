#include "LyricWidget.h"
#include "ui_LyricWidget.h"

LyricWidget::LyricWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LyricWidget)
{
    LyricWidgetDebug("lyricwidget start!");
    ui->setupUi(this);

    upLyricLabel = new LyricLabel(true,ui->upWidget);
    downLyricLabel = new LyricLabel(false,ui->downWidget);
    ui->upVerticalLayout->addWidget(upLyricLabel);
    ui->downVerticalLayout->addWidget(downLyricLabel);
    upLyricLabel->show();
    downLyricLabel->show();
    lyricParse = LyricParse::getInstance();

    timer = NULL;
}

LyricWidget::~LyricWidget()
{
    delete ui;
}

void LyricWidget::start(int sentenceIndex)
{
    LyricWidgetDebug("lyricwidget start!");

    upLyricLabel->setSentence(sentenceIndex);
    downLyricLabel->setSentence(sentenceIndex + 1);

    this->show();

    if (!timer)
    {
        timer = new MMTimer();
    }
    timer->setInterval(UPDATE_INTERVAL);
    timer->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));

    playerTime = 0;
    lyricTime = lyricParse->getSentenceBeginTime(sentenceIndex);
    deltaTime = 0;

    LyricWidgetDebug("LyricWidget start success!");
}

void LyricWidget::stop()
{
    LyricWidgetDebug("lyricwidget hide!");
    this->hide();
    if (timer)
    {
        timer->stop();
        timer->deleteLater();
        timer = NULL;
    }
    LyricWidgetDebug("lyricwidget success!");
}

void LyricWidget::updatePlayerTime(float time)
{
    playerTime = time;

 //   LyricWidgetDebug("playerTime is: %f, lyricTime is: %f", playerTime, lyricTime);

    if (lyricTime >= playerTime)
    {
        return;
    }

    deltaTime  = (float)(playerTime - lyricTime)/(float)UPDATE_COUNT;

//    LyricWidgetDebug("deltaTime is: %f", deltaTime);
}

void LyricWidget::reset()
{
    LyricWidgetDebug("lyricwidget reset!");
    start(0);
}


void LyricWidget::handleTimeout()
{
    if (lyricTime < playerTime)
    {
        lyricTime += deltaTime;
    }

//    LyricWidgetDebug("lyricTime is: %f, deltaTime is: %f", lyricTime, deltaTime);

    if (lyricTime >= playerTime)
    {
        return ;
    }

//    LyricWidgetDebug("lyricTime is %f , playerTime is %f",lyricTime , playerTime);
    int upLabelSentenceIndex = upLyricLabel->getSentenceIndex();
    int downLabelSentenceIndex = downLyricLabel->getSentenceIndex();


    if (lyricTime < lyricParse->getSentenceBeginTime(upLabelSentenceIndex)
            && lyricTime < lyricParse->getSentenceBeginTime(downLabelSentenceIndex))
    {
        return;
    }

    while(1)
    {
        // update upLyricLabelBegin
        bool bUpBreak = false;
        bool bDownBreak = false;
        upLabelSentenceIndex = upLyricLabel->getSentenceIndex();
        if (lyricTime >= lyricParse->getSentenceBeginTime(upLabelSentenceIndex))
        {
            upLyricLabel->updateTime(lyricTime);

            if (lyricTime <= lyricParse->getSentenceEndTime(upLabelSentenceIndex))
            {
                bUpBreak = true;
            }
            else
            {
                emit OneSentenceEnd(upLabelSentenceIndex);
                upLyricLabel->setSentence(upLabelSentenceIndex + 2);
            }
        }


        // update downLyricLabel
        downLabelSentenceIndex = downLyricLabel->getSentenceIndex();
        if (lyricTime >= lyricParse->getSentenceBeginTime(downLabelSentenceIndex))
        {
            downLyricLabel->updateTime(lyricTime);
            if (lyricTime <= lyricParse->getSentenceEndTime(downLabelSentenceIndex))
            {
                bDownBreak = true;
            }
            else
            {
                emit OneSentenceEnd(downLabelSentenceIndex);
                downLyricLabel->setSentence(downLabelSentenceIndex + 2);
            }
        }

        if(bUpBreak || bDownBreak)
        {
            break;
        }
        // break if lyricTime is not in any sentence
        if (0 > lyricParse->getSentenceIndexWithTime(lyricTime))
        {
            break;
        }

       // LyricWidgetDebug("while(1)");

    }
}
