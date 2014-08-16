#include "Prog.h"

Prog::Prog()
{
}

QString Prog::getColumnNum() const
{
    return columnNum;
}

void Prog::setColumnNum(const QString &value)
{
    columnNum = value;
}
QString Prog::getColumnName() const
{
    return columnName;
}

void Prog::setColumnName(const QString &value)
{
    columnName = value;
}
int Prog::getButtonOrder() const
{
    return buttonOrder;
}

void Prog::setButtonOrder(int value)
{
    buttonOrder = value;
}


