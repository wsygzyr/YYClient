#include "Player.h"
#include "Modules/LyricParse/LyricParse.h"
#include "QFontMetrics"
Player::Player()
{
    this->name          = "";
    this->isHost        = false;
    this->sectionIndex     = -1;
//    propList.clear();
}

//QList<Prop> Player::getPropList() const
//{
//    return propList;
//}

//void Player::setPropList(const QList<Prop> &value)
//{
//    propList = value;
//}


//void Player::updatePropList(const Prop usedProp)
//{
//    for(int i = 0; i < propList.size() ;i++)
//    {
//        if(usedProp.getName() == propList[i].getName())
//        {
//            int remain = propList[i].getRemainNumber();
//            int used   = usedProp.getUseNumber();
//            remain -= used;
//            if(remain <= 0)
//                remain = 0;
//            propList[i].setRemainNumber(remain);
//            propList[i].setUseNumber(used);
//        }
//    }
//}

int Player::getSectionIndex() const
{
    return sectionIndex;
}

void Player::setSectionIndex(int value)
{
    sectionIndex = value;
}

QString Player::getShortCutName(QFont font, int &width) const
{
    QFontMetrics metrics(font);
    int lastWidth = 0;
    int currentWidth = 0;
    int index = 0;
    for(int i  = 0 ; i < this->name.size() ; i++)
    {
        if(metrics.width(this->name.left(i)) >= width)
        {
            index = i;
            currentWidth = metrics.width(this->name.left(i));
            break;
        }
        else
        {
            lastWidth = metrics.width(this->name.left(i));
        }
    }
    if(index == 0)
    {
        width = metrics.width(this->name);
        return this->name;
    }
    else if(width == currentWidth)
    {
        return this->name.left(index);
    }
    else
    {
        width = lastWidth;
        return this->name.left(index - 1);
    }
}

Score Player::getSumScore() const
{
    return sumScore;
}

void Player::setSumScore(const Score &value)
{
    sumScore = value;
}

Prop Player::getProp() const
{
    return propInfo;
}

void Player::setProp(const Prop &value)
{
    propInfo = value;
}

QString Player::getPicFileName() const
{
    return picFileName;
}

void Player::setPicFileName(const QString &value)
{
    picFileName = value;
}

int Player::getID() const
{
    return ID;
}

void Player::setID(const int &value)
{
    ID = value;
}


VictoryReward Player::getVictoryDemand() const
{
    return victoryDemand;
}

void Player::setVictoryDemand(const VictoryReward &value)
{
    victoryDemand = value;
}


QString Player::getPicFileSvPath() const
{
    return picFileSvPath;
}

void Player::setPicFileSvPath(const QString &value)
{
    picFileSvPath = value;
}

QString Player::getName() const
{
    return this->name;
}

bool Player::getIsHost() const
{
    return this->isHost;
}

//QUrl Player::getHeadPicUrl() const
//{
//    return this->headPicUrl;
//}


void Player::setName(const QString &name)
{
    this->name = name;
}

void Player::setIsHost(bool isHost)
{
    this->isHost = isHost;
}

//void Player::setHeadPicUrl(const QUrl &headPicUrl)
//{
//    this->headPicUrl = headPicUrl;
//}




