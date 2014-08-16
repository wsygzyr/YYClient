#include "WAVDecoder.h"

struct WAVFMT
{
    char			RIFF[4];			/* should be "RIFF" */
    unsigned int   	totalSize;			/* byte_number behind it */
    char			WAVE[4];			/* should be "WAVE" */
    char			FMT[4];				/* should be "fmt " */
    unsigned int	FILTER;				/* should be 0x10 */

    unsigned short	formatTag;			/* should be 1 liner PCM */
    unsigned short  channels;
    unsigned int	sampleRate;
    unsigned int    bytesPerSec;
    unsigned short  bytesInSample;
    unsigned short  bitsDepth;
    char            DATA[4];
    unsigned int    dataSize;
};

WAVDecoder::WAVDecoder() :
    format(NULL)
{
    isAudioFormatNotified = false;
}

WAVDecoder::~WAVDecoder()
{
    delete format;
}

bool WAVDecoder::getDecoderState()
{
    return isAudioFormatNotified;
}

QByteArray WAVDecoder::decode(QByteArray wavData)
{
    
    QByteArray pcmData;
    pcmData.clear();
    if (isAudioFormatNotified)
    {
        WAVDecoderDebug("decode isAudioFormatNotified true");
        return wavData;
    }
    if (wavData.length() < 44)
    {
        WAVDecoderDebug("decode wavData.length() < 44");
        return pcmData;
    }

    //parse wav header
    format = new WAVFMT;
//    format = new format;
    QByteArray headData = wavData.left(44);
    if (wavData.length() > 44)
    {
        qint64 len = wavData.length() - 44;
        temporaryPcmData.append(wavData.right(len));
    }

    memset((char*)format, 0, 44);
    memcpy((char*)format, headData.data(), 44);

    if (format->sampleRate <= 0 ||
            format->bitsDepth <= 0 ||
            format->channels <= 0)
    {
        //emit error
        WAVDecoderDebug("decode format param < 0");
        return pcmData;
    }

    emit onAudioFormat((int)format->sampleRate,
                       (int)format->bitsDepth,
                       (int)format->channels,
                       (int)format->bytesPerSec);
    isAudioFormatNotified = true;
    return pcmData;
}

void WAVDecoder::getAudioFormat(int *sampleRate, int *bitsPerSample, int *channels, int *bitRate)
{
    *sampleRate = (int)format->sampleRate;
    *bitsPerSample = (int)format->bitsDepth;
    *channels = (int)format->channels;
    *bitRate = (int)format->bytesPerSec*8;
}
