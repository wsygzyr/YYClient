#include "TempVarCollector.h"

TempVarCollector::TempVarCollector(QObject *parent) :
    QObject(parent)
{
}

TempVarCollector::~TempVarCollector()
{
    this->clear();
}

void TempVarCollector::addObject(QObject *obj)
{
    collector.append(obj);
}

void TempVarCollector::removeObject(QObject *obj)
{
    for(int i = 0 ; i < collector.size(); i++)
    {
        if(collector[i] == obj)
        {
            QObject *tmp = collector[i];
            collector.erase(collector.begin() + i);
            delete tmp;
            tmp = 0;
            break;
        }
    }
}

void TempVarCollector::clear()
{
    for(int i = 0 ; i < collector.size(); i++)
    {
        QObject *tmp = collector[i];
        delete tmp;
        tmp = 0;
    }
    collector.clear();
}
