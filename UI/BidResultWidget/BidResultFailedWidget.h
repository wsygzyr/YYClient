#ifndef BIDRESULTFAILEDWIDGET_H
#define BIDRESULTFAILEDWIDGET_H

#include <QWidget>

#define DEBUG_BID_RESULT_FAILED_WIDGET  1

#if DEBUG_BID_RESULT_FAILED_WIDGET
#include <QDebug>
#define BidResultFailedWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define BidResultFailedWidgetDebug(format,...)
#endif

namespace Ui {
class BidResultFailedWidget;
}

class BidResultFailedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BidResultFailedWidget(QWidget *parent = 0);
    ~BidResultFailedWidget();

private:
    Ui::BidResultFailedWidget *ui;

signals:
    void onQuit();


};

#endif // BIDRESULTFAILEDWIDGET_H
