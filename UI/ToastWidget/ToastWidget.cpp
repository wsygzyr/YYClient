#include "ToastWidget.h"
#include <QFont>
#include <QFontMetrics>
ToastWidget::ToastWidget(QWidget *parent) :
    QLabel(parent)
{
    QString styleStr = "border-width: 3px;"
            "border-image: url(://image/ToastWidget/toast.png) 4 4 4 4 stretch stretch;"
            "font: 12px \"宋体\"; "
            "color:#eac5eb";
    this->setStyleSheet(styleStr);
    this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void ToastWidget::setText(const QString &text)
{
    QFont font;
    font.setPixelSize(12);
    font.setWeight(75);
    font.setBold(false);
    font.setFamily("宋体");
    QFontMetrics metrics(font);

    this->setFixedWidth(metrics.width(text) + 28);
    this->setFixedHeight(30);
    QLabel::setFont(font);
    QLabel::setText(text);
}

void ToastWidget::show()
{
    QLabel::show();
    timerId = this->startTimer(1000);
}

void ToastWidget::timerEvent(QTimerEvent *)
{
    this->killTimer(timerId);
    this->deleteLater();
}
