#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QWidget>
#include <QList>
#define DEBUG_TABLE_WIDGET  1

#if DEBUG_TABLE_WIDGET
#include <QDebug>
#define TableWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define TableWidgetDebug(format,...)
#endif

class TableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = 0);

    QPoint getWidgetSize();
    void addWidget(QWidget *widget);

    int getColumn() const;
    void setColumn(int value);

    int getRow() const;
    void setRow(int value);

    int getTopMargin() const;
    void setTopMargin(int value);

    int getLeftMargin() const;
    void setLeftMargin(int value);

    int getCurColumn() const;
    void setCurColumn(int value);

    int getCurRow() const;
    void setCurRow(int value);

    QWidget*         getWidgetAt(int index);
    QList<QWidget *> getWidgetList() const;

private:
    int column;
    int row;
    int topMargin;
    int leftMargin;
    int curColumn;
    int curRow;
    QList<QWidget *> widgetList;
    //to save the size of widget
    QPoint point;

signals:

public slots:

};

#endif // TABLEWIDGET_H
