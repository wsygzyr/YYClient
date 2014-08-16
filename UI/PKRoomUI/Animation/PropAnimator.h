#ifndef PROPANIMATOR_H
#define PROPANIMATOR_H

#include <QObject>
#include <QLabel>
#include <QTimer>
#include "Prop/Prop.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/AudioPlayer/AudioPlayer.h"
#define DEBUG_PROP_ANIMATOR   1
#if DEBUG_PROP_ANIMATOR
#include <QDebug>
#define PropAnimatorDebug(format,...) qDebug("%s,LINE: %d -->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define PropAnimatorDebug(format,...)
#endif
class PropAnimator : public QObject
{
    Q_OBJECT
public:
    explicit PropAnimator(QObject *parent = 0);

private slots:
    void handleAniTimeOut();

private:
    QLabel      *aniLabel;
    QTimer      aniTimer;
    int         aniCount;
    QString     aniPath;

    Prop        prop;
    bool        isComplexAni;
    QString     state;
    int         loopCount;

    ConfigHelper  *cfg;
    AudioPlayer   *audioPlayer;
public:
    void AnimateProp(Prop &prop, QWidget *parent, QRect geometry);
    void stopAnimate();
signals:
    void propPlayEnd(Prop);
};

#endif // PROPANIMATOR_H
