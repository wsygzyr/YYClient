#include "LyricParse.h"

enum eSentenceType
{
    NORMAL,
    RAP,
    OVERLAP
};

class LyricWord
{
public:
    float   beginTime;
    float   endTime;
    int     lang;
    QString content;
};

class LyricSentence
{
public:
    LyricSentence()
    {
        words = new QVector<LyricWord*>;
        bNormalSt = false;
    }
    ~LyricSentence()
    {
        clear();
        delete words;
    }

public:
    QVector<LyricWord*> *words;
    bool bNormalSt;
    int size() const
    {
        return words->size();
    }

    void clear()
    {
        for (int i = 0; i < words->size(); i++)
        {
            LyricWord *word = words->at(i);
            delete word;
        }
        words->clear();
    }
};

class LyricParagraph
{
public:
    LyricParagraph()
    {
        sentences = new QVector<LyricSentence*>;
    }
    ~LyricParagraph()
    {
        clear();
        delete sentences;
    }

public:
    QVector<LyricSentence*> *sentences;

    int size() const
    {
        return sentences->size();
    }

    void clear()
    {
        for (int i = 0; i < sentences->size(); i++)
        {
            LyricSentence *sentence = sentences->at(i);
            delete sentence;
        }
        sentences->clear();
    }
};

class LyricSection
{
public:
    int firstSentenceIndex;
    int lastSentenceIndex;
};


QAtomicPointer<LyricParse> LyricParse::instance = 0;
QMutex LyricParse::instanceMutex;

LyricParse::LyricParse()
{
    paragraph = new LyricParagraph;
    sections = new QList<LyricSection>();
}

LyricParse::~LyricParse()
{
    delete paragraph;
    delete sections;
}

LyricParse *LyricParse::getInstance()
{
    //    LyricParseDebug("getInstance!");
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            LyricParseDebug("create instance!");
            instance = new LyricParse();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void LyricParse::destory()
{
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = 0;
    }
    instanceMutex.unlock();
}
QString LyricParse::getLyricXmlData() const
{
    return lyricXmlData;
}

int LyricParse::getRapSentenceCountBeforeIndex(int stIndex)
{
    if(stIndex > this->paragraph->sentences->size())
    {
        LyricParseDebug("out of sentence range");
        return -1;
    }
    int rapCount = 0;
    for(int i = 0 ; i < stIndex ; i++)
    {
        if(!this->paragraph->sentences->at(i)->bNormalSt)
        {
            rapCount++;
        }
    }
    return rapCount;
}

int LyricParse::getRealSentenceIndex(int stIndex)
{
    if(stIndex > this->paragraph->sentences->size())
    {
        LyricParseDebug("out of sentence range");
        return -1;
    }
    if(!this->paragraph->sentences->at(stIndex)->bNormalSt)
    {
        for(int i = stIndex ; i < this->paragraph->sentences->size() ; i++)
        {
            if(this->paragraph->sentences->at(i)->bNormalSt)
            {
                stIndex = i;  //find the first normal sentence index
                break;
            }
        }
    }
    return (stIndex - getRapSentenceCountBeforeIndex(stIndex));

}




bool LyricParse::parseLyricXML(const QString &lyricXML, const QString &xmlData)
{
    LyricParseDebug("start parseLyricXML ...");
    QString xmlDataStr = xmlData;
    lyricXmlData = xmlDataStr;
    xmlDataStr.replace("&gt;" ,">");
    xmlDataStr.replace("&lt;" ,"<");
    xmlDataStr.replace("&amp;" ,"&");
    xmlDataStr.replace("&apos;" ,"'");
    xmlDataStr.replace("&quot;" ,"\"");

    if (xmlDataStr.isEmpty())
    {
        LyricParseDebug("xmlDataStr is Empty!");
        return false;
    }
    //part 1. parse songName and singerName
    qint64 startFindPos = 0;
    qint64 generalStartPos = xmlDataStr.indexOf("<general>", startFindPos);
    qint64 generalEndPos = xmlDataStr.indexOf("</general>", startFindPos);
    if (generalStartPos == -1 || generalEndPos == -1)
    {
        LyricParseDebug("general parse failed!");
        return false;
    }
    startFindPos = generalEndPos;
    QString generalContent = xmlDataStr.mid(generalStartPos + 9,
                                            generalEndPos - generalStartPos - 9);
    qint64 generalFindPos = 0;
    qint64 songNameStartPos = generalContent.indexOf("<name>", generalFindPos);
    qint64 songNameEndPos = generalContent.indexOf("</name>", generalFindPos);
    if (songNameStartPos == -1 || songNameEndPos == -1)
    {
        LyricParseDebug("songName parse failed!");
        return false;
    }
    generalFindPos = songNameEndPos;
    this->songName = generalContent.mid(songNameStartPos + 6,
                                        songNameEndPos - songNameStartPos - 6);
    qint64 singerNameStartPos = generalContent.indexOf("<singer>", generalFindPos);
    qint64 singerNameEndPos = generalContent.indexOf("</singer>", generalFindPos);
    if (singerNameStartPos == -1 || singerNameEndPos == -1)
    {
        LyricParseDebug("singerName parse failed!");
        return false;
    }
    generalFindPos = singerNameEndPos;
    this->singerName = generalContent.mid(singerNameStartPos + 8,
                                          singerNameEndPos - singerNameStartPos - 8);
    //part 2. parse midi_lrc
    qint64 midi_lrcStartPos = xmlDataStr.indexOf("<midi_lrc>", startFindPos);
    qint64 midi_lrcEndPos = xmlDataStr.indexOf("</midi_lrc>", startFindPos);
    if (midi_lrcStartPos == -1 || midi_lrcEndPos == -1)
    {
        LyricParseDebug("midi_lrc parse failed!");
        return false;
    }
    startFindPos = midi_lrcEndPos;

    LyricParseDebug("start parse midi_lrc");
    QString midi_lrcContent = xmlDataStr.mid(midi_lrcStartPos + 10,
                                             midi_lrcEndPos - midi_lrcStartPos - 10);
    this->paragraph->clear();

    qint64 midi_lrcFindPos = 0;
    while (midi_lrcFindPos < midi_lrcEndPos)
    {
        qint64 sentenceStartPos =
                midi_lrcContent.indexOf("<sentence", midi_lrcFindPos);
        qint64 sentenceEndPos =
                midi_lrcContent.indexOf("</sentence>", midi_lrcFindPos);

        qint64 rapStartPos =
                midi_lrcContent.indexOf("<rap ", midi_lrcFindPos);
        qint64 rapEndPos =
                midi_lrcContent.indexOf("</rap>", midi_lrcFindPos);
        if(sentenceStartPos == -1 && rapStartPos == -1)
        {
            LyricParseDebug("no sentence or rap");
            break;
        }
        LyricSentence *lyricSentence = new LyricSentence();
        if((sentenceStartPos != -1 && sentenceStartPos < rapStartPos) || rapStartPos == -1)
        {
            if (sentenceStartPos == -1)
            {
                LyricParseDebug("midi_lrc parse finished!");
                break;
            }
            if (sentenceEndPos == -1)
            {
                LyricParseDebug("<sentence> and </sentence> mismatching");
                return false;
            }
            midi_lrcFindPos = sentenceEndPos + 11;
            QString sentenceContent = midi_lrcContent.mid(sentenceStartPos + 10,
                                                          sentenceEndPos - sentenceStartPos - 10);
//            LyricParseDebug("sentenceEndPos - sentenceStartPos - 10 is %d", sentenceEndPos - sentenceStartPos - 10);
            qint64 sentenceFindPos = 0;
            //part 3. parse sentence
            //        LyricParseDebug("start parse sentence");

 //           LyricParseDebug("sentenceContent is %s" , sentenceContent.toUtf8().data());
            while (sentenceFindPos < sentenceEndPos)
            {
                qint64 toneStartPos = sentenceContent.indexOf("<tone", sentenceFindPos);
                qint64 toneEndPos = sentenceContent.indexOf("</tone>", sentenceFindPos);
                if(toneStartPos != -1 || toneEndPos != -1)
                {
                    if (toneStartPos == -1)
                    {
                        //                LyricParseDebug("sentence parse finished!");
                        break;
                    }
                    if (toneEndPos == -1)
                    {
                        LyricParseDebug("<tone> and </tone> mismatching");
                        return false;
                    }
                    sentenceFindPos = toneEndPos + 7;
                    QString toneContent = sentenceContent.mid(toneStartPos + 5,
                                                              toneEndPos - toneStartPos - 5);
                    //part 4. parse tone
                    LyricWord *lyricWord = new LyricWord();
                    qint64 toneFindPos = 0;
                    //beginTime
                    qint64 startPos = toneContent.indexOf("begin=\"", toneFindPos);
                    if (startPos == -1)
                    {
                        LyricParseDebug("beginTime parse failed!");
                        return false;
                    }
                    qint64 endPos = toneContent.indexOf("\"", startPos+7);
                    if (endPos == -1)
                    {
                        LyricParseDebug("beginTime parse failed!");
                        return false;
                    }
                    toneFindPos = endPos;
                    bool isToFloatOk = true;
                    float time = toneContent.mid(startPos + 7,
                                                 endPos - startPos - 7).toFloat(&isToFloatOk) * 1000;
                    if (! isToFloatOk)
                    {
                        LyricParseDebug("beginTime parse failed!");
                        return false;
                    }
                    lyricWord->beginTime = time;


                    //endTime
                    startPos = toneContent.indexOf("end=\"", toneFindPos);
                    if (startPos == -1)
                    {
                        LyricParseDebug("endTime parse failed!");
                        return false;
                    }
                    endPos = toneContent.indexOf("\"", startPos + 5);
                    if (endPos == -1)
                    {
                        LyricParseDebug("endTime parse failed!");
                        return false;
                    }
                    toneFindPos = endPos;
                    time = toneContent.mid(startPos + 5,
                                           endPos - startPos - 5).toFloat(&isToFloatOk) * 1000;
                    if (! isToFloatOk)
                    {
                        LyricParseDebug("endTime parse failed!");
                        return false;
                    }
                    lyricWord->endTime = time;

                    // lang
                    int lang = 1;
                    bool isToIntOk = false;
                    startPos = toneContent.indexOf("lang=\"", toneFindPos);
                    if (startPos == -1)
                    {
                        LyricParseDebug("lang parse failed!");
                        return false;
                    }
                    endPos = toneContent.indexOf("\"", startPos + 6);
                    if (endPos == -1)
                    {
                        LyricParseDebug("lang parse failed!");
                        return false;
                    }
                    toneFindPos = endPos;
                    lang = toneContent.mid(startPos + 6,
                                           endPos - startPos - 6).toInt(&isToIntOk);
                    if (!isToIntOk)
                    {
                        LyricParseDebug("lang parse failed!");
                        return false;
                    }
                    lyricWord->lang = lang;

                    //word
                    startPos = toneContent.indexOf("<word>", toneFindPos);
                    if (startPos == -1)
                    {
                        LyricParseDebug("word parse failed!");
                        return false;
                    }
                    endPos = toneContent.indexOf("</word>", startPos + 6);
                    if (endPos == -1)
                    {
                        LyricParseDebug("word parse failed!");
                        return false;
                    }
                    toneFindPos = endPos;
                    lyricWord->content =
                            toneContent.mid(startPos + 6,
                                            endPos - startPos - 6);
                    if(lyricWord->lang == 3 && lyricWord->content != "")
                    {
                        lyricWord->content.append(" "); // if the word is in English , the word must append a whiteSpace
                    }
//                    LyricParseDebug("\nbegin is %f, end is %f, word is %s",
//                                    lyricWord->beginTime,
//                                    lyricWord->endTime,
//                                    lyricWord->content.toUtf8().data());
                    lyricSentence->words->append(lyricWord);
                    lyricSentence->bNormalSt = true;
                }
                else //parse monolog
                {
                    qint64 monologStartPos = sentenceContent.indexOf("<monolog", sentenceFindPos);
                    qint64 monologEndPos = sentenceContent.indexOf("</monolog>", sentenceFindPos);
                    if(monologStartPos == -1)
                    {
                        break;
                    }
                    if(monologEndPos == -1)
                    {
                        LyricParseDebug("monolog does't match");
                        return false;
                    }
                    sentenceFindPos = monologEndPos + 10;
                    QString monologContent = sentenceContent.mid(monologStartPos + 9,
                                                              monologEndPos - monologStartPos - 9);
                    LyricParseDebug("rapContent is %s" ,monologContent.toUtf8().data());
                    qint64 monologFindPos = 0;
                    //part 4. parse tone
                    LyricWord *lyricWord = new LyricWord();
                    //beginTime
                    qint64 startPos = monologContent.indexOf("begin=\"", monologFindPos);
                    if (startPos == -1)
                    {
                        LyricParseDebug("monolog beginTime parse failed!");
                        return false;
                    }
                    qint64 endPos = monologContent.indexOf("\"", startPos+7);
                    if (endPos == -1)
                    {
                        LyricParseDebug("monolog beginTime parse failed!");
                        return false;
                    }
                    monologFindPos = endPos;
                    bool isToFloatOk = true;
                    float time = monologContent.mid(startPos + 7,
                                                endPos - startPos - 7).toFloat(&isToFloatOk) * 1000;
                    if (! isToFloatOk)
                    {
                        LyricParseDebug("beginTime parse failed!");
                        return false;
                    }
                    lyricWord->beginTime = time;


                    //endTime
                    startPos = monologContent.indexOf("end=\"", monologFindPos);
                    if (startPos == -1)
                    {
                        LyricParseDebug("rap endTime parse failed!");
                        return false;
                    }
                    endPos = monologContent.indexOf("\"", startPos + 5);
                    if (endPos == -1)
                    {
                        LyricParseDebug("rap endTime parse failed!");
                        return false;
                    }
                    monologFindPos = endPos;
                    time = monologContent.mid(startPos + 5,
                                          endPos - startPos - 5).toFloat(&isToFloatOk) * 1000;
                    if (! isToFloatOk)
                    {
                        LyricParseDebug("endTime parse failed!");
                        return false;
                    }
                    lyricWord->endTime = time;

                    // lang
                    int lang = 1;
                    bool isToIntOk = false;
                    startPos = monologContent.indexOf("lang=\"", monologFindPos);
                    if (startPos == -1)
                    {
                        LyricParseDebug("rap lang parse failed!");
                        return false;
                    }
                    endPos = monologContent.indexOf("\"", startPos + 6);
                    if (endPos == -1)
                    {
                        LyricParseDebug("lang parse failed!");
                        return false;
                    }
                    monologFindPos = endPos;
                    lang = monologContent.mid(startPos + 6,
                                          endPos - startPos - 6).toInt(&isToIntOk);
                    if (!isToIntOk)
                    {
                        LyricParseDebug("lang parse failed!");
                        return false;
                    }
                    lyricWord->lang = lang;

                    lyricWord->content = monologContent.mid(monologFindPos + 2);
                    LyricParseDebug("\nbegin is %f, end is %f, word is %s",
                                    lyricWord->beginTime,
                                    lyricWord->endTime,
                                    lyricWord->content.toUtf8().data());
                    lyricSentence->words->append(lyricWord);

                }
            }
        }
        else if ((rapStartPos != -1 && rapStartPos < sentenceStartPos) || sentenceStartPos == -1)
        {
            if (rapStartPos == -1)
            {
                //            LyricParseDebug("midi_lrc parse finished!");
                break;
            }
            if (rapEndPos == -1)
            {
                LyricParseDebug("<rap> and </rap> mismatching");
                return false;
            }
            midi_lrcFindPos = rapEndPos + 6;
            QString rapContent = midi_lrcContent.mid(rapStartPos + 5,
                                                     rapEndPos - rapStartPos - 5);
            LyricParseDebug("rapContent is %s" ,rapContent.toUtf8().data());
            qint64 rapFindPos = 0;
            //part 4. parse tone
            LyricWord *lyricWord = new LyricWord();
            //beginTime
            qint64 startPos = rapContent.indexOf("begin=\"", rapFindPos);
            if (startPos == -1)
            {
                LyricParseDebug("rap beginTime parse failed!");
                return false;
            }
            qint64 endPos = rapContent.indexOf("\"", startPos+7);
            if (endPos == -1)
            {
                LyricParseDebug("rap beginTime parse failed!");
                return false;
            }
            rapFindPos = endPos;
            bool isToFloatOk = true;
            float time = rapContent.mid(startPos + 7,
                                        endPos - startPos - 7).toFloat(&isToFloatOk) * 1000;
            if (! isToFloatOk)
            {
                LyricParseDebug("beginTime parse failed!");
                return false;
            }
            lyricWord->beginTime = time;


            //endTime
            startPos = rapContent.indexOf("end=\"", rapFindPos);
            if (startPos == -1)
            {
                LyricParseDebug("rap endTime parse failed!");
                return false;
            }
            endPos = rapContent.indexOf("\"", startPos + 5);
            if (endPos == -1)
            {
                LyricParseDebug("rap endTime parse failed!");
                return false;
            }
            rapFindPos = endPos;
            time = rapContent.mid(startPos + 5,
                                  endPos - startPos - 5).toFloat(&isToFloatOk) * 1000;
            if (! isToFloatOk)
            {
                LyricParseDebug("endTime parse failed!");
                return false;
            }
            lyricWord->endTime = time;

            // lang
            int lang = 1;
            bool isToIntOk = false;
            startPos = rapContent.indexOf("lang=\"", rapFindPos);
            if (startPos == -1)
            {
                LyricParseDebug("rap lang parse failed!");
                return false;
            }
            endPos = rapContent.indexOf("\"", startPos + 6);
            if (endPos == -1)
            {
                LyricParseDebug("lang parse failed!");
                return false;
            }
            rapFindPos = endPos;
            lang = rapContent.mid(startPos + 6,
                                  endPos - startPos - 6).toInt(&isToIntOk);
            if (!isToIntOk)
            {
                LyricParseDebug("lang parse failed!");
                return false;
            }
            lyricWord->lang = lang;

            lyricWord->content = rapContent.mid(rapFindPos + 2);
            LyricParseDebug("\nbegin is %f, end is %f, word is %s",
                            lyricWord->beginTime,
                            lyricWord->endTime,
                            lyricWord->content.toUtf8().data());
            lyricSentence->words->append(lyricWord);
            lyricSentence->bNormalSt = false;
        }
        this->paragraph->sentences->append(lyricSentence);
    }
    LyricParseDebug("lyricParse finished!");
    return true;
}

bool LyricParse::parseSectionXML(const QString &sectionXML,const QString &xmlDataStr)
{
    LyricParseDebug("start parse SectionParse!");
    if (xmlDataStr.isEmpty())
    {
        LyricParseDebug("xmlDataStr is Empty!");
        return false;
    }
    //part 1. parse songName and singerName
    qint64 startFindPos = 0;
    qint64 generalStartPos = xmlDataStr.indexOf("<general>", startFindPos);
    qint64 generalEndPos = xmlDataStr.indexOf("</general>", startFindPos);
    if (generalStartPos == -1 || generalEndPos == -1)
    {
        LyricParseDebug("general parse failed!");
        return false;
    }
    startFindPos = generalEndPos;
    QString generalContent = xmlDataStr.mid(generalStartPos + 9,
                                            generalEndPos - generalStartPos - 9);
    qint64 generalFindPos = 0;
    qint64 songNameStartPos = generalContent.indexOf("<name>", generalFindPos);
    qint64 songNameEndPos = generalContent.indexOf("</name>", generalFindPos);
    if (songNameStartPos == -1 || songNameEndPos == -1)
    {
        LyricParseDebug("songName parse failed!");
        return false;
    }
    generalFindPos = songNameEndPos;
    this->songName = generalContent.mid(songNameStartPos + 6,
                                        songNameEndPos - songNameStartPos - 6);
    LyricParseDebug("songName is: %s", this->songName.toUtf8().data());

    qint64 singerNameStartPos = generalContent.indexOf("<singer>", generalFindPos);
    qint64 singerNameEndPos = generalContent.indexOf("</singer>", generalFindPos);
    if (singerNameStartPos == -1 || singerNameEndPos == -1)
    {
        LyricParseDebug("singerName parse failed!");
        return false;
    }
    generalFindPos = singerNameEndPos;
    this->singerName = generalContent.mid(singerNameStartPos + 8,
                                          singerNameEndPos - singerNameStartPos - 8);
    LyricParseDebug("singerName is: %s", this->singerName.toUtf8().data());
    qint64 paraCountStartPos = generalContent.indexOf("<SectionCount>", generalFindPos);
    qint64 paraCountEndPos = generalContent.indexOf("</SectionCount>", generalFindPos);
    if (paraCountStartPos == -1 || paraCountEndPos == -1)
    {
        LyricParseDebug("SectionCount parse failed!");
        return false;
    }
    generalFindPos = paraCountEndPos;
    bool isToIntOk;
    qint64 count = generalContent.mid(paraCountStartPos + 14,
                                      paraCountEndPos - paraCountStartPos - 14).toInt(&isToIntOk);
    if (! isToIntOk)
    {
        LyricParseDebug("paraCount parse failed!");
        return false;
    }
    qint64 paraCount = count;
    //part 2. parse midi_lrc
    qint64 midi_lrcStartPos = xmlDataStr.indexOf("<midi_lrc>", startFindPos);
    qint64 midi_lrcEndPos = xmlDataStr.indexOf("</midi_lrc>", startFindPos);
    if (midi_lrcStartPos == -1 || midi_lrcEndPos == -1)
    {
        LyricParseDebug("midi_lrc parse failed!");
        return false;
    }
    startFindPos = midi_lrcEndPos;
    //    LyricParseDebug("start parse midi_lrc");
    QString midi_lrcContent = xmlDataStr.mid(midi_lrcStartPos + 10,
                                             midi_lrcEndPos - midi_lrcStartPos - 10);
    this->sections->clear();
    qint64 midi_lrcFindPos = 0;
    while (midi_lrcFindPos < midi_lrcEndPos)
    {
        qint64 paragraphStartPos = midi_lrcContent.indexOf("<section", midi_lrcFindPos);
        qint64 paragraphEndPos = midi_lrcContent.indexOf("></section>", midi_lrcFindPos);
        if (paragraphStartPos == -1)
        {
            LyricParseDebug("midi_lrc parse finished!");
            break;
        }
        if (paragraphEndPos == -1)
        {
            LyricParseDebug("<section and ></section> mismatching");
            return false;
        }
        QString paragraphContent = midi_lrcContent.mid(paragraphStartPos + 8,
                                                       paragraphEndPos - paragraphStartPos - 8);
        qint64 paragraphFindPos = 0;


        LyricSection section;
        //parse begin, end
        qint64 startPos = paragraphContent.indexOf("begin=\"", paragraphFindPos);
        if (startPos == -1)
        {
            LyricParseDebug("begin parse failed!");
            return false;
        }
        qint64 endPos = paragraphContent.indexOf("\"", startPos+7);
        if (endPos == -1)
        {
            LyricParseDebug("begin parse failed!");
            return false;
        }
        paragraphFindPos = endPos;
        bool isToIntOk = true;
        int index = paragraphContent.mid(startPos + 7,
                                         endPos - startPos - 7).toFloat(&isToIntOk);
        if (! isToIntOk)
        {
            LyricParseDebug("begin parse failed!");
            return false;
        }
        section.firstSentenceIndex = index;


        startPos = paragraphContent.indexOf("end=\"", paragraphFindPos);
        if (startPos == -1)
        {
            LyricParseDebug("end parse failed!");
            return false;
        }
        endPos = paragraphContent.indexOf("\"", startPos + 5);
        if (endPos == -1)
        {
            LyricParseDebug("end parse failed!");
            return false;
        }
        paragraphFindPos = endPos;
        index = paragraphContent.mid(startPos + 5,
                                     endPos - startPos - 5).toInt(&isToIntOk);
        if (! isToIntOk)
        {
            LyricParseDebug("end parse failed!");
            return false;
        }
        section.lastSentenceIndex = index;
        if (section.lastSentenceIndex <= section.firstSentenceIndex)
        {
            LyricParseDebug("begin and end parse failed!");
            return false;
        }
        this->sections->append(section);

        midi_lrcFindPos = paragraphEndPos + 11;
    }
    if (paraCount != this->sections->size())
    {
        LyricParseDebug("SectionParse failed!");
        return false;
    }
    LyricParseDebug("SectionParse finished!");
    LyricParseDebug("SectionCount is %d", getSectionCount());
    for (int i = 0; i < sections->size(); i++)
    {
        LyricParseDebug("section begin = %d, end = %d",
                        getSectionFirstSentenceIndex(i),
                        getSectionLastSentenceIndex(i));
    }

    return true;
}

void LyricParse::reset()
{
    paragraph->clear();
    sections->clear();
}

void LyricParse::setSectionCountHint(const int &count)
{
    sectionCountHint = count;
}

QString LyricParse::getSongName() const
{
    return songName;
}

QString LyricParse::getSingerName() const
{
    return singerName;
}

float LyricParse::getBeginTime() const
{    
    LyricSentence *sentence = paragraph->sentences->first();
    LyricWord *word = sentence->words->first();

    float beginTime = word->beginTime;

    LyricParseDebug("beginTime is: %f", beginTime);

    return beginTime;
}

float LyricParse::getEndTime() const
{
    LyricSentence *sentence = paragraph->sentences->last();
    LyricWord *word = sentence->words->last();

    float endTime = word->endTime;

    LyricParseDebug("endTime is: %f", endTime);

    return endTime;
}

int LyricParse::getSentenceCount() const
{
    return  paragraph->size();
}

QString LyricParse::getSentenceString(int sentenceIndex) const
{
 //   LyricParseDebug("sentenceIndex is: %d", sentenceIndex);

//    LyricParseDebug("paragraph->size() is: %d", paragraph->size());

    if (sentenceIndex >= paragraph->size() || sentenceIndex < 0)
    {
        LyricParseDebug("index out of paragraph range: %d", sentenceIndex);
        return "";
    }

    QString content;
    LyricParseDebug("paragraph->sentences->size() is: %d", paragraph->sentences->size());
    LyricSentence *sentence = paragraph->sentences->at(sentenceIndex);
    LyricParseDebug("sentence->size() is: %d", sentence->size());
    for (int i= 0; i < sentence->size(); i++)
    {
//        LyricParseDebug("");
        LyricWord *word = sentence->words->at(i);
//        LyricParseDebug("i = %d, word.content is: %s", i, word->content.toUtf8().data());
        content += word->content;
    }

    LyricParseDebug("sentence string is: %s", content.toUtf8().data());

    return content;
}

float LyricParse::getSentenceBeginTime(int sentenceIndex) const
{
 //   LyricParseDebug("sentenceIndex is: %d", sentenceIndex);

    if(sentenceIndex >= paragraph->size() || sentenceIndex < 0)
    {
        LyricParseDebug("index out of paragraph range:%d",sentenceIndex);
        return 0;
    }

    LyricSentence *sentence = paragraph->sentences->at(sentenceIndex);
    LyricWord *word = sentence->words->first();
    float sentenceBeginTime = word->beginTime;

//    LyricParseDebug("sentenceBeginTime is: %f", sentenceBeginTime);

    return sentenceBeginTime;
}

float LyricParse::getSentenceEndTime(int sentenceIndex) const
{
//    LyricParseDebug("sentenceIndex is: %d", sentenceIndex);

    if(sentenceIndex >= paragraph->size() || sentenceIndex < 0)
    {
        LyricParseDebug("index out of paragraph range:%d",sentenceIndex);
        return 0;
    }

    LyricSentence *sentence = paragraph->sentences->at(sentenceIndex);
    LyricWord *word = sentence->words->last();
    float sentenceEndTime = word->endTime;

 //   LyricParseDebug("sentenceEndTime is: %f", sentenceEndTime);

    return sentenceEndTime;
}

int LyricParse::getSentenceIndexWithTime(float time) const
{
    if(time < getBeginTime())
    {
        LyricParseDebug("SentenceIndexWithTime SENTENCE_BEFORE!");
        return SENTENCE_BEFORE;
    }

    if(time > getEndTime())
    {
        LyricParseDebug("SentenceIndexWithTime SENTENCE_END!");
        return SENTENCE_END;
    }

    for(int i = 0; i < paragraph->size(); i++)
    {
        float beginTime = getSentenceBeginTime(i);
        float endTime = getSentenceEndTime(i);

        if (time >= beginTime && time <= endTime)
        {
            return i;
        }
    }
    LyricParseDebug("SentenceIndexWithTime SENTENCE_BETWEEN!");
    return SENTENCE_BETWEEN;
}

int LyricParse::getWordCount(int sentenceIndex) const
{
    LyricSentence *sentence = paragraph->sentences->at(sentenceIndex);
    int wordCount = sentence->size();

    return wordCount;
}

QString LyricParse::getWordString(int sentenceIndex, int wordIndex) const
{
    LyricSentence *sentence = paragraph->sentences->at(sentenceIndex);
    LyricWord *word = sentence->words->at(wordIndex);
    QString strWord = word->content;

    return strWord;
}

float LyricParse::getWordBeginTime(int sentenceIndex, int wordIndex) const
{
    LyricSentence *sentence = paragraph->sentences->at(sentenceIndex);
    LyricWord *word = sentence->words->at(wordIndex);

    float wordBeginTime = word->beginTime;

    return wordBeginTime;
}

float LyricParse::getWordEndTime(int sentenceIndex, int wordIndex) const
{
    LyricSentence *sentence = paragraph->sentences->at(sentenceIndex);
    LyricWord *word = sentence->words->at(wordIndex);

    float wordEndTime = word->endTime;

    return wordEndTime;
}

int LyricParse::getSectionCount() const
{
    return sections->size();
}

int LyricParse::getSectionFirstSentenceIndex(int sectionIndex) const
{
    if(sectionIndex < 0 || sectionIndex >= sections->size())
    {
        LyricParseDebug("sectionIndex out of paragraph divide info vector range: %d", sectionIndex);
        return -1;
    }

    return sections->at(sectionIndex).firstSentenceIndex;
}

int LyricParse::getSectionLastSentenceIndex(int sectionIndex) const
{
    if(sectionIndex < 0 || sectionIndex >= sections->size())
    {
        LyricParseDebug("sectionIndex out of paragraph divide info vector range: %d", sectionIndex);
        return -1;
    }

    return sections->at(sectionIndex).lastSentenceIndex;
}

float LyricParse::getSectionBeginTime(int sectionIndex) const
{
//    LyricParseDebug("sectionIndex is: %d", sectionIndex);

    if (sectionIndex < 0 || sectionIndex >= sections->size())
    {
        LyricParseDebug("sectionIndex out of paragraph divide info vector range:%d", sectionIndex);
        return -1;
    }

    if (sectionIndex == 0)
    {
        return 0.0f;
    }

    //    int sentenceIndex = getSectionLastSentenceIndex(sectionIndex - 1);
    //    float sectionBeginTime = getSentenceEndTime(sentenceIndex);

    int lastSentenceIndex = getSectionLastSentenceIndex(sectionIndex - 1);
    float lastSentenceEndTime = getSentenceEndTime(lastSentenceIndex);

    int firstSentenceIndex = getSectionFirstSentenceIndex(sectionIndex);
    float firstSentenceBeginTime = getSentenceBeginTime(firstSentenceIndex);

    float sectionBeginTime = (lastSentenceEndTime + firstSentenceBeginTime) / 2;

//    LyricParseDebug("sectionBeginTime is: %f", sectionBeginTime);

    return sectionBeginTime;
}

float LyricParse::getSectionEndTime(int sectionIndex) const
{
//    LyricParseDebug("");

    if(sectionIndex < 0 || sectionIndex >= sections->size())
    {
        LyricParseDebug("sectionIndex out of paragraph divide info vector range:%d", sectionIndex);
        return -1;
    }

    //    int sentenceIndex = getSectionLastSentenceIndex(sectionIndex);
    //    float sectionEndTime = getSentenceEndTime(sentenceIndex);


    float sectionEndTime;

    // last sectionIndex
    if (sectionIndex == sections->size() - 1)
    {
        int sentenceIndex = getSectionLastSentenceIndex(sectionIndex);
        sectionEndTime = getSentenceEndTime(sentenceIndex);   // add 2s music in the end
    }
    else
    {
        sectionEndTime = getSectionBeginTime(sectionIndex + 1);
    }

//    LyricParseDebug("sectionEndTime is: %f", sectionEndTime);

    return sectionEndTime;
}


