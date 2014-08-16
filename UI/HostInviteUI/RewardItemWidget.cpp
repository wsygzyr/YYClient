#include "RewardItemWidget.h"
#include "ui_RewardItemWidget.h"
#include "Modules/PkInfoManager/PkInfoManager.h"

RewardItemWidget::RewardItemWidget(int column, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RewardItemWidget)
{
    ui->setupUi(this);
    PkInfoManager *info = PkInfoManager::getInstance();
    ui->RewardDespLb->setText(info->getVictoryRewardList()[column].getDesp());
    ui->RewardPicLb->setStyleSheet(QString("border-image:url(%1);").arg(info->getVictoryRewardList()[column].getPicFileSavePath()));
}

RewardItemWidget::~RewardItemWidget()
{
    delete ui;
}
