#include "AudienceInviteController.h"
#include "ui_AudienceInvitePKWidget.h"

AudienceInviteController::AudienceInviteController(QObject *parent) :
    QObject(parent)
{
}


void AudienceInviteController::setAudienceInvitePKWidget(AudienceInvitePKWidget * widget)
{
    AudienceInviteControllerDebug("AudienceInviteController setAudienceInvitePKWidget");
    AudienceInviteWidget = widget;

    connect(AudienceInviteWidget->ui->closeBtn , SIGNAL(clicked()) , this , SIGNAL(onCloseBtnClicked()));
}
