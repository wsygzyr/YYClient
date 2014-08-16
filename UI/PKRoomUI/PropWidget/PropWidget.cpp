#include "propwidget.h"
#include "ui_propwidget.h"
#include <QIcon>
#include <QPainter>
#include "UI/PKResultUI/RotateWidget.h"


PropWidget::PropWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PropWidget)
{
    ui->setupUi(this);

    cfg = ConfigHelper::getInstance();

    pkInfo = PkInfoManager::getInstance();

    yync = YYNotificationCenter::getInstance();

    nc = NodeNotificationCenter::getInstance();
    stateCtrl = StateController::getInstance();

    //QObject::connect(nc, SIGNAL(onPropActive(Player, Prop)), SLOT(handleNodeNotificationCenterOnPropActive(Player, Prop)));
    QObject::connect(stateCtrl, SIGNAL(onStatePropActive(Player, Prop)), SLOT(handleStateControllerOnStatePropActive(Player, Prop)));

    int propIndex = 2;  // skip prop0 (PKBean) and prop1 (luckyWing)
    for (int row = 0; row < PROP_ROW; row++)
    {
        ui->PropTableWidget->setRowHeight(row,40);
        for (int column = 0; column < PROP_COLUMN; column++)
        {
            ui->PropTableWidget->setColumnWidth(column, 40);

            PropItemWidget *itemWidget = new PropItemWidget();

            Prop prop = cfg->getPropList().at(propIndex);
            PropWidgetDebug("propIndex is: %d, prop.getID() is: %d", propIndex, prop.getID());
            itemWidget->setProp(prop);
            ui->PropTableWidget->setCellWidget(row, column, itemWidget);
            propIndex++;
        }
    }
    QObject::connect(ui->PropTableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(handlePropTableWidgetCellClicked(int, int)));
    QObject::connect(ui->LuckyWingBtn, SIGNAL(clicked()), this, SLOT(handleLuckyWingsClicked()));
    QObject::connect(ui->PropTableWidget ,SIGNAL(cellPressed(int,int)) ,this , SLOT(handlePropTableWidgetCellPressed(int,int)));
//    ui->LuckyWingBtn->setToolTip(cfg->getPropForID(PROP_ID_LUCKYWING).getPriceDesp() + "\n" + cfg->getPropForID(PROP_ID_LUCKYWING).getFuncDesp());
    ui->LuckyWingBtn->setToolTip(QString("<html><head/><body><p><span style=\" font-family:'宋体'; font-weight:75; font-size:12px; color:#ffffff;\">%1</span></p></body></html>").arg(cfg->getPropForID(PROP_ID_LUCKYWING).getPriceDesp() + "\n" + cfg->getPropForID(PROP_ID_LUCKYWING).getFuncDesp()));

//    luckyWingBtnMask = new RotateWidget(QString("://image/PKRoomUI/zaijia.png") ,this);
//    luckyWingBtnMask->move(ui->LuckyWingBtn->pos() - QPoint(4,4));
//    luckyWingBtnMask->hide();
//    luckyWingBtnMask->setTimerSpeed(30);
//    luckyWingBtnMask->setAngleSpeed(5);

    ButtonValid tempValid;
    tempValid.isValid = false;
    tempValid.aTimer = NULL;
    m_aPropButtonVoild.insert("PROP_ID_TRANSFORMER", tempValid);
    m_aPropButtonVoild.insert("PROP_ID_TOMCAT", tempValid);
    m_aPropButtonVoild.insert("PROP_ID_PIG", tempValid);
    m_aPropButtonVoild.insert("PROP_ID_KISS", tempValid);
    m_aPropButtonVoild.insert("PROP_ID_SCRAWL", tempValid);
    m_aPropButtonVoild.insert("PROP_ID_FROG", tempValid);
    m_aPropButtonVoild.insert("PROP_ID_LUCKYWING", tempValid);

    m_aPropButtonVoild["PROP_ID_TRANSFORMER"].aTimer = new QTimer;
    connect(m_aPropButtonVoild["PROP_ID_TRANSFORMER"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetTransformerButtonFalse()));

    m_aPropButtonVoild["PROP_ID_TOMCAT"].aTimer = new QTimer;
    connect(m_aPropButtonVoild["PROP_ID_TOMCAT"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetTomcatButtonFalse()));

    m_aPropButtonVoild["PROP_ID_PIG"].aTimer = new QTimer;
    connect(m_aPropButtonVoild["PROP_ID_PIG"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetPigButtonFalse()));

    m_aPropButtonVoild["PROP_ID_KISS"].aTimer = new QTimer;
    connect(m_aPropButtonVoild["PROP_ID_KISS"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetKissButtonFalse()));

    m_aPropButtonVoild["PROP_ID_SCRAWL"].aTimer = new QTimer;
    connect(m_aPropButtonVoild["PROP_ID_SCRAWL"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetScrawlButtonFalse()));

    m_aPropButtonVoild["PROP_ID_FROG"].aTimer = new QTimer;
    connect(m_aPropButtonVoild["PROP_ID_FROG"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetFrogButtonFalse()));

    m_aPropButtonVoild["PROP_ID_LUCKYWING"].aTimer = new QTimer;
    connect(m_aPropButtonVoild["PROP_ID_LUCKYWING"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetLuckywingsButtonFalse()));

}

PropWidget::~PropWidget()
{
    delete ui;
    m_aPropButtonVoild["PROP_ID_TRANSFORMER"].aTimer->deleteLater();
    m_aPropButtonVoild["PROP_ID_TOMCAT"].aTimer->deleteLater();
    m_aPropButtonVoild["PROP_ID_PIG"].aTimer->deleteLater();
    m_aPropButtonVoild["PROP_ID_KISS"].aTimer->deleteLater();
    m_aPropButtonVoild["PROP_ID_SCRAWL"].aTimer->deleteLater();
    m_aPropButtonVoild["PROP_ID_FROG"].aTimer->deleteLater();
    m_aPropButtonVoild["PROP_ID_LUCKYWING"].aTimer->deleteLater();
}

//void PropWidget::reset()
//{
//    PropWidgetDebug("reset...");
//    int row = ui->PropTableWidget->rowCount();
//    int column = ui->PropTableWidget->columnCount();
//    for(int i = 0; i < row; i++)
//    {
//        for(int j = 0; j < column; j++)
//        {
//            QTableWidgetItem *item = ui->PropTableWidget->takeItem(i,j);
//            if(item)
//            {
//                delete item;
//                item = NULL;
//            }
//        }
//    }
//}

void PropWidget::resetProp()
{
    PropWidgetDebug("PropWidget resetProp");
    //stop prop animation
    for (int row = 0; row < ui->PropTableWidget->rowCount(); row++)
    {
        for (int column = 0; column < ui->PropTableWidget->columnCount(); column++)
        {
            PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
            itemWidget->stopAnimate();
        }
    }

    //whether to show lucky wing
    PkInfoManager *info = PkInfoManager::getInstance();
    if(info->getCurrentPkPlayerIndex() == -1)
    {
        ui->LuckyWingBtn->hide();
        return;
    }
    if(info->getMe().getID() == info->getCurrentPkPlayer().getID())
    {
        ui->LuckyWingBtn->setStyleSheet("QPushButton:pressed{border-image:url(://image/PKRoomUI/7001_pressed.png);}"
                                        "QPushButton:hover{border-image:url(://image/PKRoomUI/7001_hover.png);}"
                                        "QPushButton{border-image: url(:/image/PKRoomUI/7001.png);}"
                                        "QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"
                                        );
        ui->LuckyWingBtn->show();
    }
    else
    {
        ui->LuckyWingBtn->hide();
       // luckyWingBtnMask->hide();
    }
    //luckyWingBtnMask->stopRotate();
    //luckyWingBtnMask->hide();

    m_aPropButtonVoild["PROP_ID_TRANSFORMER"].isValid = false;
    m_aPropButtonVoild["PROP_ID_TOMCAT"].isValid = false;
    m_aPropButtonVoild["PROP_ID_PIG"].isValid = false;
    m_aPropButtonVoild["PROP_ID_KISS"].isValid = false;
    m_aPropButtonVoild["PROP_ID_SCRAWL"].isValid = false;
    m_aPropButtonVoild["PROP_ID_FROG"].isValid = false;
    m_aPropButtonVoild["PROP_ID_LUCKYWING"].isValid = false;

    m_aPropButtonVoild["PROP_ID_TRANSFORMER"].aTimer->stop();
    m_aPropButtonVoild["PROP_ID_TOMCAT"].aTimer->stop();
    m_aPropButtonVoild["PROP_ID_PIG"].aTimer->stop();
    m_aPropButtonVoild["PROP_ID_KISS"].aTimer->stop();
    m_aPropButtonVoild["PROP_ID_SCRAWL"].aTimer->stop();
    m_aPropButtonVoild["PROP_ID_FROG"].aTimer->stop();
    m_aPropButtonVoild["PROP_ID_LUCKYWING"].aTimer->stop();
}

void PropWidget::updateLuckyWing()
{
    PropWidgetDebug("PropWidget updateLuckyWing");
    if(!ui->LuckyWingBtn->isHidden())
    {
       // luckyWingBtnMask->show();
      //  luckyWingBtnMask->startRotate();
    }
    ui->LuckyWingBtn->setStyleSheet("QPushButton{border-image: url(://image/PKRoomUI/7001_used.png);}"
                                    "QToolTip{border-width: 1px;border-image: url(://image/ToastWidget/toast.png) 2 2 2 2 stretch stretch;color:white;}"
                                    );
}

void PropWidget::updatePropItemWidget(Prop prop)
{
    for (int row = 0; row < PROP_ROW; row ++)
    {
        for (int column = 0; column < PROP_COLUMN; column ++)
        {
            PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
            Prop itemProp = itemWidget->getProp();
            if (itemProp.getID() == prop.getID())
            {
                itemWidget->setPressed(false);
                itemWidget->setEnabled(true);
                return;
            }
        }
    }
}

void PropWidget::handlePropTableWidgetCellClicked(int row, int column)
{
    PropWidgetDebug("row is %d, column is:%d", row, column);

    PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
    Prop prop = itemWidget->getProp();
    itemWidget->setPressed(true);
    itemWidget->setEnabled(false);
    switch (prop.getID())
    {
    case PROP_ID_TRANSFORMER:
    {

        // if one of SpeechChange Prop has been actived by any user or player, we shall return
        if (pkInfo->getChangeSpeechProp().getIsActive())
        {
            return;
        }

        if(m_aPropButtonVoild["PROP_ID_TRANSFORMER"].isValid == false)
        {
            m_aPropButtonVoild["PROP_ID_TRANSFORMER"].isValid = true;
//            m_aPropButtonVoild["PROP_ID_TRANSFORMER"].aTimer = new QTimer;
//            connect(m_aPropButtonVoild["PROP_ID_TRANSFORMER"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetTomcatButtonFalse()));
            m_aPropButtonVoild["PROP_ID_TRANSFORMER"].aTimer->start(12000);
            nc->sendProp(prop);
        }


    }
        break;
    case PROP_ID_TOMCAT:
    {
        // if one of SpeechChange Prop has been actived by any user or player, we shall return
        if (pkInfo->getChangeSpeechProp().getIsActive())
        {
            return;
        }

        if(m_aPropButtonVoild["PROP_ID_TOMCAT"].isValid == false)
        {
            m_aPropButtonVoild["PROP_ID_TOMCAT"].isValid = true;
 //           m_aPropButtonVoild["PROP_ID_TOMCAT"].aTimer = new QTimer;
 //           connect(m_aPropButtonVoild["PROP_ID_TOMCAT"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetTomcatButtonFalse()));
            m_aPropButtonVoild["PROP_ID_TOMCAT"].aTimer->start(12000);
            nc->sendProp(prop);
        }

    }
        break;
    case PROP_ID_PIG:
    {
        if(m_aPropButtonVoild["PROP_ID_PIG"].isValid == false)
        {
            m_aPropButtonVoild["PROP_ID_PIG"].isValid = true;
//            m_aPropButtonVoild["PROP_ID_PIG"].aTimer = new QTimer;
//            connect(m_aPropButtonVoild["PROP_ID_PIG"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetTomcatButtonFalse()));
            m_aPropButtonVoild["PROP_ID_PIG"].aTimer->start(12000);
            yync->sendUsePropRequest(prop);
        }
    }
        break;
    case PROP_ID_KISS:
    {
        if(m_aPropButtonVoild["PROP_ID_KISS"].isValid == false)
        {
            m_aPropButtonVoild["PROP_ID_KISS"].isValid = true;
 //           m_aPropButtonVoild["PROP_ID_KISS"].aTimer = new QTimer;
 //           connect(m_aPropButtonVoild["PROP_ID_KISS"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetTomcatButtonFalse()));
            m_aPropButtonVoild["PROP_ID_KISS"].aTimer->start(12000);
            yync->sendUsePropRequest(prop);
        }
    }
        break;
    case PROP_ID_SCRAWL:
    {
        if(m_aPropButtonVoild["PROP_ID_SCRAWL"].isValid == false)
        {
            m_aPropButtonVoild["PROP_ID_SCRAWL"].isValid = true;
 //           m_aPropButtonVoild["PROP_ID_SCRAWL"].aTimer = new QTimer;
 //           connect(m_aPropButtonVoild["PROP_ID_SCRAWL"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetTomcatButtonFalse()));
            m_aPropButtonVoild["PROP_ID_SCRAWL"].aTimer->start(12000);
            yync->sendUsePropRequest(prop);
        }
    }
        break;
    case PROP_ID_FROG:
    {
        if(m_aPropButtonVoild["PROP_ID_FROG"].isValid == false)
        {
            m_aPropButtonVoild["PROP_ID_FROG"].isValid = true;
//            m_aPropButtonVoild["PROP_ID_FROG"].aTimer = new QTimer;
//            connect(m_aPropButtonVoild["PROP_ID_FROG"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetTomcatButtonFalse()));
            m_aPropButtonVoild["PROP_ID_FROG"].aTimer->start(12000);
            yync->sendUsePropRequest(prop);
        }
    }
        break;

    default:
        break;
    }

}

void PropWidget::handlePropTableWidgetCellPressed(int row, int column)
{
    PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
//    itemWidget->setPressed(true);
}


void PropWidget::handleLuckyWingsClicked()
{
    PropWidgetDebug("PropWidget handleLuckyWingsClicked");
    if (pkInfo->getMe().getID() != pkInfo->getCurrentPkPlayer().getID())
    {
        return;
    }

    if (pkInfo->getLuckyWingProp().getIsActive())
    {
        return;
    }

    if(m_aPropButtonVoild["PROP_ID_LUCKYWING"].isValid == false)
    {
        Prop prop;
        prop.setID(PROP_ID_LUCKYWING);
        prop.setUseNumber(1);
        m_aPropButtonVoild["PROP_ID_LUCKYWING"].isValid = true;
    //    m_aPropButtonVoild["PROP_ID_LUCKYWING"].aTimer = new QTimer;
    //    connect(m_aPropButtonVoild["PROP_ID_LUCKYWING"].aTimer, SIGNAL(timeout()), this, SLOT(handleSetTomcatButtonFalse()));
        m_aPropButtonVoild["PROP_ID_LUCKYWING"].aTimer->start(12000);
        yync->sendUsePropRequest(prop);
    }


}

void PropWidget::handleStateControllerOnStatePropActive(Player player, Prop prop)
{
    PropWidgetDebug("player.getID() is: %d, prop.getID() is: %d, prop.getName() is: %s",
                    player.getID(), prop.getID(), prop.getName().toUtf8().data());

    if (prop.getID() != PROP_ID_TRANSFORMER && prop.getID() != PROP_ID_TOMCAT)
    {
        return;
    }

    for (int row = 0; row < ui->PropTableWidget->rowCount(); row++)
    {
        for (int column = 0; column < ui->PropTableWidget->columnCount(); column++)
        {
            PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
            Prop itemProp = itemWidget->getProp();
            if (itemProp.getID() == PROP_ID_TRANSFORMER || itemProp.getID() == PROP_ID_TOMCAT)
            {
                PropWidgetDebug("prop matched! prop.getID() is: %d, prop.getName() is: %s",
                                prop.getID(), prop.getName().toUtf8().data());
                itemWidget->startAnimate();
            }
        }
    }
}

void PropWidget::handleSetTransformerButtonFalse()
{
    PropWidgetDebug("Transformer clicke timeoute");
    m_aPropButtonVoild["PROP_ID_TRANSFORMER"].isValid = false;
    m_aPropButtonVoild["PROP_ID_TRANSFORMER"].aTimer->stop();

    for (int row = 0; row < PROP_ROW; row ++)
    {
        for (int column = 0; column < PROP_COLUMN; column ++)
        {
            PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
            Prop itemProp = itemWidget->getProp();
            if (itemProp.getID() == PROP_ID_TRANSFORMER)
            {
                itemWidget->setPressed(false);
                itemWidget->setEnabled(true);
                return;
            }
        }
    }
}

void PropWidget::handleSetTomcatButtonFalse()
{
    PropWidgetDebug("Tomcat clicke timeoute");
    m_aPropButtonVoild["PROP_ID_TOMCAT"].isValid = false;
    m_aPropButtonVoild["PROP_ID_TOMCAT"].aTimer->stop();

    for (int row = 0; row < PROP_ROW; row ++)
    {
        for (int column = 0; column < PROP_COLUMN; column ++)
        {
            PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
            Prop itemProp = itemWidget->getProp();
            if (itemProp.getID() == PROP_ID_TOMCAT)
            {
                itemWidget->setPressed(false);
                itemWidget->setEnabled(true);
                return;
            }
        }
    }
}

void PropWidget::handleSetPigButtonFalse()
{
    PropWidgetDebug("Pig clicke timeoute");
    m_aPropButtonVoild["PROP_ID_PIG"].isValid = false;
    m_aPropButtonVoild["PROP_ID_PIG"].aTimer->stop();

    for (int row = 0; row < PROP_ROW; row ++)
    {
        for (int column = 0; column < PROP_COLUMN; column ++)
        {
            PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
            Prop itemProp = itemWidget->getProp();
            if (itemProp.getID() == PROP_ID_PIG)
            {
                itemWidget->setPressed(false);
                itemWidget->setEnabled(true);
                return;
            }
        }
    }
}

void PropWidget::handleSetKissButtonFalse()
{
    PropWidgetDebug("Kiss clicke timeout");
    m_aPropButtonVoild["PROP_ID_KISS"].isValid = false;
    m_aPropButtonVoild["PROP_ID_KISS"].aTimer->stop();

    for (int row = 0; row < PROP_ROW; row ++)
    {
        for (int column = 0; column < PROP_COLUMN; column ++)
        {
            PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
            Prop itemProp = itemWidget->getProp();
            if (itemProp.getID() == PROP_ID_KISS)
            {
                itemWidget->setPressed(false);
                itemWidget->setEnabled(true);
                return;
            }
        }
    }
}

void PropWidget::handleSetScrawlButtonFalse()
{
    PropWidgetDebug("Scrawl clicke timeout");
    m_aPropButtonVoild["PROP_ID_SCRAWL"].isValid = false;
    m_aPropButtonVoild["PROP_ID_SCRAWL"].aTimer->stop();

    for (int row = 0; row < PROP_ROW; row ++)
    {
        for (int column = 0; column < PROP_COLUMN; column ++)
        {
            PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
            Prop itemProp = itemWidget->getProp();
            if (itemProp.getID() == PROP_ID_SCRAWL)
            {
                itemWidget->setPressed(false);
                itemWidget->setEnabled(true);
                return;
            }
        }
    }
}

void PropWidget::handleSetFrogButtonFalse()
{
    PropWidgetDebug("Frog clicke timeout");
    m_aPropButtonVoild["PROP_ID_FROG"].isValid = false;
    m_aPropButtonVoild["PROP_ID_FROG"].aTimer->stop();

    for (int row = 0; row < PROP_ROW; row ++)
    {
        for (int column = 0; column < PROP_COLUMN; column ++)
        {
            PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
            Prop itemProp = itemWidget->getProp();
            if (itemProp.getID() == PROP_ID_FROG)
            {
                itemWidget->setPressed(false);
                itemWidget->setEnabled(true);
                return;
            }
        }
    }
}

void PropWidget::handleSetLuckywingsButtonFalse()
{
    PropWidgetDebug("Luckywings clicke timeout");
    m_aPropButtonVoild["PROP_ID_LUCKYWING"].isValid = false;
    m_aPropButtonVoild["PROP_ID_LUCKYWING"].aTimer->stop();

    for (int row = 0; row < PROP_ROW; row ++)
    {
        for (int column = 0; column < PROP_COLUMN; column ++)
        {
            PropItemWidget *itemWidget = dynamic_cast<PropItemWidget*>(ui->PropTableWidget->cellWidget(row, column));
            Prop itemProp = itemWidget->getProp();
            if (itemProp.getID() == PROP_ID_LUCKYWING)
            {
                itemWidget->setPressed(false);
                itemWidget->setEnabled(true);
                return;
            }
        }
    }
}


