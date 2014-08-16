#include "VictoryReward.h"
VictoryReward::VictoryReward()
{
    name = "";
    picUrl = "";
}
QString VictoryReward::getDesp() const
{
    return desp;
}

void VictoryReward::setDesp(const QString &value)
{
    desp = value;
}


int VictoryReward::getID() const
{
    return ID;
}

void VictoryReward::setID(int value)
{
    ID = value;


}

void VictoryReward::setName(const QString &name)
{
    this->name = name;
}

QString VictoryReward::getName() const
{
    return this->name;
}

void VictoryReward::setPicUrl(const QUrl &picUrl)
{
    this->picUrl = picUrl;
}

QUrl VictoryReward::getPicUrl() const
{
    return this->picUrl;
}
