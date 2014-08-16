#include "BidResultFailedWidget.h"
#include "ui_BidResultFailedWidget.h"

BidResultFailedWidget::BidResultFailedWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BidResultFailedWidget)
{
    ui->setupUi(this);
    connect(ui->QuitBtn, SIGNAL(clicked()), this, SIGNAL(onQuit()));
}

BidResultFailedWidget::~BidResultFailedWidget()
{
    delete ui;
}
