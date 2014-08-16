#ifndef VICTORYREWARD_H
#define VICTORYREWARD_H

#include <QtCore>

class VictoryReward
{
public:
    VictoryReward();

private:
    int             ID;
    QString         name;
    QString         desp;
    QUrl            picUrl;

public:
    void setName(const QString &);
    QString getName() const;

    void setPicUrl(const QUrl &);
    QUrl getPicUrl() const;

    int getID() const;
    void setID(int value);

    QString getDesp() const;
    void setDesp(const QString &value);
};

#endif // VICTORYREWARD_H
