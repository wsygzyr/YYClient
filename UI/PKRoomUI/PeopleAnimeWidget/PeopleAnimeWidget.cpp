#include "PeopleAnimeWidget.h"
#include "ui_PeopleAnimeWidget.h"
#include <QFontMetrics>

PeopleAnimeWidget::PeopleAnimeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PeopleAnimeWidget)
{
    ui->setupUi(this);


    isPlayingText = false;
    currentPlayingPriority = -1;
    nextPlayingPriority = -1;

    textCountDownTimer = new QTimer(this);
    textCountDownTimer->setInterval(1000);
    connect(textCountDownTimer , SIGNAL(timeout()) ,  this ,SLOT(handleTextCountDownTimerOnTimeOut()));



    font.setBold(false);
    font.setFamily("微软雅黑");
    font.setPixelSize(14);
    QFontMetrics metrics(font);
    ui->AnimeTipLabel->setFixedWidth(metrics.width(ui->AnimeTipLabel->text()) + 20);

    ui->AnimeTipLabel->hide();

    fadeOutHelper = new FadeHelper(this);
    fadeOutHelper->setFadeWidget(ui->countDownLabel);
    fadeOutHelper->setFadeTime(500);
    fadeOutHelper->setFadeType(FadeHelper::FADE_OUT);
    connect(fadeOutHelper , SIGNAL(fadeEnd()) , this , SLOT(handleFadeHelperOnFadeEnd()));

    timer = NULL;

    cheerAnimator = new Animator;
    connect(cheerAnimator , SIGNAL(playEnd()) , this , SLOT(handleCheerAnimatorOnPlayEnd()));

    ani = AnimationManager::getInstance();
    connect(ani , SIGNAL(onPlayEnd(Prop)) , this , SLOT(handlePropAnimatorOnPlayEnd(Prop)));

    bPlayAni = false;
}

PeopleAnimeWidget::~PeopleAnimeWidget()
{
    cheerAnimator->deleteLater();
    if (timer != NULL)
    {
        timer->deleteLater();
        timer = NULL;
    }
    delete ui;
}

void PeopleAnimeWidget::animateProp(Prop prop)
{
    PeopleAnimeWidgetDebug("PeopleAnimeWidget animateProp");
    ui->GirlPicLabel->hide();
    ani->addAnimation(this , ui->GirlPicLabel->geometry() , prop);

}

void PeopleAnimeWidget::stopAnimateProp()
{
    PeopleAnimeWidgetDebug("PeopleAnimeWidget stopAnimateProp");
    ui->AnimeTipLabel->hide();
    ui->GirlPicLabel->show();
}

void PeopleAnimeWidget::updateText(int priority,QString str)
{
    PeopleAnimeWidgetDebug("PeopleAnimeWidget updateText");
    PeopleAnimeWidgetDebug("currentPlayingPriority is :%d , str is %s" , priority , str.toUtf8().data());
    if(!isPlayingText || (priority >= currentPlayingPriority))
    {
        PeopleAnimeWidgetDebug("priority is :%d , str is %s" , priority , str.toUtf8().data());
        currentPlayingPriority = priority;

        if(priority == 5)
        {
            ui->AnimeTipLabel->hide();

            textCountDownTimer->stop();
            ui->countDownLabel->setStyleSheet(QString("border-image: url(:/image/PKRoomUI/countDown_%1.png);").arg(str));
            ui->countDownLabel->show();
//            fadeOutHelper->startFade();

            //add code by zhaoli , solve ready 5 4 3 2 1 go
            if (timer != NULL)
            {
                timer->deleteLater();
                timer = NULL;
            }
            timer = new QTimer;
            timer->setSingleShot(true);
            connect(timer , SIGNAL(timeout()) , fadeOutHelper , SLOT(startFade()));
            timer->start(350);

            cheerAnimator->stop();
        }
        else if(priority == 1)
        {
            ConfigHelper *cfg = ConfigHelper::getInstance();
            QString path = cfg->getPluginPath() + "Animation";
            QPoint mapPoint = mapFrom(this->parentWidget()->parentWidget() , QPoint(340,52));
            cheerAnimator->Animate(path , str ,this , QRect(mapPoint.x() , mapPoint.y() , 123 , 48) , 12 , false);

        }
        else
        {
            QFontMetrics metrics(font);
            ui->AnimeTipLabel->setFixedWidth(metrics.width(str) + 20);
            ui->AnimeTipLabel->setText(str);
            ui->AnimeTipLabel->show();
            textCountDownTimer->stop();
            textCountDownTimer->start(2000);

            cheerAnimator->stop();
        }

        isPlayingText = true;
    }
}

void PeopleAnimeWidget::handlePropAnimatorOnPlayEnd(Prop prop)
{
    PeopleAnimeWidgetDebug("PeopleAnimeWidget handlePropAnimatorOnPlayEnd");
    if(prop.getID() == PROP_ID_TRANSFORMER || prop.getID() == PROP_ID_TOMCAT)
    {
        PeopleAnimeWidgetDebug("onPlayEnd ,propName is %s",prop.getName().toUtf8().data());
        ui->GirlPicLabel->show();
    }
}

void PeopleAnimeWidget::handleTextCountDownTimerOnTimeOut()
{
    PeopleAnimeWidgetDebug("PeopleAnimeWidget handleTextCountDownTimerOnTimeOut");
    PeopleAnimeWidgetDebug("time out, prioity is %d" ,currentPlayingPriority);
    ui->AnimeTipLabel->setText("");
    ui->AnimeTipLabel->hide();
    textCountDownTimer->stop();
    isPlayingText = false;
}

void PeopleAnimeWidget::handleFadeHelperOnFadeEnd()
{
    PeopleAnimeWidgetDebug("PeopleAnimeWidget handleFadeHelperOnFadeEnd");
    ui->AnimeTipLabel->setText("");
    ui->AnimeTipLabel->hide();
    isPlayingText = false;
}

void PeopleAnimeWidget::handleCheerAnimatorOnPlayEnd()
{
    PeopleAnimeWidgetDebug("PeopleAnimeWidget handleCheerAnimatorOnPlayEnd");
    ui->AnimeTipLabel->setText("");
    ui->AnimeTipLabel->hide();
    isPlayingText = false;
}

