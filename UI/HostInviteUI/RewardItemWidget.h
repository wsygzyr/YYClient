#ifndef REWARDITEMWIDGET_H
#define REWARDITEMWIDGET_H

#include <QWidget>

namespace Ui {
class RewardItemWidget;
}

class RewardItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RewardItemWidget(int column, QWidget *parent = 0);
    ~RewardItemWidget();

private:
    Ui::RewardItemWidget *ui;
};

#endif // REWARDITEMWIDGET_H
