#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <QObject>
#include <QLabel>
#include <QTimer>
#define DEBUG_ANIMATOR   1
#if DEBUG_ANIMATOR
#include <QDebug>
#define AnimatorDebug(format,...) qDebug("%s,LINE: %d -->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define AnimatorDebug(format,...)
#endif
class Animator : public QObject
{
    Q_OBJECT
public:
    explicit Animator(QObject *parent = 0);
    ~Animator();
private slots:
    void handleAniTimeOut();

private:
    QLabel      *aniLabel;
    QTimer      aniTimer;
    int         aniCount;
    QString     aniPath;
    bool        bSelfDelete;
public:
    void Animate(QString path , QString name, QWidget *parent, QRect geo, int frameRate , bool selfDelete);
    void stop();
signals:
    void playEnd();
};
#endif // ANIMATOR_H
