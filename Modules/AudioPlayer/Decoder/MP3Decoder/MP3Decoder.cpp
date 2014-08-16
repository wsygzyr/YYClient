#include "MP3Decoder.h"
#include "Modules/ConfigHelper/ConfigHelper.h"
//Define DLL function pointer
typedef int (*pDecoderInit)(int bufLen);
typedef int (*pDecoderUninit)(int handle);
typedef int (*pGetBitRate)(int handle);
typedef int (*pGetChannels)(int handle);
typedef int (*pGetSampleRate)(int handle);
typedef float (*pGetDuration)(int handle,int fileLen) ;
typedef unsigned char* (*pDecode)(int handle,
                                  const unsigned char* in,
                                  int inLen,
                                  int eof,
                                  int *outLen);

pDecoderInit                    pFunDecoderInit;
pDecoderUninit                  pFunDecoderUninit;
pGetBitRate                     pFunGetBitRate;
pGetChannels                    pFunGetChannels;
pGetSampleRate                  pFunGetSampleRate;
pGetDuration                    pFunGetDuration;
pDecode                         pFunDecode;

MP3Decoder::MP3Decoder(qint64 decoderBufferLen,
                       qint64 fileLength) :
    isLoadDllSuccess(false)
{
    if (decoderBufferLen <= 0)
    {
        MP3DecoderDebug("Decoder Buffer is error!");
        return;
    }
    if (fileLength <= 0)
    {
        MP3DecoderDebug("fileLength is error!");
        return;
    }

    this->fileLength = fileLength;
    ConfigHelper *cfg = ConfigHelper::getInstance();
    libMad.setFileName(cfg->getPluginPath() +  MADDLL);

    if(!libMad.load())
    {
        MP3DecoderDebug("Can't Load madApi dll :%s,error str is %s !",
                        QString(cfg->getPluginPath() +  MADDLL).toUtf8().data(),
                        libMad.errorString().toUtf8().data());
        return;
    }

    pFunDecoderInit = (pDecoderInit)libMad.resolve("initDecoder");
    if(pFunDecoderInit == NULL)
    {
        MP3DecoderDebug("Can't link to function: initDecoder() !");
        return;
    }

    pFunDecoderUninit = (pDecoderUninit)libMad.resolve("unInitDecoder");
    if(pFunDecoderUninit == NULL)
    {
        MP3DecoderDebug("Can't link to function: unInitDecoder() !");
        return;
    }

    pFunDecode = (pDecode)libMad.resolve("decode");
    if(pFunDecoderUninit == NULL)
    {
        MP3DecoderDebug("Can't link to function: decode() !");
        return;
    }

    pFunGetBitRate = (pGetBitRate)libMad.resolve("getBitRate");
    if(pFunGetBitRate == NULL)
    {
        MP3DecoderDebug("Can't link to function: getBitRate() !");
        return;
    }

    pFunGetChannels = (pGetChannels)libMad.resolve("getChannels");
    if(pFunGetChannels == NULL)
    {
        MP3DecoderDebug("Can't link to function: getChannels() !");
        return;
    }

    pFunGetSampleRate = (pGetSampleRate)libMad.resolve("getSampleRate");
    if(pFunGetSampleRate == NULL)
    {
        MP3DecoderDebug("Can't link to function: getSampleRate() !");
        return;
    }

    pFunGetDuration = (pGetDuration)libMad.resolve("getDuration");
    if(pFunGetDuration == NULL)
    {
        MP3DecoderDebug("Can't link to function: getDuration() !");
        return;
    }
    MP3DecoderDebug("load libMad succeed!");

    isLoadDllSuccess = true;

    this->decoderBufferLen = decoderBufferLen;
    this->isAudioFormatNotified = false;
    this->isDurationNotified = false;
    this->temporaryPcmData.clear();

    if (decoderBufferLen <= 0)
    {
        MP3DecoderDebug("Decoder buffer length is error!");
        return;
    }
    handle = pFunDecoderInit(decoderBufferLen);
}

QByteArray MP3Decoder::decode(QByteArray mp3Data)
{
    QByteArray data;
    data.clear();
    if (decoderBufferLen <= 0)
    {
        MP3DecoderDebug("Decoder buffer length is error!");
        return data;
    }
    if (! isLoadDllSuccess)
    {
        MP3DecoderDebug("decoder load dll failed!");
        return data;
    }
    if (fileLength <= 0 || mp3Data.isEmpty())
    {
        MP3DecoderDebug("Parameter is not correct!");
        return data;
    }
    int pcmDataLen = 0;
    unsigned char* decodeData = NULL;
    decodeData = pFunDecode(handle,
                            (unsigned char*)mp3Data.data(),
                            mp3Data.length(),
                            0,
                            &pcmDataLen);
//    MP3DecoderDebug("pcmDataLength is %d", pcmDataLen);
    if (pcmDataLen != 0)
    {
        data += QByteArray((char*)decodeData, pcmDataLen);
        if (!data.isEmpty())
        {
            if (isAudioFormatNotified == false)
            {
                int sampleRate = pFunGetSampleRate(handle);
                int bitsPerSample = 16;
                int channels = pFunGetChannels(handle);
                emit onAudioFormat(sampleRate, bitsPerSample, channels);
                isAudioFormatNotified = true;
                temporaryPcmData.append(data);
                data.clear();
            }
            if (isDurationNotified == false)
            {
                float duration = pFunGetDuration(handle, fileLength);
                if (duration != 0.0)
                {
                    emit onDuration(duration);
                    isDurationNotified = true;
                }
            }
            MP3DecoderDebug("pcmDataLength is %d", pcmDataLen);
            return data;
        }
    }
//    MP3DecoderDebug("pcmDataLength is 0");
    return data;
}

bool MP3Decoder::getDecoderState()
{
    if (! isLoadDllSuccess)
    {
        MP3DecoderDebug("decoder load dll failed!");
        return false;
    }
    return isAudioFormatNotified;
}

void MP3Decoder::getAudioFormat(int *sampleRate,
                    int *bitsPerSample,
                    int *channels,
                    int *bitRate)
{
    if (! isLoadDllSuccess)
    {
        MP3DecoderDebug("decoder load dll failed!");
        return;
    }
    *sampleRate = pFunGetSampleRate(handle);
    *bitsPerSample = 16;
    *channels = pFunGetChannels(handle);
    *bitRate = pFunGetBitRate(handle);
}

MP3Decoder::~MP3Decoder()
{
    pFunDecoderUninit(handle);
    libMad.unload();
}
