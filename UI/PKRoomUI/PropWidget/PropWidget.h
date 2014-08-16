#ifndef PROPWIDGET_H
#define PROPWIDGET_H

#include <QWidget>
#include "PropItemWidget.h"

#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "Modules/StateController/StateController.h"
#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"

#include "UI/PKResultUI/RotateWidget.h"
#include <QMap>
#include <QTimer>

#define DEBUG_PROPWIDGET  1

#if DEBUG_PROPWIDGET
#include <QDebug>
#define PropWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define PropWidgetDebug(format,...)
#endif

#define PROP_ROW  3
#define PROP_COLUMN  2

namespace Ui {
class PropWidget;
}

struct ButtonValid
{
    bool isValid;
    QTimer  *aTimer;

};

class PropWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PropWidget(QWidget *parent = 0);
    ~PropWidget();

private:
    Ui::PropWidget *ui;

    ConfigHelper *cfg;
    PkInfoManager *pkInfo;
    YYNotificationCenter *yync;
    StateController       *stateCtrl;
    NodeNotificationCenter *nc;


public:
    QMap<QString, ButtonValid> m_aPropButtonVoild;


    RotateWidget *luckyWingBtnMask;
public:
    void resetProp();
    void updateLuckyWing();

    void updatePropItemWidget(Prop prop);

public slots:
    void handlePropTableWidgetCellClicked(int row, int column);
    void handlePropTableWidgetCellPressed(int row, int column);
    void handleLuckyWingsClicked();

    void handleStateControllerOnStatePropActive(Player, Prop);
    void handleSetTransformerButtonFalse();
    void handleSetTomcatButtonFalse();
    void handleSetPigButtonFalse();
    void handleSetKissButtonFalse();
    void handleSetScrawlButtonFalse();
    void handleSetFrogButtonFalse();
    void handleSetLuckywingsButtonFalse();

};

#endif // PROPWIDGET_H
