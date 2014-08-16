#include "Animator.h"
#include <QDir>
Animator::Animator(QObject *parent) :
    QObject(parent)
{
    aniCount = 0;
    aniLabel = NULL;
    QObject::connect(&aniTimer ,SIGNAL(timeout()),this ,SLOT(handleAniTimeOut()));
}

Animator::~Animator()
{
}

void Animator::handleAniTimeOut()
{
    aniCount++;
    QString path = aniPath + QString(" (%1).png").arg(aniCount);

    QDir dir;
    if (!dir.exists(path))
    {
        AnimatorDebug("handleAniTimeOut dir not exist");
 //       AnimatorDebug("handleAniTimeOut dir path:%s", path.toUtf8().data());
        aniTimer.stop();
        aniLabel->hide();
        emit playEnd();
        if(bSelfDelete)
        {
            aniLabel->deleteLater();
            aniLabel = NULL;
            this->deleteLater();
            return;
        }
    }

    if(aniLabel)
    {
        QPixmap pix(path);
        aniLabel->setPixmap(pix);
        aniLabel->setScaledContents(true);
        aniLabel->show();
    }
}

void Animator::Animate(QString path , QString name , QWidget *parent , QRect geo , int frameRate, bool selfDelete)
{
    if(!aniLabel)
        aniLabel = new QLabel(parent);
    aniLabel->setGeometry(geo);
    aniTimer.setInterval(1000 / frameRate);
    AnimatorDebug("%d ,%d ,%d,%d",aniLabel->x() ,aniLabel->y() ,aniLabel->width() ,aniLabel->height());
    aniCount = 0;
    aniPath = QString("%1/%2/%3").arg(path,name,name);
    aniTimer.start();

    bSelfDelete = selfDelete;
}

void Animator::stop()
{
    aniTimer.stop();
    if(aniLabel)
        aniLabel->hide();
}

