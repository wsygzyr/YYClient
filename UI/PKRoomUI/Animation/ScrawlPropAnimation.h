#ifndef SCRAWLPROPANIMATION_H
#define SCRAWLPROPANIMATION_H
#include "PropAnimation.h"
class ScrawlPropAnimation : public PropAnimation
{
public slots:
    ScrawlPropAnimation(QObject* parent = 0);
    ~ScrawlPropAnimation();
    void startPlay();
    void stopPlay();
private:
    int  repeatTimes;
    void handlTimerOnTimeOut();
};

#endif // SCRAWLPROPANIMATION_H
