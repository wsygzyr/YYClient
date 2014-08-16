#include "NStepPropAnimation.h"

NStepPropAnimation::NStepPropAnimation(QObject *parent):
    PropAnimation(parent),
    maxStepCount(1),
    currentStep(1),
    currentLoopCount(1)
{
    for(int i = 0 ; i < sizeof(loopCountArray) / sizeof(int) ; i++)
    {
        loopCountArray[i] = 1;
    }
}

NStepPropAnimation::~NStepPropAnimation()
{

}

void NStepPropAnimation::startPlay()
{
    aniPath = cfg->getPluginPath() + "Animation\\PropAnimation\\" +
            QString("%1Prop\\%2_%3").arg(prop.getID()).arg(prop.getID()).arg(currentStep);
    PropAnimation::startPlay();
}

void NStepPropAnimation::stopPlay()
{
    currentLoopCount = 1;
    currentStep = 1;
    aniCount = 0;

    PropAnimation::stopPlay();
}

void NStepPropAnimation::handlTimerOnTimeOut()
{
    if(!aniLabel)
    {
        stopPlay();
        emit onPlayEnd(prop);
        return;
    }
    aniCount++;
    QString path = aniPath + QString(" (%1).png").arg(aniCount);
    QDir dir ;
    if (!dir.exists(path))
    {
        if(loopCountArray[currentStep] == currentLoopCount)
        {
            currentStep++;
            currentLoopCount = 1;
            if(currentStep > maxStepCount)
            {
                stopPlay();
                emit onPlayEnd(prop);
                return;
            }
        }
        else
        {
            currentLoopCount++;
        }
        aniCount = 1;
        aniPath = cfg->getPluginPath() + "Animation\\PropAnimation\\" +
                QString("%1Prop\\%2_%3").arg(prop.getID()).arg(prop.getID()).arg(currentStep);
        path = aniPath + QString(" (%1).png").arg(aniCount);
    }

    QPixmap pix(path);
    aniLabel->setPixmap(pix);
    aniLabel->setScaledContents(true);
    //aniLabel->show();

}


void NStepPropAnimation::setAnimeStepCount(int stepCount)
{
    this->maxStepCount = stepCount;
}

void NStepPropAnimation::setStepLoopCount(int step, int loopCount)
{
    this->loopCountArray[step] = loopCount;
}
