#ifndef BIDRESULTCONTROLLER_H
#define BIDRESULTCONTROLLER_H
#include "UI/BidResultWidget/BidResultWidget.h"

#include <QObject>

#define DEBUG_BID_RESULT_CONTROLLER  1

#if DEBUG_BID_RESULT_CONTROLLER
#include <QDebug>
#define BidResultControllerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define BidResultControllerDebug(format,...)
#endif

class BidResultController : public QObject
{
    Q_OBJECT
public:
    explicit BidResultController(QObject *parent = 0);
    void     setBidResultWidget(BidResultWidget*);

signals:
    void onCloseBtnClicked();
    void onMinBtnClicked();

private:
    BidResultWidget *bidResultWidget;

};

#endif // BIDRESULTCONTROLLER_H
