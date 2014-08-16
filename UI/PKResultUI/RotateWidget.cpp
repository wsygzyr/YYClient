#include "RotateWidget.h"
#include <QPainter>

RotateWidget::RotateWidget(QString &fileName , QWidget *parent ) :
    QWidget(parent)
{
    pix = new QPixmap();
    pix->load(fileName);
    this->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明
    this->setGeometry(this->x() , this->y() , pix->width() , pix->height());
    timerId = -1;
    angle = 0;
    timerSpeed = 30;
    angleSpeed = 1;
}

RotateWidget::~RotateWidget()
{
    delete pix;
}

void RotateWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.translate(pix->width() / 2 , pix->height() / 2);                //使图片的中心作为旋转的中心
    painter.rotate(angle);                //顺时针旋转90°
    painter.translate(-pix->width() / 2 , -pix->height() / 2);        //将原点复位
    painter.drawPixmap(0,0,pix->width(),pix->height(),*pix);

    QWidget::paintEvent(e);
}

void RotateWidget::timerEvent(QTimerEvent *)
{
    angle += angleSpeed;
    repaint();
}

void RotateWidget::startRotate()
{
    timerId = this->startTimer(timerSpeed);
}

void RotateWidget::stopRotate()
{
    if(timerId != -1)
    {
        killTimer(timerId);
        timerId = -1;
    }
    angle = 0;
}

void RotateWidget::setTimerSpeed(int value)
{
    timerSpeed = value;
}

void RotateWidget::setAngleSpeed(int value)
{
    angleSpeed = value;
}



