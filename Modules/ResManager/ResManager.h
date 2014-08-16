#ifndef RESMANAGER_H
#define RESMANAGER_H

#include <QObject>
#include "Modules/HttpDownload/HttpDownload.h"
#include "Modules/PkInfoManager/PkInfoManager.h"
#include <QHash>
#include "Modules/AudioPlayer/Decoder/MP3Decoder/MP3Decoder.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
#include "Modules/FileCut/FileCut.h"

#define DEBUG_RES_MANAGER  1

#if DEBUG_RES_MANAGER
#include <QDebug>
#define ResManagerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__,__LINE__, ##__VA_ARGS__)
#else
#define ResManagerDebug(format,...)
#endif

class ResManager : public QObject
{
    Q_OBJECT
public:
    explicit ResManager(QObject *parent = 0);
    ~ResManager();
private:
    typedef enum
    {
        NOT_DL,
        DL_OK,
        DL_FAIL
    }DownloadState;

    static QAtomicPointer<ResManager> instance;
    static QMutex instanceMutex;
public:
    static ResManager *getInstance();
    static void destory();

    void     reset();
    void     addDownLoadResRequest(QString fileName , QUrl url , QString filePath);
    int getSampleRate() const;
    void setSampleRate(int value);

    int getChannels() const;
    void setChannels(int value);

    int getBits() const;
    void setBits(int value);

signals:
    void     onDownloadResPrepared(QString , bool );
public slots:
private:
    QHash<QString , DownloadState> dlResState;
    HttpDownload          *httpDownload;
    PkInfoManager         *pkInfo;
    QThread               *thread;
    ConfigHelper          *cfg;
    FileCut               *fileCut;

    int                   sampleRate;
    int                   channels;
    int                   bits;
    QByteArray            mp3Data;
public :
    void     cutPcm(QString inFileName, QString outFileName, int bits, int channel, int rate, int startTime,int endTime);
private slots:
    void     handleThreadOnStart();
    void     handleHttpDownloadOnDownloadFinished(QString ,bool,qint64 allByteRead);
    void     handleHttpDownloadOnProgress(QString keyword, QByteArray data, qint64 allByteRead );
    void     handleClearErrorData(QString keyword);
};
#endif
