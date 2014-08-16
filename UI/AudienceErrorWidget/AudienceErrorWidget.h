#ifndef AUDIENCEERRORWIDGET_H
#define AUDIENCEERRORWIDGET_H

#include <QWidget>
#include "PkInfoManager/PkInfoManager.h"
#include "YYNotificationCenter/YYNotificationCenter.h"

namespace Ui {
class AudienceErrorWidget;
}

class AudienceErrorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AudienceErrorWidget(QWidget *parent = 0);
    ~AudienceErrorWidget();

private slots:
    void on_quitBtn_clicked();

private:
    Ui::AudienceErrorWidget *ui;
    QPixmap *pix;
    void paintEvent(QPaintEvent *);
signals:
    void onQuit();
};

#endif // AUDIENCEERRORWIDGET_H
