#ifndef HOSTINFOWIDGET_H
#define HOSTINFOWIDGET_H

#include <QWidget>
#include "DataType/Player/Player.h"

namespace Ui {
class HostInfoWidget;
}

class HostInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HostInfoWidget(QWidget *parent = 0);
    ~HostInfoWidget();
    void updateHostInfo(Player host);
    void updateHostHeat(int heat);

    void reset();

private:
    Ui::HostInfoWidget *ui;
};


#endif // HOSTINFOWIDGET_H
