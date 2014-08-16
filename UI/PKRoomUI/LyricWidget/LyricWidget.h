#ifndef LYRICWIDGET_H
#define LYRICWIDGET_H

#include "Modules/MMTimer/MMTimer.h"
#define DEBUG_LYRIC_WIDGET   1

#if DEBUG_LYRIC_WIDGET
#include <QDebug>
#define LyricWidgetDebug(format,...) qDebug("%s,LINE: %d -->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define LyricWidgetDebug(format,...)
#endif

#define FIRST_SENTENCE  0
#define SECOND_SENTENCE 1
//定时器间隔时间
#define UPDATE_INTERVAL 50
//循环时间
#define CIRCLE_TIME     100
//刷新次数
#define UPDATE_COUNT    (CIRCLE_TIME / UPDATE_INTERVAL)

#include <QWidget>
#include "LyricLabel.h"

namespace Ui {
class LyricWidget;
}

class LyricWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LyricWidget(QWidget *parent = 0);
    ~LyricWidget();

private:
    LyricLabel *upLyricLabel;
    LyricLabel *downLyricLabel;

    MMTimer    *timer;

    float      playerTime;
    float      lyricTime;
    float      deltaTime;

private:
    LyricParse *lyricParse;
public:
    void start(int senIndex);
    void stop();
    void reset();
    void updatePlayerTime(float playerTime);

private:
    Ui::LyricWidget *ui;
public slots:
    void handleTimeout();

signals:
    void OneSentenceEnd(int index);


};

#endif // LYRICWIDGET_H
