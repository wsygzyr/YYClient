#ifndef PROG_H
#define PROG_H
#include <QString>
class Prog
{
public:
    Prog(QString columnNum, QString columnName,int buttonOrder)
    {
        this->columnNum = columnNum;
        this->columnName = columnName;
        this->buttonOrder = buttonOrder;
    }
    Prog();
    QString getColumnNum() const;
    void setColumnNum(const QString &value);

    QString getColumnName() const;
    void setColumnName(const QString &value);

    int getButtonOrder() const;
    void setButtonOrder(int value);

private:
    QString         columnNum;
    QString         columnName;
    int             buttonOrder;
};

#endif // PROG_H
