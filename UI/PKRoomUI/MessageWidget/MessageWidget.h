#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include "Player/Player.h"
#include "Prop/Prop.h"

#define DEBUG_MESSAGE_WIDGET 1

#if DEBUG_MESSAGE_WIDGET
#include <QDebug>
#define MessageWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define Debug(format,...)
#endif


#define NAME_LONGEST_LENGTH     65

namespace Ui {
class MessageWidget;
}

class MessageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MessageWidget(QWidget *parent = 0);
    ~MessageWidget();

private:
    Ui::MessageWidget *ui;

public slots:
    void updateMessage(const Player &player, Prop prop);
    void reset();
};

#endif // MESSAGEWIDGET_H
