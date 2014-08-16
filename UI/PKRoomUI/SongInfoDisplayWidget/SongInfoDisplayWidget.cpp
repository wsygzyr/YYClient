#include "SongInfoDisplayWidget.h"
#include <QDebug>
SongInfoDisplayWidget::SongInfoDisplayWidget(QWidget *parent) :
    QWidget(parent)
{
    offset = 0;
    timerID = 0;
    this->resize(146, 24);
    this->setStyleSheet("font: 10pt \"宋体\";color: rgb(211, 205, 228);");
}

SongInfoDisplayWidget::~SongInfoDisplayWidget()
{
}

void SongInfoDisplayWidget::setText(QString text)
{
    songInfo = text + "        ";
    update();
    updateGeometry();
}

QString SongInfoDisplayWidget::text()
{
    return songInfo;
}

void SongInfoDisplayWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int textWidth = fontMetrics().width(text());
    if (textWidth < 1)
    {
        return;
    }
    int x = -offset;
    while (x < width())
    {
        painter.drawText(x, 6, textWidth, 14, Qt::AlignLeft | Qt::AlignVCenter, text());
        x += textWidth;
    }
}

void SongInfoDisplayWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timerID)
    {
        ++offset;
        if (offset >= fontMetrics().width(text()))
        {
            offset = 0;
        }
        scroll(-1, 0);
    }
    else
    {
        QWidget::timerEvent(event);
    }
}

void SongInfoDisplayWidget::showEvent(QShowEvent * )
{
    timerID = startTimer(100);
}

void SongInfoDisplayWidget::hideEvent(QHideEvent * )
{
    killTimer(timerID);
    timerID = 0;
}
