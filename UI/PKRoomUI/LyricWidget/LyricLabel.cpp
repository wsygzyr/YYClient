#include "LyricLabel.h"

LyricLabel::LyricLabel(bool bUp,QWidget *parent) :
    QLabel(parent)
{
    LyricLabelDebug("lyricLabel()");
    this->bUp = bUp;

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setGeometry(0,0,1000,30);
    // 固定显示区域大小
    setMaximumSize(1000, 30);
    setMinimumSize(1000, 30);
    //歌词的线性渐变填充
    linearGradient.setStart(0, 10);//填充的起点坐标
    linearGradient.setFinalStop(0, 40);//填充的终点坐标
    //第一个参数终点坐标，相对于我们上面的区域而言，按照比例进行计算
    linearGradient.setColorAt(0.1, QColor(255, 255, 255));
    linearGradient.setColorAt(0.5, QColor(255, 255, 255));
    linearGradient.setColorAt(0.9, QColor(255, 255, 255));

    // 遮罩的线性渐变填充
    maskLinearGradient.setStart(0, 10);
    maskLinearGradient.setFinalStop(0, 40);
    maskLinearGradient.setColorAt(0.1, QColor(238, 204, 3));
    maskLinearGradient.setColorAt(0.5, QColor(238, 204, 3));
    maskLinearGradient.setColorAt(0.9, QColor(238, 204, 3));

    // 设置字体
    font.setFamily("幼圆");
    font.setBold(true);
    font.setPixelSize(17);
    lyricParse = LyricParse::getInstance();
    
    LyricLabelDebug("lyricLabel() successed");
}


void LyricLabel::paintEvent(QPaintEvent *e)
{
    LyricLabelDebug("LyricLabel paintEvent");
    QPainter painter(this);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing);  //消除锯齿
    // 先绘制底层文字，作为阴影，这样会使显示效果更加清晰，且更有质感
    painter.setPen(QColor(0, 0, 0, 200));//黑色

    int x = dynamic_cast<QWidget*>(this->parent())->width() - plainStrWidth;
    if(x < 0|| this->bUp)
        x = 0;
    painter.drawText(x, 1, plainStrWidth, 30, Qt::AlignLeft|Qt::AlignVCenter, text());//左对齐

    // 再在上面绘制渐变文字
    painter.setPen(QPen(linearGradient, 0));
    painter.drawText(x, 0, plainStrWidth, 30, Qt::AlignLeft|Qt::AlignVCenter , text());

    // 设置歌词遮罩
    painter.setPen(QPen(maskLinearGradient, 0));
    painter.drawText(x, 0, highLightStrWidth, 30, Qt::AlignLeft|Qt::AlignVCenter , text());

    if(this->x() + this->width() >= dynamic_cast<QWidget*>(this->parent())->x() + dynamic_cast<QWidget*>(this->parent())->width())
        move(highLightStrWidth > LYRIC_LABEL_SLIDE_DISTANCE ? (LYRIC_LABEL_SLIDE_DISTANCE - highLightStrWidth) : 0, y());
//    LyricLabelDebug("sentence is %s ,update over",text().toUtf8().data());
}

int LyricLabel::calcHighLightWidth(float lyricTime)
{
    QFontMetrics fm(font);

    if (lyricTime <= lyricParse->getSentenceBeginTime(sentenceIndex))
    {
        return 0;
    }

    if (lyricTime >= lyricParse->getSentenceEndTime(sentenceIndex))
    {
        QString strSentence = lyricParse->getSentenceString(sentenceIndex);
        return fm.width(strSentence);
    }


    int highLightWidth = 0;


    for (int i = 0; i < lyricParse->getWordCount(sentenceIndex); i++)
    {

        float wordBeginTime = lyricParse->getWordBeginTime(sentenceIndex, i);
        float wordEndTime = lyricParse->getWordEndTime(sentenceIndex, i);
        QString wordString = lyricParse->getWordString(sentenceIndex, i);

        int wordWidth = fm.width(wordString);

        if (lyricTime > wordEndTime)
        {
            highLightWidth += wordWidth;
            continue;
        }

        if (lyricTime > wordBeginTime)
        {
            highLightWidth += (lyricTime - wordBeginTime) / (wordEndTime - wordBeginTime) * wordWidth;
        }

//        LyricLabelDebug("lyricTime is: %f, word is: %s, wordEndTime is: %f, highLightWidth is: %d",
//                        lyricTime, wordString.toUtf8().data(), wordEndTime, highLightWidth);

        break;

    }

    return highLightWidth;
}

void LyricLabel::setFont(QFont tempFont)
{
    font = tempFont;
}


void LyricLabel::reset()
{

}

void LyricLabel::setSentence(int sentenceIndex)
{
//    LyricLabelDebug("setSentence! sentenceIndex is: %d", sentenceIndex);

    if(sentenceIndex >= lyricParse->getSentenceCount())
    {
        LyricLabelDebug("sentenceIndex >= lyricParse->getSentenceCount()!");
        return ;
    }

    this->sentenceIndex = sentenceIndex;

    QString sentenceString = lyricParse->getSentenceString(sentenceIndex);
    QFontMetrics fm(font);

    this->plainStrWidth = fm.width(sentenceString);
    this->highLightStrWidth = 0;

    this->setText(sentenceString);
 //   LyricLabelDebug("index :%d ,sentenceString is %s" ,sentenceIndex ,sentenceString.toUtf8().data());

    repaint();
}


int LyricLabel::getSentenceIndex() const
{
    return this->sentenceIndex;
}

void LyricLabel::updateTime(float time)
{
//    LyricLabelDebug("updateTime! time is: %f", time);

    highLightStrWidth = this->calcHighLightWidth(time);

    if (0 >= highLightStrWidth)
    {
        LyricLabelDebug("quit because highLightStrWidth is: %d", highLightStrWidth);
        return;
    }

    repaint();
}
