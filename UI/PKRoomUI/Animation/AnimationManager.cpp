#include "AnimationManager.h"

QAtomicPointer<AnimationManager> AnimationManager::instance = 0;
QMutex AnimationManager::instanceMutex;
AnimationManager *AnimationManager::getInstance()
{
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            instance = new AnimationManager();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void AnimationManager::destory()
{
    AnimationManagerDebug("AnimationManager destory");
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}



AnimationManager::AnimationManager(QObject *parent) :
    QObject(parent)
{
}

void AnimationManager::addAnimation(QWidget *parent , QRect geometry , Prop prop)
{
    AnimationManagerDebug("AnimationManager addAnimation");
    PropAnimation *ani = NULL;
    stopAnimation(prop);
    if(!animeHash.contains(prop.getID()))
    {
        AnimationManagerDebug("prop:%d is not exist, play now" , prop.getID());
        switch (prop.getID())
        {
        case PROP_ID_LUCKYWING:
        case PROP_ID_PIG:
        case PROP_ID_KISS:
        case PROP_ID_FROG:
        {
            ani = new NStepPropAnimation(this);
            dynamic_cast<NStepPropAnimation*>(ani)->setAnimeStepCount(1);
            break;
        }

        case PROP_ID_TRANSFORMER:
        case PROP_ID_TOMCAT:
        {
            ani = new NStepPropAnimation(this);
            dynamic_cast<NStepPropAnimation*>(ani)->setAnimeStepCount(3);
            dynamic_cast<NStepPropAnimation*>(ani)->setStepLoopCount(2 , 11);
            break;
        }
        case PROP_ID_SCRAWL:
        {
            ani = new ScrawlPropAnimation(this);
            break;
        }
        default:
            break;
        }
        ani->setProp(prop);
        ani->setParams(parent , geometry);
        connect(ani , SIGNAL(onPlayEnd(Prop)) , this , SLOT(handleAnimationOnPlayEnd(Prop)));
        animeHash.insert(prop.getID() , ani);
    }
    ani = animeHash[prop.getID()];
    ani->startPlay();
}

void AnimationManager::stopAnimation(Prop prop)
{
    AnimationManagerDebug("AnimationManager stopAnimation");
    if(animeHash.contains(prop.getID()))
    {
        AnimationManagerDebug("prop :%d exist , stop now" , prop.getID());
        animeHash[prop.getID()]->stopPlay();
    }
}

void AnimationManager::stopAllAnimation()
{
    AnimationManagerDebug("AnimationManager stopAllAnimation");
    QHash<int , PropAnimation*>::iterator i = animeHash.begin();
    while(i != animeHash.end())
    {
        i = animeHash.erase(i);
    }
}

void AnimationManager::handleAnimationOnPlayEnd(Prop prop)
{
    AnimationManagerDebug("AnimationManager handleAnimationOnPlayEnd");
    //stopAnimation(prop);
    AnimationManagerDebug("prop :%s play end ",prop.getName().toUtf8().data());
    emit onPlayEnd(prop);
}
