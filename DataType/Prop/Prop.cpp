#include "Prop.h"
#include <QPixmap>
#include <QObject>

Prop::Prop()
{
    name = "";
    isActive = false;
    remainNumber = 0;
    useNumber = 0;
}

Prop::Prop(const QString &name, int remainNumber)
{
    this->name = name;
    this->remainNumber = remainNumber;
}
QString Prop::getPriceDesp() const
{
    return priceDesp;
}

void Prop::setPriceDesp(const QString &value)
{
    priceDesp = value;
}

QString Prop::getFuncDesp() const
{
    return funcDesp;
}

void Prop::setFuncDesp(const QString &value)
{
    funcDesp = value;
}

int Prop::getAniLoopCount() const
{
    return aniLoopCount;
}

void Prop::setAniLoopCount(int value)
{
    aniLoopCount = value;
}

bool Prop::getIsComplexAni() const
{
    return isComplexAni;
}

void Prop::setIsComplexAni(bool value)
{
    isComplexAni = value;
}

int Prop::getCountdownTime() const
{
    return countdownTime;
}

void Prop::setCountdownTime(int value)
{
    countdownTime = value;
}


bool Prop::getIsActive() const
{
    return isActive;
}

void Prop::setIsActive(bool value)
{
    isActive = value;
}

int Prop::getID() const
{
    return ID;
}

void Prop::setID(int value)
{
    ID = value;
}


int Prop::getUseNumber() const
{
    return useNumber;
}


void Prop::setUseNumber(int value)
{
    useNumber = value;
}

void Prop::addUseNumber()
{
    useNumber++;
}

void Prop::setName(const QString &name)
{
    this->name = name;
}

QString Prop::getName() const
{
    return this->name;
}




int Prop::getRemainNumber() const
{
    return remainNumber;
}

void Prop::setRemainNumber(int value)
{
    remainNumber = value;
}

QString Prop::getPicFileSavePath() const
{
  return QString("://image/PKRoomUI/%1.png").arg(ID);
}

