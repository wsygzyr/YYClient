#ifndef PROP_H
#define PROP_H

#include <QtCore>

#define  PROP_LIST_SIZE  4

#define PROP_ID_PKBEAN                  7000
#define PROP_ID_LUCKYWING               7001
#define PROP_ID_TRANSFORMER             7010
#define PROP_ID_TOMCAT                  7011
#define PROP_ID_PIG                     7020
#define PROP_ID_KISS                    7021
#define PROP_ID_SCRAWL                  7030
#define PROP_ID_FROG                    7031

class Prop
{
public:
    Prop();
    Prop(const QString &, int);
private:
    int         ID;
    QString     name;
    QString     priceDesp;
    QString     funcDesp;
    int         remainNumber;
    int         useNumber;
    bool        isActive;
    int         countdownTime;  // in millisecond
    bool        isComplexAni;
    int         aniLoopCount;
public:
    QString getName() const;
    void setName(const QString &);

    QString getPicFileSavePath() const;

    int getRemainNumber() const;
    void setRemainNumber(int value);

    int getUseNumber() const;
    void setUseNumber(int value);
    void addUseNumber();

    int getID() const;
    void setID(int value);

    bool getIsActive() const;
    void setIsActive(bool value);

    int getCountdownTime() const;
    void setCountdownTime(int value);
    bool getIsComplexAni() const;
    void setIsComplexAni(bool value);
    int getAniLoopCount() const;
    void setAniLoopCount(int value);
    QString getFuncDesp() const;
    void setFuncDesp(const QString &value);
    QString getPriceDesp() const;
    void setPriceDesp(const QString &value);
};

#endif // PROP_H
