#include "DecodeAudioFile.h"
#include "ConfigHelper/ConfigHelper.h"
//Define DLL function pointer
typedef int (*pFileDecoderInit)(int bufLen);
typedef int (*pFileDecoderUninit)(int handle);
typedef int (*pFileGetBitRate)(int handle);
typedef int (*pFileGetChannels)(int handle);
typedef int (*pFileGetSampleRate)(int handle);
typedef float (*pFileGetDuration)(int handle,int fileLen) ;
typedef unsigned char* (*pFileDecode)(int handle,
                                            const unsigned char* in,
                                            int inLen,
                                            int eof,
                                            int *outLen);

pFileDecoderInit                    pSEFunDecoderInit;
pFileDecoderUninit                  pSEFunDecoderUninit;
pFileGetBitRate                     pSEFunGetBitRate;
pFileGetChannels                    pSEFunGetChannels;
pFileGetSampleRate                  pSEFunGetSampleRate;
pFileGetDuration                    pSEFunGetDuration;
pFileDecode                         pSEFunDecode;

struct WAVFMT
{
    char			RIFF[4];			/* should be "RIFF" */
    unsigned int   	totalSize;			/* byte_number behind it */
    char			WAVE[4];			/* should be "WAVE" */
    char			FMT[4];				/* should be "fmt " */
    unsigned int	FILTER;				/* should be 0x10 */

    unsigned short	formatTag;      		/* should be 1 liner PCM */
    unsigned short  channels;
    unsigned int	sampleRate;
    unsigned int    bytesPerSec;
    unsigned short  bytesInSample;
    unsigned short  bitsDepth;
    char            DATA[4];
    unsigned int    dataSize;
};

DecodeAudioFile::DecodeAudioFile()
{
}

bool DecodeAudioFile::decode(QString audioFilePath, QString outFilePath)
{
    if (loadDll() == false)
    {
        DecodeAudioFileDebug("DecodeAudioFile decode loadDll() == false");
        return false;
    }
    // decode from file infile, put data to outfile
    DecodeAudioFileDebug("decodeMp3File(%s,%s)...",audioFilePath.toUtf8().data(), outFilePath.toUtf8().data());
    QFile mp3InFile(audioFilePath);
    if(!mp3InFile.open(QIODevice::ReadOnly))
    {
        DecodeAudioFileDebug("open mp3 file failed:%s",audioFilePath.toUtf8().data());
        return false;
    }

    if (outFilePath != NULL)
    {
        QFile::remove(outFilePath);
    }

    QFile pcmFile(outFilePath);
    if(!pcmFile.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        DecodeAudioFileDebug("open out file failed:%s", outFilePath.toUtf8().data());
        return false;
    }

    //write wav head info,put into mp3InFile
    QByteArray mp3Data = mp3InFile.readAll();
    DecodeAudioFileDebug("mp3Data size is:%d", mp3Data.size());
    int decorderHandle = pSEFunDecoderInit(mp3Data.size()*15);

    int outLen = 0;
    unsigned char *pcmData = pSEFunDecode(decorderHandle, (unsigned char*)mp3Data.data(), mp3Data.size(), 0, &outLen);
    DecodeAudioFileDebug("decorderHandle  is @%d ,pcmData is: @%p",decorderHandle ,  pcmData);
    if (NULL == pcmData)
    {
        DecodeAudioFileDebug("error: pcmData is NULL!");
        return false;
    }

    this->channel = pSEFunGetChannels(decorderHandle);
    this->sampleSize = 16;
    this->sampleRate = pSEFunGetSampleRate(decorderHandle);
    DecodeAudioFileDebug("mp3SampleRate is:%d",sampleRate);

//    pcm2wav(pcmFile,outLen,this->sampleRate,this->channel,this->sampleSize);
    WriteWavHeadToFile(pcmFile,outLen,this->sampleRate,this->sampleSize,this->channel);
    DecodeAudioFileDebug("this->sampleRate:%d,this->channel:%d,this->sampleSize:%d",this->sampleRate,this->channel,this->sampleSize);

    pcmFile.write((char*)pcmData,outLen);

    pcmFile.close();

    pSEFunDecoderUninit(decorderHandle);
    unLoadDll();
    return true;
}

bool DecodeAudioFile::decode(QByteArray mp3Data, QString outFilePath)
{
    if (loadDll() == false)
    {
        DecodeAudioFileDebug("DecodeAudioFile decode loadDll() == false");
        return false;
    }
    QFile pcmFile(outFilePath);
    if(!pcmFile.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        DecodeAudioFileDebug("open out file failed:%s", outFilePath.toUtf8().data());
        return false;
    }

    //write wav head info,put into mp3InFile

    int decorderHandle = pSEFunDecoderInit(mp3Data.size()*15);

    int outLen = 0;
    unsigned char *pcmData = pSEFunDecode(decorderHandle, (unsigned char*)mp3Data.data(), mp3Data.size(), 0, &outLen);
    DecodeAudioFileDebug("decorderHandle  is @%d ,pcmData is: @%p",decorderHandle ,  pcmData);
    if (NULL == pcmData)
    {
        DecodeAudioFileDebug("error: pcmData is NULL!");
        return false;
    }

    this->channel = pSEFunGetChannels(decorderHandle);
    this->sampleSize = 16;
    this->sampleRate = pSEFunGetSampleRate(decorderHandle);
    DecodeAudioFileDebug("mp3SampleRate is:%d",sampleRate);

//    pcm2wav(pcmFile,outLen,this->sampleRate,this->channel,this->sampleSize);
   // WriteWavHeadToFile(pcmFile,outLen,this->sampleRate,this->sampleSize,this->channel);
    DecodeAudioFileDebug("this->sampleRate:%d,this->channel:%d,this->sampleSize:%d",this->sampleRate,this->channel,this->sampleSize);

    pcmFile.write((char*)pcmData,outLen);

    pcmFile.close();

    pSEFunDecoderUninit(decorderHandle);
    unLoadDll();
    return true;
}


int DecodeAudioFile::getChannel() const
{
    return channel;
}

void DecodeAudioFile::setChannel(int value)
{
    channel = value;
}
int DecodeAudioFile::getSampleSize() const
{
    return sampleSize;
}

void DecodeAudioFile::setSampleSize(int value)
{
    sampleSize = value;
}
int DecodeAudioFile::getSampleRate() const
{
    return sampleRate;
}

void DecodeAudioFile::setSampleRate(int value)
{
    sampleRate = value;
}



bool DecodeAudioFile::loadDll()
{
    ConfigHelper *cfg = ConfigHelper::getInstance();
    libDecord.setFileName(cfg->getPluginPath() +  MADDLL);

    if(!libDecord.load())
    {
        DecodeAudioFileDebug("Can't Load decode dll :%s ,error is %s!",
                             QString(cfg->getPluginPath() +  MADDLL).toUtf8().data(),
                             libDecord.errorString().toUtf8().data());
        return false;
    }

    pSEFunDecoderInit = (pFileDecoderInit)libDecord.resolve("initDecoder");
    if(pSEFunDecoderInit == NULL)
    {
        DecodeAudioFileDebug("Can't link to function: initDecoder() !");
        return false;
    }

    pSEFunDecoderUninit = (pFileDecoderUninit)libDecord.resolve("unInitDecoder");
    if(pSEFunDecoderUninit == NULL)
    {
        DecodeAudioFileDebug("Can't link to function: unInitDecoder() !");
        return false;
    }

    pSEFunDecode = (pFileDecode)libDecord.resolve("decode");
    if(pSEFunDecoderUninit == NULL)
    {
        DecodeAudioFileDebug("Can't link to function: decode() !");
        return false;
    }

    pSEFunGetBitRate = (pFileGetBitRate)libDecord.resolve("getBitRate");
    if(pSEFunGetBitRate == NULL)
    {
        DecodeAudioFileDebug("Can't link to function: getBitRate() !");
        return false;
    }

    pSEFunGetChannels = (pFileGetChannels)libDecord.resolve("getChannels");
    if(pSEFunGetChannels == NULL)
    {
        DecodeAudioFileDebug("Can't link to function: getChannels() !");
        return false;
    }

    pSEFunGetSampleRate = (pFileGetSampleRate)libDecord.resolve("getSampleRate");
    if(pSEFunGetSampleRate == NULL)
    {
        DecodeAudioFileDebug("Can't link to function: getSampleRate() !");
        return false;
    }

    pSEFunGetDuration = (pFileGetDuration)libDecord.resolve("getDuration");
    if(pSEFunGetDuration == NULL)
    {
        DecodeAudioFileDebug("Can't link to function: getDuration() !");
        return false;
    }
    DecodeAudioFileDebug("load decode dll succeed!");
    return true;
}

void DecodeAudioFile::unLoadDll()
{
    libDecord.unload();
}

void DecodeAudioFile::WriteWavHeadToFile(QFile &f,int outLen, int sampleRate, int sampleSize, int channel)
{
    WAVFMT wavHeader ;
    wavHeader.RIFF[0] = 'R';
    wavHeader.RIFF[1] = 'I';
    wavHeader.RIFF[2] = 'F';
    wavHeader.RIFF[3] = 'F';

    wavHeader.totalSize = 44 + outLen;

    wavHeader.WAVE[0] = 'W';
    wavHeader.WAVE[1] = 'A';
    wavHeader.WAVE[2] = 'V';
    wavHeader.WAVE[3] = 'E';

    wavHeader.FMT[0] = 'f';
    wavHeader.FMT[1] = 'm';
    wavHeader.FMT[2] = 't';
    wavHeader.FMT[3] = ' ';

    wavHeader.FILTER = 0x00000010;
    wavHeader.formatTag = 1;
    wavHeader.channels = channel;
    wavHeader.sampleRate = sampleRate;
    wavHeader.bytesPerSec = sampleRate*sampleSize*channel/8;
    wavHeader.bytesInSample = sampleSize/8;
    wavHeader.bitsDepth = sampleSize;

    wavHeader.DATA[0] = 'd';
    wavHeader.DATA[1] = 'a';
    wavHeader.DATA[2] = 't';
    wavHeader.DATA[3] = 'a';

    wavHeader.dataSize = outLen;
    int pos = f.pos();
    f.seek(0);
    f.write((char*)&wavHeader,sizeof(WAVFMT));
    f.seek(pos);
    f.flush();
}
