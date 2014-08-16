#include "PropAnimation.h"

PropAnimation::PropAnimation(QObject *parent) :
    QObject(parent),
    aniCount(0),
    aniPath("")
{
    aniTimer = new MMTimer;
    aniTimer->setInterval(67);

    cfg = ConfigHelper::getInstance();

    aniLabel = NULL;

    connect(aniTimer , SIGNAL(timeout()) , this , SLOT(handlTimerOnTimeOut()));
    audioPlayer = NULL;
}

PropAnimation::~PropAnimation()
{
    delete aniTimer;
    if(aniLabel)
    {
        aniLabel->deleteLater();
        aniLabel = NULL;
    }
    if(audioPlayer)
    {
        audioPlayer->stop();
    }
}

void PropAnimation::setProp(Prop prop)
{
    PropAnimationDebug("PropAnimation setProp");
    for(int i = 0 ; i < cfg->getPropList().size() ; i++)
    {
        if(prop.getID() == cfg->getPropList().at(i).getID())
        {
            this->prop = cfg->getPropList().at(i);
        }
    }
}

void PropAnimation::startPlay()
{
    PropAnimationDebug("PropAnimation startPlay");
    PropAnimationDebug("start play");
    mutex.lock();
    if(audioPlayer == NULL)
    {
        audioPlayer = new QSound(cfg->getPluginPath() + QString("Sound\\PropSound\\%1.wav").arg(this->prop.getID()),this);
        audioPlayer->play();
    }
    mutex.unlock();
    aniTimer->start();
    if(aniLabel)
    {
        aniLabel->clear();
        aniLabel->show();
    }
}

void PropAnimation::stopPlay()
{
    PropAnimationDebug("PropAnimation stopPlay");
    PropAnimationDebug("prop id :%s , stop play" , prop.getName().toUtf8().data());

    aniTimer->stop();
    if (aniLabel)
    {
        PropAnimationDebug("aniLabel is not NULL!");
        aniLabel->hide();
        aniLabel->clear();
        aniLabel->repaint();
    }
    else
    {
        PropAnimationDebug("aniLabel is NULL!");
    }
    mutex.lock();
    if(audioPlayer)
    {
        audioPlayer->stop();
    }
    mutex.unlock();
}

void PropAnimation::setParams(QWidget *parent, QRect geometry)
{
    PropAnimationDebug("PropAnimation setParams");
    aniLabel = new QLabel(parent);
    aniLabel->setGeometry(geometry);
    aniLabel->show();
}

void PropAnimation::handleAudioPlayerOnFinished()
{
    PropAnimationDebug("PropAnimation handleAudioPlayerOnFinished");
    mutex.lock();
    if(audioPlayer)
    {
        audioPlayer->stop();
        audioPlayer = NULL;
    }
    mutex.unlock();
}
