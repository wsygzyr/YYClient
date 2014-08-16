#ifndef AUDIENCEINVITECONTROLLER_H
#define AUDIENCEINVITECONTROLLER_H

#include <QObject>
#include "UI/AudienceInvitePKWidget/AudienceInvitePKWidget.h"

#define DEBUG_AUDIENCE_INVITE_CONTROLLER  1

#if DEBUG_AUDIENCE_INVITE_CONTROLLER
#include <QDebug>
#define AudienceInviteControllerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define AudienceInviteControllerDebug(format,...)
#endif


class AudienceInviteController : public QObject
{
    Q_OBJECT
public:
    explicit AudienceInviteController(QObject *parent = 0);
    void     setAudienceInvitePKWidget(AudienceInvitePKWidget* widget);

signals:
    void onCloseBtnClicked();
public slots:
private:
    AudienceInvitePKWidget *AudienceInviteWidget;

};

#endif // AUDIENCEINVITECONTROLLER_H
