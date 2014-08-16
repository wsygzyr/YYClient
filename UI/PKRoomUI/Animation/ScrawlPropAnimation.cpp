#include "ScrawlPropAnimation.h"

ScrawlPropAnimation::ScrawlPropAnimation(QObject *parent):
    PropAnimation(parent),
    repeatTimes(0)
{
}

ScrawlPropAnimation::~ScrawlPropAnimation()
{

}

void ScrawlPropAnimation::startPlay()
{
    aniPath = cfg->getPluginPath() + "Animation\\PropAnimation\\" +
            QString("%1Prop\\%2").arg(prop.getID()).arg(prop.getID());
    PropAnimation::startPlay();
}

void ScrawlPropAnimation::stopPlay()
{
    aniCount = 0;
    repeatTimes = 0;
    PropAnimation::stopPlay();
}

void ScrawlPropAnimation::handlTimerOnTimeOut()
{
    if (!aniLabel)
    {
        stopPlay();
        emit onPlayEnd(prop);
        return;
    }
    if (aniCount < 16 && repeatTimes == 0)
    {
        aniCount++;
    }
    else if(aniCount == 16 && repeatTimes < 3000 / 67)
    {
        aniCount = 16;
        repeatTimes++;
    }
    else if(repeatTimes == 3000 / 67)
    {
        aniCount--;
    }
    QString path = aniPath + QString(" (%1).png").arg(aniCount);
    QDir *dir = new QDir;
    if (!dir->exists(path))
    {
        stopPlay();
        emit onPlayEnd(prop);
        return;
    }


    QPixmap pix(path);
    aniLabel->setFixedSize(pix.size());
    aniLabel->setPixmap(pix);
   // aniLabel->show();

}
