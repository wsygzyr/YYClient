#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QObject>
#include <QHash>
#include "NStepPropAnimation.h"
#include "ScrawlPropAnimation.h"
#include "DataType/Prop/Prop.h"
#define ANIMATION_MANAGER_DEBUG 1

#if ANIMATION_MANAGER_DEBUG
#include <QDebug>
#define AnimationManagerDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define AnimationManagerDebug(format, ...)
#endif
class AnimationManager : public QObject
{
    Q_OBJECT
private:
    static QAtomicPointer<AnimationManager> instance;
    static QMutex instanceMutex;

public:
    static AnimationManager* getInstance();
    static void destory();
public:
    explicit AnimationManager(QObject *parent = 0);
    void     addAnimation(QWidget *parent, QRect geometry, Prop);
    void     stopAnimation(Prop);
    void     stopAllAnimation();
signals:
    void     onPlayEnd(Prop);
private slots:
    void     handleAnimationOnPlayEnd(Prop);
private:
    QHash<int ,PropAnimation*> animeHash;
};

#endif // ANIMATIONMANAGER_H
