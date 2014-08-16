#include "EventHandler.h"
#include "Modules/ConfigHelper/ConfigHelper.h"

EventHandler::EventHandler(QObject *parent) :
    QObject(parent)
{
    pkInfo = PkInfoManager::getInstance();
    resManager = ResManager::getInstance();
}

EventHandler::~EventHandler()
{
    EventHandlerDebug("~EventHandler()");
}

void EventHandler::downloadRes4PkState()
{
    ConfigHelper *cfg = ConfigHelper::getInstance();

    resManager->reset();
    resManager->addDownLoadResRequest(pkInfo->getCurrentSong().getLyricFileName(),
                             pkInfo->getCurrentSong().getLyricUrl() ,
                             "");

    resManager->addDownLoadResRequest(pkInfo->getCurrentSong().getSectionFileName(),
                             pkInfo->getCurrentSong().getSectionUrl() ,
                             "");

//    resManager->addDownLoadResRequest(pkInfo->getCurrentSong().getAccompanyFileName(),
//                             pkInfo->getCurrentSong().getAccompanyUrl() ,
//                             cfg->getPluginPath() +  pkInfo->getCurrentSong().getAccompanyFilePath());
}





