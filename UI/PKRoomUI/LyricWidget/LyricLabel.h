#ifndef LyricLabel_H
#define LyricLabel_H

#include <QLabel>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QPainter>
#include "LyricParse/LyricParse.h"

#define DEBUG_LYRIC_LABEL   1
#if DEBUG_LYRIC_LABEL
#include <QDebug>
#define LyricLabelDebug(format,...) qDebug("%s,LINE: %d -->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define LyricLabelDebug(format,...)
#endif

#define LYRIC_LABEL_WIDTH           300
#define LYRIC_LABEL_SLIDE_DISTANCE  200

#define INTERVAL_TIME               10
#define CIRCLE_TIME                 100


class LyricLabel : public QLabel
{
    Q_OBJECT
public:
    explicit LyricLabel(bool bUp, QWidget *parent);


protected:
    void paintEvent(QPaintEvent *e);

private:
    QLinearGradient linearGradient;
    QLinearGradient maskLinearGradient;
    QFont font;

    int plainStrWidth;
    int highLightStrWidth;

    int sentenceIndex;

    LyricParse *lyricParse;

    bool bUp;
private:
    int calcHighLightWidth(float time);

public:
    void reset();
    void setFont(QFont tempFont);

    void setSentence(int sentenceIndex);
    int getSentenceIndex() const;
    void updateTime(float time);
};

#endif // LyricLabel_H
