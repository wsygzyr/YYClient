#ifndef REWARDEDITWIDGET_H
#define REWARDEDITWIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QTextEdit>
#include "Modules/PkInfoManager/PkInfoManager.h"
#include "UI/TextEdit/TextEdit.h"
#include "UI/ToastWidget/ToastWidget.h"

#define DEBUG_REWARD_EDIT_WIDGET  1

#if DEBUG_REWARD_EDIT_WIDGET
#include <QDebug>
#define RewardEditDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define RewardEditDebug(format,...)
#endif

namespace Ui {
class RewardEditWidget;
}

class RewardEditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RewardEditWidget(QList<VictoryReward> vicList,bool bEdit, int index, QWidget *parent = 0);
    ~RewardEditWidget();
signals:
    void onCancel();
    void onDelete(int index);
    void onSave(int index, QString RewardName, QString RewardDesp);
private slots:
    void on_SaveBtn_clicked();

    void on_CancelBtn_clicked();

    void on_DelBtn_clicked();

private:
    Ui::RewardEditWidget *ui;
    TextEdit*  textEdit;
    QPixmap *pixmap;
    int index;
private:
    void paintEvent(QPaintEvent *event);
};

#endif // REWARDEDITWIDGET_H
