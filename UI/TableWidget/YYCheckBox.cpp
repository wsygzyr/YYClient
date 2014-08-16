#include "YYCheckBox.h"

YYCheckBox::YYCheckBox(QWidget *parent) :
    QCheckBox(parent)
{
    connect(this,SIGNAL(stateChanged(int)) ,this ,SLOT(onStateChanged(int)));
}
int YYCheckBox::getIndex() const
{
    return index;
}

void YYCheckBox::setIndex(int value)
{
    index = value;
}

void YYCheckBox::onStateChanged(int value)
{
    emit stateChanged(value,index);
}

