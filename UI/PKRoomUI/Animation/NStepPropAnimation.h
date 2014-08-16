#ifndef NSTEPPROPANIMATION_H
#define NSTEPPROPANIMATION_H
#include "PropAnimation.h"
class NStepPropAnimation:public PropAnimation
{
public slots:
    NStepPropAnimation(QObject* parent = 0);
    ~NStepPropAnimation();
    void startPlay();
    void stopPlay();
    void handlTimerOnTimeOut();
    void setAnimeStepCount(int stepCount);
    void setStepLoopCount(int step , int loopCount);
private:
    int  maxStepCount;
    int  currentStep;
    int  loopCountArray[256];  //step loopCount array
    int  currentLoopCount;
};

#endif // NSTEPPROPANIMATION_H
