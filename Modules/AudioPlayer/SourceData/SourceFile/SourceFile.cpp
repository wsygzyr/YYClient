#include "SourceFile.h"

SourceFile::SourceFile(QString fileName)
{
    musicFileName = fileName;
    if (musicFileName.isEmpty())
    {
        SourceFileDebug("Music FileName is empty!");
        return;
    }
    isOpenError = false;
    musicFile.setFileName(musicFileName);
}

qint64 SourceFile::getSourceFileLength()
{
    if (!musicFile.open(QFile::ReadOnly))
    {
        SourceFileDebug("The file cannot be opened : %s",musicFileName.toLocal8Bit().data());
        isOpenError = true;
        return 0;
    }
    qint64 fileLength = musicFile.size();
    musicFile.close();
    return fileLength;
}

QByteArray SourceFile::readFileData(qint64 dataLength, qint64 startReatePos, qint64 endReadPos)
{
    QByteArray data;
    data.clear();
    if (dataLength <= 0 ||
            startReatePos < 0 ||
            endReadPos <= 0 ||
            startReatePos >= endReadPos)
    {
        SourceFileDebug("parameters error!");
        return data;
    }
    qint64 len = qMin(dataLength, endReadPos - startReatePos);
    if (!musicFile.open(QFile::ReadOnly))
    {
        //SourceFileDebug("The file cannot be opened : %s",musicFileName.toLocal8Bit().data());
        isOpenError = true;
        return data;
    }
    musicFile.seek(startReatePos);
    data = musicFile.read(len);
//    SourceFileDebug("read data length is %d", data.length());
    musicFile.close();
    return data;
}

bool SourceFile::getErrorCode()
{
    return isOpenError;
}
