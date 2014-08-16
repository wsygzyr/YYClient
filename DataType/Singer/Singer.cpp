#include "Singer.h"

Singer::Singer()
{
}


QString Singer::getSingerName() const
{
    return singerName;
}

void Singer::setSingerName(const QString &value)
{
    singerName = value;
}

char Singer::getFirstLetter() const
{
    return firstLetter;
}

void Singer::setFirstLetter(char value)
{
    firstLetter = value;
}

int Singer::getType() const
{
    return this->type;
}

void Singer::setType(int value)
{
    this->type = value;
}

QString Singer::getSingNum() const
{
    return singNum;
}

void Singer::setSingNum(const QString &value)
{
    singNum = value;
}
