#include "SingChangeErrorWidget.h"
#include "ui_SingChangeErrorWidget.h"
#include <QPainter>

SingChangeErrorWidget::SingChangeErrorWidget(QString errorText, QWidget *parent) :
    QWidget(parent),
    m_sErrorText(errorText),
    ui(new Ui::SingChangeErrorWidget)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool/*|Qt::WindowStaysOnTopHint*/);
//    setAttribute(Qt::WA_ShowModal, true);
    info = PkInfoManager::getInstance();
    startTimer(3000);
    loopMapIndex = 1;
    loopTimer = NULL;
}

SingChangeErrorWidget::~SingChangeErrorWidget()
{
    delete ui;
    delete pix;
    if(loopTimer != NULL)
    {
        loopTimer->deleteLater();
    }
}

void SingChangeErrorWidget::SetWaitLoopLabel(bool isDownloding)
{
    if(isDownloding)
    {
        SingChangeErrorWidgetDebug("set wait loop");
        ui->hideBtn->hide();
        ui->errorLb->hide();
        ui->errorLb2->setText(m_sErrorText);
        pix = new QPixmap("://image/SingChangeErrorWidget/bg_loading.png");

        loopTimer = new QTimer;
        connect(loopTimer, SIGNAL(timeout()), this, SLOT(DealWaitLoopLabel()));
        loopTimer->start(66);
    }
    else
    {
        SingChangeErrorWidgetDebug("error widget");
        ui->errorLb2->hide();
        pix = new QPixmap("://image/SingChangeErrorWidget/bg_error.png");
        ui->errorLb->setText(m_sErrorText);
    }


}

void SingChangeErrorWidget::DealWaitLoopLabel()
{
    SingChangeErrorWidgetDebug("deal wait loop %d", loopMapIndex);
    if (30 == loopMapIndex)
    {
        loopMapIndex = 1;
    }
    QPixmap pix(QString(":/image/SongSelectUI/juhuazhuan/jz (%1)").arg(loopMapIndex));
    ui->waitlooplabel->setPixmap(pix);
    loopMapIndex ++;
}


void SingChangeErrorWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, *pix);//绘制图像
    QWidget::paintEvent(e);
}

void SingChangeErrorWidget::timerEvent(QTimerEvent *e)
{
    emit onTimeOut(info->getCurrentPkPlayerIndex());
}

void SingChangeErrorWidget::on_hideBtn_clicked()
{
    this->hide();
}
