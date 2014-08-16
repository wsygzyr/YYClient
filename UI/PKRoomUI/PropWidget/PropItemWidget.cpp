#include "PropItemWidget.h"
#include "ui_PropItemWidget.h"
#include <QToolTip>
PropItemWidget::PropItemWidget(QWidget *parent) :
    QWidget(parent)
{
    bPainting = false;
    bPressed = false;
    time = 0;
    maskTimer = new MMTimer(this);
    maskTimer->setInterval(100);
    connect(maskTimer , SIGNAL(timeout()) , this , SLOT(handleMaskTimerOnTimeOut()));
    this->setStyleSheet("QToolTip {"
                        "border-width: 1px;"
                        "border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;"
                        "color:white;"
                        "}");
}

PropItemWidget::~PropItemWidget()
{

}

Prop PropItemWidget::getProp() const
{
    return prop;
}

void PropItemWidget::setProp(const Prop &value)
{
    PropItemWidgetDebug("PropItemWidget setProp");
    prop = value;
    prop.setUseNumber(1);
//    this->setToolTip( prop.getPriceDesp() + "\n" + prop.getFuncDesp());
    this->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">%1</span></p></body></html>").arg(prop.getPriceDesp() + "\n" + prop.getFuncDesp()));

    PropItemWidgetDebug("prop.getID() is: %d, prop.getName() is: %s, prop.getCountdownTime() is: %d",
                        prop.getID(), prop.getName().toUtf8().data(), prop.getCountdownTime());

}

void PropItemWidget::paintEvent(QPaintEvent *event)
{
   // QPushButton::paintEvent(event);
    QPainter painter(this);
    painter.setClipRect(0,0,38,38);

//    // load background picture
    QPixmap pix;
    if(!bPressed)
    {
        if (bHover)
        {
            pix.load(QString("://image/PKRoomUI/%1_hover.png").arg(prop.getID()));
        }
        else
        {
            pix.load(QString("://image/PKRoomUI/%1.png").arg(prop.getID()));
        }
    }
    else
    {
        pix.load(QString("://image/PKRoomUI/%1_pressed.png").arg(prop.getID()));
    }

    painter.drawPixmap(0,0,38,38,pix);
    painter.setRenderHint(QPainter::Antialiasing);
    if (bPainting)
    {
        painter.setBrush(QBrush(QColor(37,11,46,155)));
        painter.setPen(Qt::NoPen);
        painter.drawPie(QRectF(-8,-8,56,56),90*16,(360 - angel)*16);
    }

}


void PropItemWidget::startAnimate()
{
    PropItemWidgetDebug("PropItemWidget startAnimate");
    if (bPainting)
    {
        PropItemWidgetDebug("bPainting!");
        return;
    }

    PropItemWidgetDebug("start!");
    time = 0;
    maskTimer->start();
    bPainting = true;
}

void PropItemWidget::stopAnimate()
{
    PropItemWidgetDebug("PropItemWidget stopAnimate");
    if (!bPainting)
    {
        return;
    }
    maskTimer->stop();
    bPainting = false;
    time = 0;
    repaint();
}

void PropItemWidget::setPressed(bool bPressed)
{
    PropItemWidgetDebug("PropItemWidget setPressed");
    this->bPressed = bPressed;
    if (!bPainting)
    {
        repaint();
    }
}

void PropItemWidget::enterEvent(QEvent *e)
{
    PropItemWidgetDebug("PropItemWidget hover");
    bHover = true;
    repaint();
}

void PropItemWidget::leaveEvent(QEvent *)
{
    PropItemWidgetDebug("PropItemWidget leave");
    bHover = false;
    repaint();
}

void PropItemWidget::handleMaskTimerOnTimeOut()
{
    time = time + 100;
    //PropItemWidgetDebug("time is: %d", time);
    angel = time * 360 / (float)prop.getCountdownTime();
    if (angel > 360)
    {
        PropItemWidgetDebug("angel is: %d", angel);
        maskTimer->stop();
        bPainting = false;
        time = 0;
    }
    repaint();
}
