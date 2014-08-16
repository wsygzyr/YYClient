#ifndef MAINPAGEINFO_H
#define MAINPAGEINFO_H

#include "DataType/Player/Player.h"
#include "DataType/Prop/Prop.h"

//daily income info of host
class DailyPaperIncome
{
public:
    DailyPaperIncome();
    Player getHost() const;
    void setHost(const Player &value);

    int getPkTimes() const;
    void setPkTimes(int value);

    float getIncome() const;
    void setIncome(float value);

private:
    Player host;
    int    pkTimes;
    float  income;
};

//daily heat info of host
class DailyPaperHeat
{
public:
    DailyPaperHeat();
    Player getHost() const;
    void setHost(const Player &value);

    int getHeat() const;
    void setHeat(int value);

private:
    Player host;
    int heat;
};

//daily prop used info of host
class DailyPaperProp
{
public:
    DailyPaperProp();
    Player getHost() const;
    void setHost(const Player &value);

    Prop getProp() const;
    void setProp(const Prop &value);

private:
    Player host;
    Prop prop;
};


//the info struct of host home page
class MainPageInfo
{
public:
    MainPageInfo();
    int getThisWeekHeat() const;
    void setThisWeekHeat(int value);

    float getThisMonthIncome() const;
    void setThisMonthIncome(float value);

//    QList<DailyPaperIncome> getDailyPaperIncomeList() const;
//    void setDailyPaperIncomeList(const QList<DailyPaperIncome> &value);

    QList<DailyPaperHeat> getDailyPaperHeatList() const;
    void setDailyPaperHeatList(const QList<DailyPaperHeat> &value);

    QList<DailyPaperProp> getDailyPaperPropList() const;
    void setDailyPaperPropList(const QList<DailyPaperProp> &value);

private:
    int thisWeekHeat;
    float thisMonthIncome;
//    QList<DailyPaperIncome> dailyPaperIncomeList;
    QList<DailyPaperHeat> dailyPaperHeatList;
    QList<DailyPaperProp> dailyPaperPropList;
};

#endif // MAINPAGEINFO_H
