#include "ScoreWidget.h"
#include "ui_ScoreWidget.h"
#include <QTimer>
ScoreWidget::ScoreWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScoreWidget)
{
    ui->setupUi(this);
}

ScoreWidget::~ScoreWidget()
{
    delete ui;
}

void ScoreWidget::updateScore(Score score)
{
    this->hide();
    int stScore = score.getTotalScore() + (int)score.getPropScore();
    int bai = stScore / 100;
    int shi = (stScore % 100) / 10;
    int ge  = stScore % 10;

    QString baiStyle = QString("background-image: url(://image/PKRoomUI/%1.png);").arg(bai);
    QString shiStyle = QString("background-image: url(://image/PKRoomUI/%1.png);").arg(shi);
    QString geStyle = QString("background-image: url(://image/PKRoomUI/%1.png);").arg(ge);
    if(bai != 0)
    {
        ui->baiLabel->setStyleSheet(baiStyle);
    }
    else
    {
        ui->baiLabel->setStyleSheet("");
    }

    if(shi != 0)
    {
        ui->shiLabel->setStyleSheet(shiStyle);
    }
    else
    {
        ui->shiLabel->setStyleSheet("");
    }

    ui->geLabel->setStyleSheet(geStyle);

    int propScore = score.getPropScore();
    if(propScore > 0)
    {
        ui->propLabel->show();
        ui->propLabel->setText(QString("%1%2").arg(propScore > 0 ? "+" : "").arg(propScore));
    }
    else
    {
        ui->propLabel->hide();
    }

    QTimer::singleShot(100 , this ,SLOT(show()));
    QTimer::singleShot(1250 , this , SLOT(hide()));
}

void ScoreWidget::reset()
{
    ui->baiLabel->setStyleSheet("");
    ui->shiLabel->setStyleSheet("");
    ui->geLabel->setStyleSheet("");
    ui->propLabel->hide();
    this->hide();
}
