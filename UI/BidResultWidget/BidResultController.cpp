#include "BidResultController.h"
#include "ui_BidResultWidget.h"

#include <QTimer>

BidResultController::BidResultController(QObject *parent) :
    QObject(parent)
{
}


void BidResultController::setBidResultWidget(BidResultWidget * widget)
{
    BidResultControllerDebug("BidResultController setBidResultWidget");
    bidResultWidget = widget;

    connect(bidResultWidget->ui->closeBtn , SIGNAL(clicked()) , this , SIGNAL(onCloseBtnClicked()));
    connect(bidResultWidget->ui->minBtn , SIGNAL(clicked()) , this , SIGNAL(onMinBtnClicked()));
}   
