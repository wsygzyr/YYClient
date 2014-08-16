#ifndef SINGER_H
#define SINGER_H
#include <QString>
class Singer
{
public:
    Singer(QString singNum, QString singerName, char firstLetter, int type)
    {
        this->singNum = singNum;
        this->singerName = singerName;
        this->firstLetter = firstLetter;
        this->type = type;
    }
    Singer();


    QString getSingNum() const;
    void setSingNum(const QString &value);
    QString getSingerName() const;
    void setSingerName(const QString &value);
    char getFirstLetter() const;
    void setFirstLetter(char value);
    int getType() const;
    void setType(int value);

private:
    QString         singNum;
    QString         singerName;
    char            firstLetter;
    int             type;
};

#endif // SINGER_H
