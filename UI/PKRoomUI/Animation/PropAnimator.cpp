#include "PropAnimator.h"
PropAnimator::PropAnimator(QObject *parent) :
    QObject(parent)
{
    aniCount = 0;
    aniTimer.setInterval(67);
    isComplexAni = false;
    state = "head";
    QObject::connect(&aniTimer ,SIGNAL(timeout()),this ,SLOT(handleAniTimeOut()));
    cfg = ConfigHelper::getInstance();
    audioPlayer = new AudioPlayer;

}

void PropAnimator::handleAniTimeOut()
{
    aniCount++;

    QString path = aniPath + QString(" (%1).png").arg(aniCount);
    QDir *dir = new QDir;
    if (!dir->exists(path))
    {
        if(isComplexAni && state != "tail")
        {
            if(state == "head")
            {
                state = "middle";
                loopCount = 0;
            }
            else if(state == "middle")
            {
                loopCount++;
                if(loopCount >= this->prop.getAniLoopCount())
                    state = "tail";
            }
            aniCount = 0;
            aniPath = cfg->getPluginPath() + "PropAnimation\\" +
                      QString("%1Prop\\%2_%3").arg(prop.getID()).arg(prop.getID()).arg(state);
        }
        else
        {
//            audioPlayer->stop();
//            delete audioPlayer;
            PropAnimatorDebug("animation finished");
            aniTimer.stop();
            aniLabel->deleteLater();
            emit propPlayEnd(prop);
        }
    }
    else
    {
        QPixmap pix(path);
        aniLabel->setFixedSize(pix.size());
        aniLabel->setPixmap(pix);
        aniLabel->setScaledContents(true);
        aniLabel->show();
    }

}

void PropAnimator::AnimateProp(Prop &prop , QWidget *parent, QRect geometry)
{
    for(int i = 0 ; i < cfg->getPropList().size() ; i++)
    {
        if(prop.getID() == cfg->getPropList().at(i).getID())
        {
            this->prop = cfg->getPropList().at(i);
        }
    }


//    audioPlayer->setSourceData(cfg->getPluginPath() + QString("PropSound\\%1.wav").arg(this->prop.getID()),
//                               AudioPlayer::eSourceFile,
//                               AudioPlayer::eDecodeWAV);
//    audioPlayer->start(false, false);



    aniLabel = new QLabel(parent);
    aniLabel->setGeometry(geometry);
    aniCount = 0;
    if(this->prop.getIsComplexAni())
    {
        isComplexAni = true;
        aniPath = cfg->getPluginPath() + "PropAnimation\\" +
                  QString("%1Prop\\%1_head").arg(prop.getID()).arg(prop.getID());
        state = "head";
        aniTimer.start();
    }
    else
    {
        isComplexAni = false;
        //test
        prop.setID(PROP_ID_SCRAWL);
        //--test end--
        aniPath = cfg->getPluginPath() + "PropAnimation\\" +
                  QString("%1Prop\\%2").arg(prop.getID()).arg(prop.getID());
        aniTimer.start();
    }
}

void PropAnimator::stopAnimate()
{
    aniTimer.stop();
    QTimer::singleShot(100 , aniLabel ,SLOT(deleteLater()));
}

