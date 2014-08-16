#include "AACEncodeCaller.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
typedef struct {
    char* buffer;
    int length;
}byteArray;

//定义aac编码的指针函数
typedef int (*pAACInit)(int bitrate,
                        int channels,
                        int sampleRate,
                        int bitsPerSample);
typedef void (*pAACUninit)();
typedef byteArray (*pAACEncode)(char* pcmBuffer,
                                int length);


pAACInit pFuncAACInit ;
pAACUninit pFuncAACUninit;
pAACEncode pFuncAACEncode;

QLibrary AACEncodeCaller::libAAC;
AACEncodeCaller::AACEncodeCaller(QObject *parent) :
    QObject(parent)
{
    ConfigHelper *cfg = ConfigHelper::getInstance();
    libAAC.setFileName(cfg->getPluginPath() + AACDLL);

    if(libAAC.load() == false)
    {
        AACEncodeCallerDebug("Can't Load libAACEnc.dll :%s !", AACDLL);
        QMessageBox::information(NULL, "Error !", "Cannot load " + QString(AACDLL));
        return;
    }
    pFuncAACInit = (pAACInit)libAAC.resolve("AACInit");    //援引 aacInit() 函数
    if(pFuncAACInit == NULL)
    {
        AACEncodeCallerDebug("Can't Link to AACInit() !");
        return;
    }

    pFuncAACUninit = (pAACUninit)libAAC.resolve("AACUnInit");
    if(pFuncAACUninit == NULL)
    {
        AACEncodeCallerDebug("Can't Link to AACUnInit() !");
        return;
    }

    pFuncAACEncode = (pAACEncode)libAAC.resolve("AACEncode");
    if(pFuncAACEncode == NULL)
    {
        AACEncodeCallerDebug("Can't Link to AACEncode() !");
        return;
    }

    AACEncodeCallerDebug("Load libAAC succeed!");
}

AACEncodeCaller::~AACEncodeCaller()
{
    AACEncodeCallerDebug("");

    libAAC.unload();
}

QAtomicPointer<AACEncodeCaller> AACEncodeCaller::instance;
QMutex AACEncodeCaller::instanceMutex;

AACEncodeCaller* AACEncodeCaller::getInstance()
{
    AACEncodeCallerDebug("getInstance!");
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            AACEncodeCallerDebug("create instance!");
            instance = new AACEncodeCaller();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void AACEncodeCaller::destroy()
{
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = NULL;
    }
    instanceMutex.unlock();
}

bool AACEncodeCaller::init(int bitrate, int channels, int sampleRate, int bitsPerSample)
{
    if (pFuncAACInit(bitrate, channels, sampleRate, bitsPerSample))
    {
        AACEncodeCallerDebug("Init AACEncodeCaller failed!");
        return false;
    }
    AACEncodeCallerDebug("Init AACEncodeCaller succeed!");
    return true;
}

void AACEncodeCaller::uninit()
{
    AACEncodeCallerDebug("uninit!");
    pFuncAACUninit();
}

QByteArray AACEncodeCaller::encode(QByteArray pcmData)
{
    AACEncodeCallerDebug("encode!");
    byteArray bytearray = pFuncAACEncode( pcmData.data(), pcmData.length() );
    QByteArray aacData( bytearray.buffer, bytearray.length );
    return aacData;
}
