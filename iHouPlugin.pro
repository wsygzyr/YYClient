#-------------------------------------------------
#
# Project created by QtCreator 2014-03-10T14:19:45
#
#-------------------------------------------------

QT       += core\
            gui\
            network\
            multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iHouPlugin
TEMPLATE = app
CONFIG += embed_manifest_exe
include(Modules/log4qt/log4qt.pri)
SOURCES += main.cpp\
    Modules/SESMark/SesMarkCaller.cpp \
    Modules/AACEncode/AACEncodeCaller.cpp \
    Modules/ConfigHelper/ConfigHelper.cpp \
    Modules/HttpDownload/HttpDownload.cpp \
    Modules/HttpGet/HttpGet.cpp \
    Modules/YYNotificationCenter/YYNotificationCenter.cpp \
    Modules/LyricParse/LyricParse.cpp \
    Modules/PkInfoManager/PkInfoManager.cpp \
    UI/HostMainWidget.cpp \
    UI/AudienceMainWidget.cpp \
    UI/PKRoomUI/MessageWidget/MessageWidget.cpp \
    UI/PKRoomUI/PlayerListWidget/PlayerWidget.cpp \
    UI/PKRoomUI/PlayerListWidget/PlayerListWidget.cpp \
    DataType/Player/Player.cpp \
    DataType/Prop/Prop.cpp \
    DataType/Score/Score.cpp \
    DataType/Song/Song.cpp \
    DataType/VictoryReward/VictoryReward.cpp \
    UI/PKRoomUI/PKRoomWidget.cpp \
    UI/PKRoomUI/ScoreWidget/ScoreWidget.cpp \
    UI/HostBidUI/HostBidWidget.cpp \
    UI/PKResultUI/PKResultWidget.cpp \
    UI/HostBidUI/HostBidController.cpp \
    UI/HostInviteUI/HostInviteController.cpp \
    UI/PKResultUI/PKResultController.cpp \
    UI/PKRoomUI/PKRoomController.cpp \
    Modules/LyricParse/LyricParse.cpp \
    UI/PKRoomUI/LyricWidget/LyricWiget.cpp \
    UI/PKRoomUI/LyricWidget/LyricLabel.cpp \
    UI/BidResultWidget/BidResultWidget.cpp \
    UI/AudienceBidWidget/AudienceBidWidget.cpp \
    UI/AudienceInvitePKWidget/AudienceInvitePKWidget.cpp \
    UI/TableWidget/TableWidget.cpp \
    UI/TableWidget/YYCheckBox.cpp \
    Modules/YYNotificationCenter/YYTCPPipe.cpp \
    Modules/TempVarCollector/TempVarCollector.cpp \
    Modules/AudioRecord/AudioRecord.cpp \
    Modules/SingEngine/SingEngine.cpp \
    Modules/AudioPlayer/Decoder/MP3Decoder/MP3Decoder.cpp \
    Modules/AudioPlayer/Decoder/WAVDecoder/WAVDecoder.cpp \
    Modules/AudioPlayer/Decoder/Decoder.cpp \
    Modules/AudioPlayer/PCMPlayer/PCMplayer.cpp \
    Modules/AudioPlayer/SourceData/SourceFile/SourceFile.cpp \
    Modules/AudioPlayer/SourceData/SourceData.cpp \
    Modules/AudioPlayer/AudioPlayer.cpp \
    Modules/AudioPlayer/SourceData/SourceRAM/SourceRAM.cpp \
    Modules/DecodeAudioFile/DecodeAudioFile.cpp \
    Modules/NodeNotificationCenter/node.cpp \
    Modules/NodeNotificationCenter/NodeNotificationCenter.cpp \
    Modules/JsonPrase/cJSON.c \
    Modules/ChangeSpeech/ChangeSpeech.cpp \
    Modules/LogHelper/LogHelperWidget.cpp \
    Modules/ihouAuth/ihouAuth.c \
    Modules/ihouAuth/des/base64.c \
    Modules/ihouAuth/des/DES.c \
    Modules/ihouAuth/des/set_key.c \
    Modules/ihouAuth/des/spr.c \
    UI/PKRoomUI/HostInfoWidget/HostInfoWidget.cpp \
    UI/PKRoomUI/PeopleAnimeWidget/PeopleAnimeWidget.cpp \
    UI/PKRoomUI/PropWidget/PropWidget.cpp \
    UI/PKRoomUI/PropWidget/PropWidget.cpp \
    UI/PKRoomUI/PropWidget/PropItemWidget.cpp \
    UI/HostHomeUI/HostHomeWidget.cpp \
    UI/HostHomeUI/HostHomeController.cpp \
    UI/PKResultUI/QHoverWidget.cpp \
    UI/BidResultWidget/BidResultFailedWidget.cpp \
    DataType/MainPageInfo/MainPageInfo.cpp \
    UI/PKRoomUI/Animation/AnimationManager.cpp \
    UI/PKRoomUI/Animation/PropAnimation.cpp \
    UI/PKRoomUI/Animation/NStepPropAnimation.cpp \
    UI/PKRoomUI/Animation/ScrawlPropAnimation.cpp \
    Modules/MMTimer/MMTimer.cpp\
    UI/RewardEditWidget/RewardEditWidget.cpp \
    UI/AudienceErrorWidget/AudienceErrorWidget.cpp \
    UI/TextEdit/TextEdit.cpp \
    UI/FeedBackWidget/FeedBackWidget.cpp \
    UI/ToastWidget/ToastWidget.cpp \
    Modules/FilePlayer/FilePlayer.cpp \
    UI/HelpWidget/HelpWidget.cpp \
    UI/PKResultUI/RotateWidget.cpp \
    Modules/Animator/Animator.cpp \
    Modules/ResManager/ResManager.cpp \
    UI/PKRoomUI/SingChangeErrorWidget/SingChangeErrorWidget.cpp \
    Modules/FadeHelper/FadeHelper.cpp \
    Modules/HttpUpload/HttpUpLoad.cpp \
    UI/PKRoomUI/ResourceErrorWidget/ResourceErrorWidget.cpp \
    Modules/FileCut/FileCut.cpp \
    Modules/WindowFollowHelper/WindowFollowHelper.cpp \
    Modules/TextShowHelper/TextShowHelper.cpp \
    UI/SongSelectWidget/SongMenuControl.cpp \
    UI/SongSelectWidget/SongMenuWidget.cpp \
    DataType/Prog/Prog.cpp \
    DataType/Singer/Singer.cpp \
    Modules/StateController/StateController.cpp \
    Modules/StateController/EventHandler.cpp \
    UI/PKRoomUI/PKPlayerListWidget/pkplayerlistwidget.cpp \
    UI/ExceptionWidget/ExceptionWidget.cpp \
    Modules/ExceptionController/ExceptionController.cpp\
    UI/PKRoomUI/SongInfoDisplayWidget/SongInfoDisplayWidget.cpp \
    UI/AudienceInvitePKWidget/AudienceInviteController.cpp \
    UI/BidResultWidget/BidResultController.cpp\
    Modules/AudioHelper/AudioHelper.cpp \
    UI/HostInviteUI/HostInviteWidget.cpp








HEADERS  += \
    Modules/SESMark/SesMarkCaller.h \
    Modules/AACEncode/AACEncodeCaller.h \
    Modules/ConfigHelper/ConfigHelper.h \
    Modules/HttpDownload/HttpDownload.h \
    Modules/HttpGet/HttpGet.h \
    Modules/YYNotificationCenter/YYNotificationCenter.h \
    Modules/LyricParse/LyricParse.h \
    Modules/PkInfoManager/PkInfoManager.h \
    UI/HostMainWidget.h \
    UI/AudienceMainWidget.h \
    UI/PKRoomUI/MessageWidget/MessageWidget.h \
    UI/PKRoomUI/PlayerListWidget/PlayerWidget.h \
    UI/PKRoomUI/PlayerListWidget/PlayerListWidget.h \
    DataType/Player/Player.h \
    DataType/Prop/Prop.h \
    DataType/Score/Score.h \
    DataType/Song/Song.h \
    DataType/VictoryReward/VictoryReward.h \
    UI/PKRoomUI/PKRoomWidget.h \
    UI/PKRoomUI/ScoreWidget/ScoreWidget.h \
    UI/HostBidUI/HostBidWidget.h \
    UI/PKResultUI/PKResultWidget.h \
    UI/HostBidUI/HostBidController.h \
    UI/HostInviteUI/HostInviteController.h \
    UI/PKResultUI/PKResultController.h \
    UI/PKRoomUI/PKRoomController.h \
    UI/PKRoomUI/LyricWidget/LyricWidget.h \
    UI/PKRoomUI/LyricWidget/LyricLabel.h \
    UI/BidResultWidget/BidResultWidget.h \
    UI/AudienceBidWidget/AudienceBidWidget.h \
    UI/AudienceInvitePKWidget/AudienceInvitePKWidget.h \
    UI/TableWidget/TableWidget.h \
    UI/TableWidget/YYCheckBox.h \
    Modules/YYNotificationCenter/YYTCPPipe.h \
    Modules/TempVarCollector/TempVarCollector.h \
    Modules/AudioRecord/AudioRecord.h \
    Modules/SingEngine/SingEngine.h \
    Modules/AudioPlayer/Decoder/MP3Decoder/MP3Decoder.h \
    Modules/AudioPlayer/Decoder/WAVDecoder/WAVDecoder.h \
    Modules/AudioPlayer/Decoder/Decoder.h \
    Modules/AudioPlayer/PCMPlayer/PCMplayer.h \
    Modules/AudioPlayer/SourceData/SourceFile/SourceFile.h \
    Modules/AudioPlayer/SourceData/SourceData.h \
    Modules/AudioPlayer/AudioPlayer.h \
    Modules/AudioPlayer/SourceData/SourceRAM/SourceRAM.h \
    Modules/DecodeAudioFile/DecodeAudioFile.h \
    Modules/NodeNotificationCenter/node.h \
    Modules/NodeNotificationCenter/NodeNotificationCenter.h \
    Modules/JsonPrase/cJSON.h \
    Modules/ChangeSpeech/ChangeSpeech.h \
    Modules/LogHelper/LogHelperWidget.h \
    Modules/ihouAuth/ihouAuth.h \
    Modules/ihouAuth/des/base64.h \
    Modules/ihouAuth/des/DES.h \
    Modules/ihouAuth/des/des_locl.h \
    Modules/ihouAuth/des/spr.h \
    UI/PKRoomUI/HostInfoWidget/HostInfoWidget.h \
    UI/PKRoomUI/PeopleAnimeWidget/PeopleAnimeWidget.h \
    Modules/SESMark/SES3DApi.h \
    UI/PKRoomUI/PropWidget/PropWidget.h \
    UI/PKRoomUI/PropWidget/PropItemWidget.h \
    UI/PKRoomUI/PropWidget/PropWidget.h \
    UI/HostHomeUI/HostHomeWidget.h \
    UI/HostHomeUI/HostHomeController.h \
    UI/PKResultUI/QHoverWidget.h \
    UI/BidResultWidget/BidResultFailedWidget.h \
    DataType/MainPageInfo/MainPageInfo.h \
    UI/PKRoomUI/Animation/AnimationManager.h \
    UI/PKRoomUI/Animation/PropAnimation.h \
    UI/PKRoomUI/Animation/NStepPropAnimation.h \
    UI/PKRoomUI/Animation/ScrawlPropAnimation.h \
    Modules/MMTimer/MMTimer.h\
    UI/RewardEditWidget/RewardEditWidget.h \
    UI/AudienceErrorWidget/AudienceErrorWidget.h \
    UI/TextEdit/TextEdit.h \
    UI/FeedBackWidget/FeedBackWidget.h \
    UI/ToastWidget/ToastWidget.h \
    Modules/FilePlayer/FilePlayer.h \
    UI/HelpWidget/HelpWidget.h \
    UI/PKResultUI/RotateWidget.h \
    Modules/Animator/Animator.h \
    Modules/ResManager/ResManager.h \
    UI/PKRoomUI/SingChangeErrorWidget/SingChangeErrorWidget.h \
    Modules/FadeHelper/FadeHelper.h \
    Modules/HttpUpload/HttpUpLoad.h \
    UI/PKRoomUI/ResourceErrorWidget/ResourceErrorWidget.h \
    Modules/FileCut/FileCut.h \
    Modules/WindowFollowHelper/WindowFollowHelper.h \
    Modules/TextShowHelper/TextShowHelper.h \
    Modules/StateController/StateController.h \
    Modules/StateController/EventHandler.h \
    UI/SongSelectWidget/SongMenuControl.h \
    UI/SongSelectWidget/SongMenuWidget.h \
    DataType/Prog/Prog.h \
    DataType/Singer/Singer.h \
    UI/PKRoomUI/PKPlayerListWidget/pkplayerlistwidget.h \
    UI/ExceptionWidget/ExceptionWidget.h \
    Modules/ExceptionController/ExceptionController.h\
    UI/PKRoomUI/SongInfoDisplayWidget/SongInfoDisplayWidget.h \
    UI/AudienceInvitePKWidget/AudienceInviteController.h \
    UI/BidResultWidget/BidResultController.h\
    Modules/AudioHelper/AudioHelper.h \
    UI/HostInviteUI/HostInviteWidget.h








RESOURCES += \
    resource/Res.qrc

INCLUDEPATH += ./Modules  \
               ./DataType \
                           ./UI \
                           ./ \



FORMS += \
    UI/PKRoomUI/MessageWidget/MessageWidget.ui \
    UI/PKRoomUI/PlayerListWidget/PlayerWidget.ui \
    UI/PKRoomUI/PlayerListWidget/PlayerListWidget.ui \
    UI/PKRoomUI/PKRoomWidget.ui \
    UI/PKRoomUI/ScoreWidget/ScoreWidget.ui \
    UI/HostInviteUI/HostInviteWidget.ui \
    UI/HostBidUI/HostBidWidget.ui \
    UI/PKResultUI/PKResultWidget.ui \
    UI/PKRoomUI/LyricWidget/LyricWidget.ui \
    UI/BidResultWidget/BidResultWidget.ui \
    UI/AudienceBidWidget/AudienceBidWidget.ui \
    UI/AudienceInvitePKWidget/AudienceInvitePKWidget.ui \
    UI/PKRoomUI/HostInfoWidget/HostInfoWidget.ui \
    UI/PKRoomUI/PeopleAnimeWidget/PeopleAnimeWidget.ui \
    UI/PKRoomUI/PropWidget/PropWidget.ui \
    UI/PKRoomUI/PropWidget/PropWidget.ui \
    UI/PKRoomUI/PropWidget/PropItemWidget.ui \
    UI/HostHomeUI/HostHomeWidget.ui \
    UI/BidResultWidget/BidResultFailedWidget.ui\
    UI/RewardEditWidget/RewardEditWidget.ui \
    UI/AudienceErrorWidget/AudienceErrorWidget.ui \
    UI/FeedBackWidget/FeedBackWidget.ui \
    UI/HelpWidget/HelpWidget.ui \
    UI/PKRoomUI/SingChangeErrorWidget/SingChangeErrorWidget.ui \
    UI/PKRoomUI/ResourceErrorWidget/ResourceErrorWidget.ui \
    UI/SongSelectWidget/SongMenuWidget.ui \
    UI/PKRoomUI/PKPlayerListWidget/pkplayerlistwidget.ui \
    UI/ExceptionWidget/ExceptionWidget.ui





win32:CONFIG(release, debug|release): LIBS += -L$$PWD/Modules/SESMark  -llibSES3DLocal
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/Modules/SESMark  -llibSES3DLocald



