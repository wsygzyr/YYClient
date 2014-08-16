#include "SourceData.h"

SourceData::SourceData(QObject *parent) :
    QObject(parent)
{
}

qint64 SourceData::getSourceFileLength()
{
    return Q_INT64_C(9223372036854775807);
}

QByteArray SourceData::readFileData(qint64 dataLength,
                                   qint64 startReatePos,
                                   qint64 endReadPos)
{
    QByteArray data;
    data.clear();
    return data;
}

bool SourceData::getErrorCode()
{
    return false;
}
