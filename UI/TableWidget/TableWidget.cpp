#include "TableWidget.h"

TableWidget::TableWidget(QWidget *parent) :
    QWidget(parent)
{
    point = QPoint(0,0);
    leftMargin = 0;
    topMargin = 0;
    column = 1;
    row = 1;
    curColumn = 1;
    curRow = 1;
}

QPoint TableWidget::getWidgetSize()
{
    point.setX((this->width() - ((column + 1)*leftMargin)) / column);
    point.setY((this->height() - ((row + 1)*topMargin)) / row);
    return point;
}

void TableWidget::addWidget(QWidget *widget)
{
    if(curColumn > column)
    {
        curRow++;
        curColumn = 1;
    }
    int x = curColumn * leftMargin + (curColumn - 1) * point.x();
    int y = curRow * topMargin + (curRow - 1) * point.y();
    TableWidgetDebug("(x,y):%d,%d",x,y);
    widget->move(x,y);
    curColumn++;
    widgetList.append(widget);
}

int TableWidget::getColumn() const
{
    return column;
}

void TableWidget::setColumn(int value)
{
    column = value;
}
int TableWidget::getRow() const
{
    return row;
}

void TableWidget::setRow(int value)
{
    row = value;
}
int TableWidget::getTopMargin() const
{
    return topMargin;
}

void TableWidget::setTopMargin(int value)
{
    topMargin = value;
}
int TableWidget::getLeftMargin() const
{
    return leftMargin;
}

void TableWidget::setLeftMargin(int value)
{
    leftMargin = value;
}
int TableWidget::getCurColumn() const
{
    return curColumn;
}

void TableWidget::setCurColumn(int value)
{
    curColumn = value;
}
int TableWidget::getCurRow() const
{
    return curRow;
}

void TableWidget::setCurRow(int value)
{
    curRow = value;
}

QWidget *TableWidget::getWidgetAt(int index)
{
    return widgetList[index];
}

QList<QWidget *> TableWidget::getWidgetList() const
{
    return widgetList;
}








