#include <QAudioOutput>
#include <QAudioInput>

#include "AudioHelper/AudioHelper.h"

bool AudioHelper::AudioInputIsOK()
{
    QAudioFormat format;
    format.setFrequency(22050);
    format.setChannels(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultInputDevice());
    if (!info.isFormatSupported(format)) {
        AudioHelperDebug("no default input device");
        return false;
    }
    return true;
}

bool AudioHelper::AudioOutputIsOK()
{
    QAudioFormat format;
    format.setFrequency(22050);
    format.setChannels(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        AudioHelperDebug("no default output device");
        return false;
    }
    return true;
}
