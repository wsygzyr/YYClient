#include "ChangeSpeech.h"
#include <QFile>
#include "Modules/ConfigHelper/ConfigHelper.h"
typedef int (*pInitChangePitch)(void ** ppInst,
                                int eChangeType,
                                int eAuidoSampleRate);

typedef int (*pProcess)(void * pInst,
                        float * pfInput,
                        int nSampleCount);

typedef int (*pUnInitChangePitch)(void * pInst);

pInitChangePitch            pFunInitChangePitch;
pProcess                    pFunProcess;
pUnInitChangePitch          pFunUnInitChangePitch;


QLibrary ChangeSpeech::changeSpeechDll;
void* ChangeSpeech::hPitchshift;
ChangeSpeech::ChangeSpeech(QObject *parent) :
    QObject(parent)
{
    ConfigHelper *cfg = ConfigHelper::getInstance();
    changeSpeechDll.setFileName(cfg->getPluginPath() + CHANGESPEECHDLL);
    hPitchshift = NULL;

    if (! changeSpeechDll.load())
    {
        ChangeSpeechDebug("load %s failed! ,error is %s",
                          QString(cfg->getPluginPath() + CHANGESPEECHDLL).toUtf8().data(),
                          changeSpeechDll.errorString().toUtf8().data());
        return;
    }
    if (NULL == (pFunInitChangePitch =
           (pInitChangePitch)changeSpeechDll.resolve("initChangePitch")))
    {
        ChangeSpeechDebug("%s resolve initChangePitch() failed!", CHANGESPEECHDLL);
        return;
    }
    if (NULL == (pFunProcess =
           (pProcess)changeSpeechDll.resolve("process")))
    {
        ChangeSpeechDebug("%s resolve process() failed!", CHANGESPEECHDLL);
        return;
    }
    if (NULL == (pFunUnInitChangePitch =
           (pUnInitChangePitch)changeSpeechDll.resolve("unInitChangePitch")))
    {
        ChangeSpeechDebug("%s resolve unInitChangePitch() failed!", CHANGESPEECHDLL);
        return;
    }
    ChangeSpeechDebug("load %s successed!", CHANGESPEECHDLL);
}

ChangeSpeech::~ChangeSpeech()
{
    changeSpeechDll.unload();
    ChangeSpeechDebug("unload %s successed!", CHANGESPEECHDLL);
}

QAtomicPointer<ChangeSpeech> ChangeSpeech::instance;
QMutex ChangeSpeech::instanceMutex;
ChangeSpeech *ChangeSpeech::getInstance()
{
    ChangeSpeechDebug("getInstance!");
    if (!instance)
    {
        instanceMutex.lock();
        if (!instance)
        {
            ChangeSpeechDebug("create instance!");
            instance = new ChangeSpeech();
        }
        instanceMutex.unlock();
    }
    return instance;
}

void ChangeSpeech::destroy()
{
    instanceMutex.lock();
    if (instance)
    {
        delete instance;
        instance = NULL;
    }
    instanceMutex.unlock();
}

bool ChangeSpeech::init(ChangeSpeech::EChangeType changeType,
                        ChangeSpeech::EAuidoSampleRate audioSampleRate)
{
    mutex.lock();
    if (!changeSpeechDll.isLoaded())
    {
        ChangeSpeechDebug("load %s failed!", CHANGESPEECHDLL);
        mutex.unlock();
        return false;
    }

    if (hPitchshift != NULL)
    {
        ChangeSpeechDebug("is already init!");
        mutex.unlock();
        return false;
    }

    if (pFunInitChangePitch(&hPitchshift, changeType, audioSampleRate) == -1)
    {
        ChangeSpeechDebug("init failed!");
        mutex.unlock();
        return false;
    }
    ChangeSpeechDebug("ChangeSpeech init success");

    mutex.unlock();
    return true;
}

void ChangeSpeech::unInit()
{
    mutex.lock();
    if (!changeSpeechDll.isLoaded())
    {
        ChangeSpeechDebug("%s isn't loaded!", CHANGESPEECHDLL);
        mutex.unlock();
        return;
    }
    if (hPitchshift == NULL)
    {
        ChangeSpeechDebug("changeSpeech isn't init!");
        mutex.unlock();
        return;
    }
    pFunUnInitChangePitch(hPitchshift);
    hPitchshift = NULL;
    mutex.unlock();
}

QByteArray ChangeSpeech::process(QByteArray input)
{
    mutex.lock();
    QByteArray data;
    data.clear();
    if (!changeSpeechDll.isLoaded())
    {
        ChangeSpeechDebug("%s isn't loading!", CHANGESPEECHDLL);
        mutex.unlock();
        return data;
    }
    if (hPitchshift == NULL)
    {
        ChangeSpeechDebug("changeSpeech isn't init!");
        mutex.unlock();
        return data;
    }
    qint64 length = input.length() / 2;
    float* inputData = new float[length];
    for (int i=0; i < length; i++)
    {
        inputData[i] = ((short*)input.data())[i] / 32768.0;
    }
    pFunProcess(hPitchshift, inputData, length);

    char *cTemp = new char[2*length];
    memset(cTemp, '0', 2*length);

    for (int i=0; i<length; i++)
    {
        float fTemp = inputData[i] * 32768.0;
        short sTemp = (short)fTemp;
        memcpy(cTemp+2*i, &sTemp, 2);
    }

    QByteArray temp(cTemp, 2*length);
    data.append(temp);
    delete[] cTemp;
    delete[] inputData;

    mutex.unlock();

    return data;
}
