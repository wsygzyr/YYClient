#ifndef SOURCERAM_H
#define SOURCERAM_H

#include <QObject>
#include <QByteArray>
#include <QMutex>
#include "../SourceData.h"

#define SOURCERAM_DEBUG 1

#if SOURCERAM_DEBUG
#include <QDebug>
#define SourceRAMDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define SourceRAMDebug(format, ...)
#endif

class SourceRAM : public SourceData
{
    Q_OBJECT
public:
    explicit SourceRAM();

signals:

public slots:

private:
    QByteArray          RAMData;
    QMutex              mutex;
public:
    QByteArray readFileData(qint64 dataLength,
                           qint64 startReatePos = 0,
                           qint64 endReadPos = 0);
    void writeFileData(QByteArray data);
};

#endif // SOURCERAM_H
