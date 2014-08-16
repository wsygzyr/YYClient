#ifndef SESMARKCALLER_H
#define SESMARKCALLER_H

#include <QMutex>

#include <QList>

#include "Modules/LyricParse/LyricParse.h"
#include "Modules/AACEncode/AACEncodeCaller.h"
#include "Modules/PkInfoManager/PkInfoManager.h"

#include "DataType/Score/Score.h"



#define DEBUG_SES_MARK_CALLER  1

#if DEBUG_SES_MARK_CALLER
#include <QDebug>
#define SESMarkCallerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define SESMarkCallerDebug(format,...)
#endif

#ifdef _DEBUG
#define SESDLL "dll/libSESd.dll"
#else
#define SESDLL "dll/libSES.dll"
#endif



class SESMarkCaller : public QObject
{
    Q_OBJECT

    SESMarkCaller(QObject *parent = 0);
    ~SESMarkCaller();
/*
 *  single instance
 */
private:
    static QAtomicPointer<SESMarkCaller> instance;
    static QMutex instanceMutex;
public:
    static SESMarkCaller *getInstance();
    static void destroy();


/*
 *  SES score callback
 */
private:
    QList<Score> scoreList;
public:
    void notifyOnError(bool error);
    void notifyOnScore(bool error, Score score);
signals:
    void onScore(Score score, Score sumScore);
    void onError();

private:
    int pcmBytesCount;
    float recordTimeOffset;
    int sectionIndex;
    int sentenceIndex;

    LyricParse *lyricParse;
    AACEncodeCaller *aac;
    PkInfoManager *info;

    void calcPropScore(Score &score);
    Score calcSumScore();

/*
 *  APIs
 */

public slots:
    bool start(int sectionIndex, int samplerate);
    void stop();
    void sendAudio(QByteArray);
};

#endif // SESCALLER_H
