#ifndef LYRICPARSE_H
#define LYRICPARSE_H

#include    <QFile>
#include    <QTextCodec>
#include    <QTextStream>
#include    <QMutex>
#include    <QVector>
#include    <QList>

#define DEBUG_LYRIC_PARSE   1
#if DEBUG_LYRIC_PARSE
#include <QDebug>
#define LyricParseDebug(format,...) qDebug("%s,LINE: %d -->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define LyricParseDebug(format,...)
#endif

#define SENTENCE_BEFORE   -1     //位于歌词开始之前
#define SENTENCE_BETWEEN  -2     //在句子中
#define SENTENCE_END      -3     //位于歌词结束之后


class LyricParagraph;
class LyricSection;

class LyricParse
{

private:
    LyricParse();
    ~LyricParse();

/*
 *  singleton
 */
private:
    static QAtomicPointer<LyricParse> instance;
    static QMutex instanceMutex;

public:
    static LyricParse* getInstance();
    static void destory();

private:
    QString     songName;
    QString     singerName;
    QString     lyricXmlData;
    int         sectionCountHint;

    LyricParagraph  *paragraph;
    QList<LyricSection>   *sections;

/*
 *  APIs
 */

public:

    void setSectionCountHint(const int & count);
    bool parseLyricXML(const QString &lyricXML, const QString &xmlData);
    bool parseSectionXML(const QString &sectionXML, const QString &xmlDataStr);

    void reset();


    QString getSongName() const;
    QString getSingerName() const;

    float getBeginTime() const;
    float getEndTime() const;

    int getSentenceCount() const;
    QString getSentenceString(int sentenceIndex) const;
    float getSentenceBeginTime(int sentenceIndex) const;
    float getSentenceEndTime(int sentenceIndex) const;
    int getSentenceIndexWithTime(float time) const;

    int getWordCount(int sentenceIndex) const;
    QString getWordString(int sentenceIndex, int wordIndex) const;
    float getWordBeginTime(int sentenceIndex, int wordIndex) const;
    float getWordEndTime(int sentenceIndex, int wordIndex) const;

    int getSectionCount() const;
    int getSectionFirstSentenceIndex(int sectionIndex) const;
    int getSectionLastSentenceIndex(int sectionIndex) const;
    float getSectionBeginTime(int sectionIndex) const;
    float getSectionEndTime(int sectionIndex) const;

    QString getLyricXmlData() const;

    int  getRapSentenceCountBeforeIndex(int stIndex);
    int  getRealSentenceIndex(int stIndex);
};

#endif // LYRICPARSE_H
