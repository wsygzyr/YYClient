#ifndef PKPLAYERLISTWIDGET_H
#define PKPLAYERLISTWIDGET_H

#include <QWidget>
#include "Player/Player.h"

#define DEBUG_PKPlAYER_LIST_WIDGET  1

#if DEBUG_PKPlAYER_LIST_WIDGET
#include <QDebug>
#define PKPlayerListWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define PKPlayerListWidgetDebug(format,...)
#endif

namespace Ui {
class PKPlayerListWidget;
}

class PKPlayerListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PKPlayerListWidget(QWidget *parent = 0);
    ~PKPlayerListWidget();

    void UpdatePKPlayerList(const QList<Player> &playerList);
    void UpdatePlayerListTitle(bool isPlayer);


    void PKPlayerListWidgetShow(bool isShow);

private:
    void paintEvent(QPaintEvent *event);

private:
    Ui::PKPlayerListWidget *ui;
    QPixmap *px;
};

#endif // PKPLAYERLISTWIDGET_H
