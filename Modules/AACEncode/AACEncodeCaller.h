#ifndef AACENCODECALLER_H
#define AACENCODECALLER_H

#include <QMutex>

#include <QLibrary>
#include <QMessageBox>

#define DEBUG_AAC_ENCODE_CALLER     1

#if DEBUG_AAC_ENCODE_CALLER
#define AACEncodeCallerDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#include <QDebug>
#else
#define AACEncodeCallerDebug(format,...)
#endif

#ifdef _DEBUG
#define AACDLL "dll/libAACEncd.dll"
#else
#define AACDLL "dll/libAACEnc.dll"
#endif


//显示调用dll初始化

class AACEncodeCaller : public QObject
{
    Q_OBJECT

    AACEncodeCaller(QObject *parent = 0);
    ~AACEncodeCaller();

/*
 *  singleton
 */
private:
    static QAtomicPointer<AACEncodeCaller> instance;
    static QMutex instanceMutex;

    static QLibrary libAAC;

public:
    static AACEncodeCaller *getInstance();
    static void destroy();

public:
    bool init(int bitrate = 128000,
              int channels = 1,
              int sampleRate = 22050,
              int bitsPerSample = 16);
    void uninit();
    QByteArray encode(QByteArray pcmData);
};

#endif // AACENCODECALLER_H
