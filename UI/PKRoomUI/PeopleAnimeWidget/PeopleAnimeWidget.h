#ifndef PEOPLEANIMEWIDGET_H
#define PEOPLEANIMEWIDGET_H

#include <QWidget>
#include <DataType/Prop/Prop.h>
#include <QGraphicsOpacityEffect>
#include <QFont>
#include <QTimer>
#include "Modules/FadeHelper/FadeHelper.h"
#include "Modules/Animator/Animator.h"
#include "UI/PKRoomUI/Animation/AnimationManager.h"
#define DEBUG_PEOPLE_ANIME_WIDGET  1

#if DEBUG_PEOPLE_ANIME_WIDGET
#include <QDebug>
#define PeopleAnimeWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define PeopleAnimeWidgetDebug(format,...)
#endif

namespace Ui {
class PeopleAnimeWidget;
}

class PeopleAnimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PeopleAnimeWidget(QWidget *parent = 0);
    ~PeopleAnimeWidget();
    void animateProp(Prop prop);
    void stopAnimateProp();
    void updateText(int priority, QString str);
private:
    Ui::PeopleAnimeWidget *ui;

    bool  isPlayingText;
    int   currentPlayingPriority;
    int   nextPlayingPriority;
    QString textStr;

    QTimer *textCountDownTimer;
    QFont font;

    FadeHelper *fadeOutHelper;
    Animator   *cheerAnimator;
    AnimationManager *ani;
    bool       bPlayAni;
    QTimer      *timer;
private slots:
    void handlePropAnimatorOnPlayEnd(Prop);
    void handleTextCountDownTimerOnTimeOut();
    void handleFadeHelperOnFadeEnd();
    void handleCheerAnimatorOnPlayEnd();
};

#endif // PEOPLEANIMEWIDGET_H
