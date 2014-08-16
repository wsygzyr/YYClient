#ifndef PROPANIMATION_H
#define PROPANIMATION_H

#include <QObject>
#include "DataType/Prop/Prop.h"

#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/MMTimer/MMTimer.h"
#include <QTimer>
#include <QLabel>
#include <QSound>

#define DEBUG_PROP_ANIMATION  1
#if DEBUG_PROP_ANIMATION
#include <QDebug>
#define PropAnimationDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define PropAnimationDebug(format,...)
#endif

class PropAnimation : public QObject
{
    Q_OBJECT
public:
    explicit PropAnimation(QObject *parent = 0);
    virtual ~PropAnimation();
signals:
    void    onPlayEnd(Prop);
public slots:
    virtual void setProp(Prop) ;
    virtual void startPlay();
    virtual void stopPlay();
    virtual void setParams(QWidget *parent, QRect geometry);
protected:
    Prop         prop;
    MMTimer      *aniTimer;
    QSound       *audioPlayer;
    ConfigHelper *cfg;
    QLabel       *aniLabel;
    QString      aniPath;
    int          aniCount;
    QMutex       mutex;
protected slots:
    void    handleAudioPlayerOnFinished();
    virtual void  handlTimerOnTimeOut() = 0;


};

#endif // PROPANIMATION_H
