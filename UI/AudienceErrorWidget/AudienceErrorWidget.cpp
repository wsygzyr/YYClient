#include "AudienceErrorWidget.h"
#include "ui_AudienceErrorWidget.h"
#include <QPainter>
AudienceErrorWidget::AudienceErrorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudienceErrorWidget)
{
    ui->setupUi(this);
    pix = new QPixmap("://image/AudienceErrorWidget/bg_error.png");
    this->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明

    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);
    setAttribute(Qt::WA_ShowModal, true);
}

AudienceErrorWidget::~AudienceErrorWidget()
{
    delete ui;
}

void AudienceErrorWidget::on_quitBtn_clicked()
{
    P2Y_PKInfo *pKInfo = P2Y_PKInfo::getInstance();
    YYNotificationCenter *yync = YYNotificationCenter::getInstance();
    pKInfo->bAcceptPlayPK = false;
    yync->sendAudiencePKInfo(pKInfo);
    emit onQuit();
}

void AudienceErrorWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, *pix);//绘制图像

    QWidget::paintEvent(e);
}
