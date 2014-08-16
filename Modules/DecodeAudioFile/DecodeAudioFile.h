#ifndef DECODEAUDIOFILE_H
#define DECODEAUDIOFILE_H
#include <QLibrary>
#include <QFile>

#define MADDLL "dll/libMad.dll"
#define DECODE_AUDIO_FILE_DEBUG  1

#if DECODE_AUDIO_FILE_DEBUG
#include <QDebug>
#define DecodeAudioFileDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define DecodeAudioFileDebug(format,...)
#endif



class DecodeAudioFile
{
public:
    DecodeAudioFile();
    bool decode(QString audioFilePath, QString outFilePath);
    bool decode(QByteArray mp3Data, QString outFilePath);

    int getChannel() const;
    void setChannel(int value);

    int getSampleSize() const;
    void setSampleSize(int value);

    int getSampleRate() const;
    void setSampleRate(int value);
    static void WriteWavHeadToFile(QFile &f,int pcmDataSize,int sampleRate,int sampleSize,int channel);
private:
    QLibrary      libDecord;
    int           channel;
    int           sampleSize;
    int           sampleRate;
private:
    bool loadDll();
    void unLoadDll();

};

#endif // DECODEAUDIOFILE_H
