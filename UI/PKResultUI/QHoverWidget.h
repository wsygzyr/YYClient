#ifndef QHOVERWIDGET_H
#define QHOVERWIDGET_H

#include <QWidget>
#include <QLabel>
#include "Modules/PkInfoManager/PkInfoManager.h"
#define DEBUG_HOVER_WIDGET  1

#if DEBUG_HOVER_WIDGET
#include <QDebug>
#define QHoverWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define QHoverWidgetDebug(format,...)
#endif
class QHoverWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QHoverWidget(int row,int column, QWidget *parent = 0);

signals:

public:
    void enterEvent( QEvent * event);
    void leaveEvent(QEvent *);
private:
    int row;
    int column;
    QLabel *label;
    PkInfoManager *info;
};

#endif // QHOVERWIDGET_H
