#include "SourceRAM.h"

SourceRAM::SourceRAM()
{
}

QByteArray SourceRAM::readFileData(qint64 dataLength,
                                  qint64 startReatePos,
                                  qint64 endReadPos)
{
    mutex.lock();
    QByteArray data;
    data.clear();
    if (RAMData.isEmpty())
    {
        mutex.unlock();
        return data;
    }
    qint64 len = qMin(dataLength, (qint64)RAMData.length());
    data = RAMData.left(len);
    RAMData.remove(0, len);
    mutex.unlock();
    return data;
}

void SourceRAM::writeFileData(QByteArray data)
{
    mutex.lock();
    RAMData.append(data);
    mutex.unlock();

}

