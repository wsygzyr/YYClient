#include "FileCut.h"
#include <QFile>
#include "Modules/DecodeAudioFile/DecodeAudioFile.h"
FileCut::FileCut(QObject *parent) :
    QObject(parent)
{
}

void FileCut::setFormat(int sampleRate, int sampleChannel, int sampleBit)
{
    this->sampleBit = sampleBit;
    this->sampleChannel = sampleChannel;
    this->sampleRate = sampleRate;
}

bool FileCut::splitFile(QString inFileName, QString outFileName, float startTime, float endTime)
{
    QFile *inFile = new QFile(inFileName, this);
    if( !inFile->exists() )
    {
        FileCutDebug("%s isn't exists...",inFileName.toUtf8().data());
        return false;
    }
    if( !inFile->open(QIODevice::ReadOnly) )
    {
        FileCutDebug("%s can't open...",inFileName.toUtf8().data());
        return false;
    }

    qint64 posStart = startTime*this->sampleBit*this->sampleChannel*this->sampleRate/1000/8;
    qint64 posEnd = endTime*this->sampleBit*this->sampleChannel*this->sampleRate/1000/8;

    posStart = posStart - posStart % 4;
    posEnd = posEnd - posEnd % 4;

    if( !inFile->seek(posStart) )
    {
        FileCutDebug("%s can't seek...",inFileName.toUtf8().data());
        inFile->close();
        return false;
    }

    char *buf = (char*)malloc(posEnd - posStart);

    qint64 realLen = inFile->read(buf,posEnd - posStart );

    inFile->close();

    if( realLen != (posEnd - posStart) )
    {
         FileCutDebug("read data isn't equal real len...");
         return false;
    }

    QFile *outFile = new QFile(outFileName,this);
    if( !outFile->open(QIODevice::WriteOnly) )
    {
        FileCutDebug("%s can't open...",inFileName.toUtf8().data());
        return false;
    }
    char *out = (char*)malloc((posEnd-posStart)/2);
    for(int i = 0; i < ((posEnd-posStart)/2);)
    {
        out[i] = buf[i*2];
        out[i+1] = buf[i*2+1];
        i += 2;
    }
    outFile->write(out,(posEnd-posStart)/2);
    outFile->close();
    free(buf);
    free(out);
    return true;
}

