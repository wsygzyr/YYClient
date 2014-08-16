#include "FadeHelper.h"
#include <QTimerEvent>
#include <QEventLoop>
#include <QDebug>
FadeHelper::FadeHelper(QObject *parent) :
    QObject(parent),
    deltaTime(-1),
    deltaOpacity(-1),
    fadeWidget(0)
{
    opacityEffect = new QGraphicsOpacityEffect(this);
    m_bDealFadeProcess = false;
    timer = NULL;
}

FadeHelper::~FadeHelper()
{
    if(timer != NULL)
    {
        timer->deleteLater();
        timer = NULL;
    }
}

void FadeHelper::setFadeWidget(QWidget *widget)
{
    fadeWidget = widget;
    fadeWidget->setGraphicsEffect(opacityEffect);
}

void FadeHelper::setFadeType(FadeHelper::FadeType type)
{
    this->type = type;

}

void FadeHelper::setFadeTime(int msec)
{
    fadeTime = msec;
    deltaTime = msec / 10;
}

void FadeHelper::startFade()
{
    if(type == FADE_IN)
    {
        opacity = 0;
        deltaOpacity = 0.1;
    }
    else
    {
        opacity = 1.0;
        deltaOpacity = -0.1;
    }
    if(fadeWidget && deltaTime > 0)
    {
        opacityEffect->setOpacity(opacity);
        if (fadeWidget->isHidden())
        {
            fadeWidget->show();
        }
        this->startTimer(deltaTime);
    }
}

void FadeHelper::timerEvent(QTimerEvent *e)
{
    opacity += deltaOpacity;
    opacityEffect->setOpacity(opacity);
    if((deltaOpacity > 0 && opacity >= 1.0) || (deltaOpacity < 0 && opacity <= 0))
    {
        this->killTimer(e->timerId());
        if(!m_bDealFadeProcess)
            fadeWidget->hide();
        emit fadeEnd();
    }
}

void FadeHelper::DealFadeProcess(QWidget *widget, int fadeInTime, int fadeOutTime, int showTime)
{

    if(fadeInTime > 0)
    {
        m_bDealFadeProcess = true;
        setFadeWidget(widget);
        setFadeType(FadeHelper::FADE_IN);
        setFadeTime(fadeInTime);
        startFade();
    }


    if(showTime > 0)
    {
        m_nFadeOutTime = fadeOutTime;
        timer = new QTimer;
        timer->setSingleShot(true);

        connect(timer , SIGNAL(timeout()) , this , SLOT(DealShowProcess()));
        timer->start(showTime);
    }

}

void FadeHelper::DealShowProcess()
{
    m_bDealFadeProcess = false;
    setFadeType(FadeHelper::FADE_OUT);
    setFadeTime(m_nFadeOutTime);
    startFade();
    if(timer != NULL)
    {
        timer->deleteLater();
        timer = NULL;
    }
}

void FadeHelper::StopShow()
{
    m_bDealFadeProcess = false;
    if(fadeWidget)
    {
        fadeWidget->hide();
    }

    if(timer != NULL)
    {
        timer->deleteLater();
        timer = NULL;
    }
}

