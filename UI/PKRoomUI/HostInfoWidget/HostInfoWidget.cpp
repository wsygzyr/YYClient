#include "HostInfoWidget.h"
#include "ui_HostInfoWidget.h"

HostInfoWidget::HostInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HostInfoWidget)
{
    ui->setupUi(this);
    this->setStyleSheet("QToolTip {"
                        "border-width: 1px;"
                        "border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;"
                        "color:white;"
                        "}");
}

HostInfoWidget::~HostInfoWidget()
{
    delete ui;
}

void HostInfoWidget::updateHostInfo(Player host)
{
    int hostNameLen = 65;
    ui->HostNameLabel->setText(host.getShortCutName(ui->HostNameLabel->font(), hostNameLen));
}

void HostInfoWidget::updateHostHeat(int heat)
{
    ui->HostHotLabel->setText(QString("%1").arg(heat));
}

void HostInfoWidget::reset()
{
   ui->HostHotLabel->setText("0");
}
