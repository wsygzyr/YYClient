#ifndef FADEHELPER_H
#define FADEHELPER_H

#include <QObject>
#include <QWidget>
#include <QGraphicsOpacityEffect>
#include <QTimer>
class FadeHelper : public QObject
{
    Q_OBJECT
public:
    typedef enum
    {
        FADE_IN,
        FADE_OUT
    }FadeType;
    explicit FadeHelper(QObject *parent = 0);
    ~FadeHelper();
private:
    QWidget  *fadeWidget;
    FadeType type;
    int      fadeTime;
    int      m_nFadeOutTime;
    qreal    opacity;
    qreal    deltaOpacity;
    int      deltaTime;
    QGraphicsOpacityEffect *opacityEffect;
    QTimer   *timer;

    void     timerEvent(QTimerEvent *);
    bool     m_bDealFadeProcess;
signals:
    void     fadeEnd();
public slots:
    void     setFadeWidget(QWidget*);
    void     setFadeType(FadeType);
    void     setFadeTime(int msec);
    void     startFade();
    void     DealFadeProcess(QWidget *widget, int fadeInTime=0, int fadeOutTime=0, int showTime=0);
//    void     DealFadeIn(QWidget *widget, FadeHelper::FadeType type, int msec);
//    void     DealFadeOut(QWidget *widget, FadeHelper::FadeType type, int msec, int showtime);
    void     DealShowProcess();
    void     StopShow();
};

#endif // FADEHELPER_H
