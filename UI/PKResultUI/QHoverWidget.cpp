#include "QHoverWidget.h"

QHoverWidget::QHoverWidget(int row, int column, QWidget *parent) :
    QWidget(parent)
{
    this->row = row;
    this->column = column;
    label = new QLabel(this);
    label->setGeometry(0,0,48,48);
    info = PkInfoManager::getInstance();


    if(row*3+column+1 > info->getVictoryRewardList().size())
    {
        this->setStyleSheet(QString("QLabel{border-image: url(://image/PKResultUI/img_jqqd.png);}")
                             .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
        QHoverWidgetDebug("load image for undefine pic ");
    }
    else
    {
        QHoverWidgetDebug("vic desp is %s" ,info->getVictoryRewardList()[row*3+column].getDesp().toUtf8().data() );
        this->setStyleSheet(QString("QLabel{border-image: url(://image/PKResultUI/img_reword0%1.png);}")
                             .arg(info->getVictoryRewardList()[row*3+column].getID())
                             .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
//        this->setToolTip(info->getVictoryRewardList()[row*3+column].getDesp());
        this->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">%1</span></p></body></html>").arg(info->getVictoryRewardList()[row*3+column].getDesp()));

    }
}

void QHoverWidget::enterEvent(QEvent *event)
{
    QHoverWidgetDebug("enterEvent");
    if(row*3+column+1 > info->getVictoryRewardList().size())
    {
        this->setStyleSheet(QString("QLabel{border-image: url(://image/PKResultUI/img_jqqd.png);}")
                             .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
    }
    else
    {
        this->setStyleSheet(QString("QLabel{border-image: url(://image/PKResultUI/img_reword0%1_hover.png);}")
                             .arg(info->getVictoryRewardList()[row*3+column].getID())
                             .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
        QHoverWidgetDebug("hover stylesheet is %s",this->styleSheet().toUtf8().data());
    }
}

void QHoverWidget::leaveEvent(QEvent *)
{
    QHoverWidgetDebug("leaveEvent");
    if(row*3+column+1 > info->getVictoryRewardList().size())
    {
        this->setStyleSheet(QString("QLabel{border-image: url(://image/PKResultUI/img_jqqd.png);}")
                             .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
    }
    else
    {
        this->setStyleSheet(QString("QLabel{border-image: url(://image/PKResultUI/img_reword0%1.png);}")
                             .arg(info->getVictoryRewardList()[row*3+column].getID())
                             .append("QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"));
    }
}
