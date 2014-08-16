#include "MainPageInfo.h"

MainPageInfo::MainPageInfo()
{
}
int MainPageInfo::getThisWeekHeat() const
{
    return thisWeekHeat;
}

void MainPageInfo::setThisWeekHeat(int value)
{
    thisWeekHeat = value;
}
float MainPageInfo::getThisMonthIncome() const
{
    return thisMonthIncome;
}

void MainPageInfo::setThisMonthIncome(float value)
{
    thisMonthIncome = value;
}

/*
QList<DailyPaperIncome> MainPageInfo::getDailyPaperIncomeList() const
{
    return dailyPaperIncomeList;
}

void MainPageInfo::setDailyPaperIncomeList(const QList<DailyPaperIncome> &value)
{
    dailyPaperIncomeList.clear();
    for(int i = 0; i < value.size(); i++)
    {
        dailyPaperIncomeList.append(value[i]);
    }
}
*/

QList<DailyPaperHeat> MainPageInfo::getDailyPaperHeatList() const
{
    return dailyPaperHeatList;
}

void MainPageInfo::setDailyPaperHeatList(const QList<DailyPaperHeat> &value)
{
    dailyPaperHeatList.clear();
    for(int i = 0; i < value.size(); i++)
    {
        dailyPaperHeatList.append(value[i]);
    }
}


QList<DailyPaperProp> MainPageInfo::getDailyPaperPropList() const
{
    return dailyPaperPropList;
}

void MainPageInfo::setDailyPaperPropList(const QList<DailyPaperProp> &value)
{
    dailyPaperPropList.clear();
    for(int i = 0; i < value.size(); i++)
    {
        dailyPaperPropList.append(value[i]);
    }
}






DailyPaperIncome::DailyPaperIncome()
{

}

Player DailyPaperIncome::getHost() const
{
    return host;
}

void DailyPaperIncome::setHost(const Player &value)
{
    host = value;
}
int DailyPaperIncome::getPkTimes() const
{
    return pkTimes;
}

void DailyPaperIncome::setPkTimes(int value)
{
    pkTimes = value;
}
float DailyPaperIncome::getIncome() const
{
    return income;
}

void DailyPaperIncome::setIncome(float value)
{
    income = value;
}






DailyPaperHeat::DailyPaperHeat()
{

}

Player DailyPaperHeat::getHost() const
{
    return host;
}

void DailyPaperHeat::setHost(const Player &value)
{
    host = value;
}
int DailyPaperHeat::getHeat() const
{
    return heat;
}

void DailyPaperHeat::setHeat(int value)
{
    heat = value;
}


DailyPaperProp::DailyPaperProp()
{

}

Player DailyPaperProp::getHost() const
{
    return host;
}

void DailyPaperProp::setHost(const Player &value)
{
    host = value;
}
Prop DailyPaperProp::getProp() const
{
    return prop;
}

void DailyPaperProp::setProp(const Prop &value)
{
    prop = value;
}




