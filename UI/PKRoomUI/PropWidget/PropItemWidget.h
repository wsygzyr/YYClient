#ifndef PROPITEMWIDGET_H
#define PROPITEMWIDGET_H

#include <QPushButton>
#include <QTime>
#include <QPainter>

#include "DataType/Prop/Prop.h"
#include "Modules/MMTimer/MMTimer.h"
#define DEBUG_PROPITEM_WIDGET  1

#if DEBUG_PROPITEM_WIDGET
#include <QDebug>
#define PropItemWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define PropItemWidgetDebug(format,...)
#endif



class PropItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PropItemWidget(QWidget *parent = 0);
    ~PropItemWidget();
    bool bPainting;
    bool bPressed;
    bool bHover;

    Prop getProp() const;
    void setProp(const Prop &value);

private:
    int angel;
    int time;
    Prop prop;

    MMTimer *maskTimer;
public slots:
    void paintEvent(QPaintEvent *event);
    void startAnimate();
    void stopAnimate();
    void setPressed(bool);
protected:
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);

private slots:
    void handleMaskTimerOnTimeOut();
};

#endif // PROPITEMWIDGET_H
