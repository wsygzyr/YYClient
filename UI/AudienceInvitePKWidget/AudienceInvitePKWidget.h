#ifndef AUDIENCEONINVITEPKWIDGET_H
#define AUDIENCEONINVITEPKWIDGET_H

#include <QTableWidget>
#include <QMouseEvent>
#include "DataType/VictoryReward/VictoryReward.h"
#include "UI/TableWidget/TableWidget.h"
#include "Modules/YYNotificationCenter/YYNotificationCenter.h"
#include "Modules/NodeNotificationCenter/NodeNotificationCenter.h"
#include "Modules/HttpDownload/HttpDownload.h"

#define DEBUG_AUDIENCE_ON_INVITE_PK  1

#if DEBUG_AUDIENCE_ON_INVITE_PK
#include <QDebug>
#define AudienceInvitePKWidgetDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define AudienceInvitePKWidgetDebug(format,...)
#endif

namespace Ui {
class AudienceInvitePKWidget;
}

class AudienceInviteController;
class AudienceInvitePKWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AudienceInvitePKWidget(QWidget *parent = 0);
    ~AudienceInvitePKWidget();
    //function ()
public:
//    void httpDownLoadRewardList();
    void configRewardList(int index);
    void configRewardItem(int index);

    void setUIControlsInfo(Song song,
                   int acceptedPlayers);
    void show();
public slots:
//    void handleHttpDownLoadOnFinished(QString keyword, bool error);

private:
    void configUI();
    void paintEvent(QPaintEvent *event);
private slots:
    void on_JoinBtn_clicked();

signals:

    void onPopOutAudienceBidWidget();
    void onAcceptInviteEvent();

private:
    Ui::AudienceInvitePKWidget *ui;
    HttpDownload *httpDownload;
    PkInfoManager *info;
    QPixmap       *pixmap;
private:
    QPoint      windowPos;
    QPoint      mousePos;
    QPoint      dPos;
    bool        bMouseLeftBtnDown;
    QPoint      pointMove;
    void    mousePressEvent(QMouseEvent *event);
    void    mouseMoveEvent(QMouseEvent *event);
    void    mouseReleaseEvent(QMouseEvent *event);
    bool    winEvent(MSG *message, long *result);
public:
    friend class AudienceInviteController;
};

#endif // AUDIENCEONINVITEPKWIDGET_H
