#ifndef SOURCEFILE_H
#define SOURCEFILE_H

#include <QObject>
#include <QFile>
#include "../SourceData.h"
//#include <QMutex>

#define SOURCEFILE_DEBUG 1

#if SOURCEFILE_DEBUG
#include <QDebug>
#define SourceFileDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define SourceFileDebug(format, ...)
#endif

class SourceFile : public SourceData
{
    Q_OBJECT
public:
    SourceFile(QString fileName);
signals:
    void onFileOpenError();
public slots:

private:
    QString                 musicFileName;
    QFile                   musicFile;
    bool                    isOpenError;
//    QMutex                  mutex;
public:
    qint64  getSourceFileLength();
    QByteArray readFileData(qint64 dataLength, qint64 startReatePos, qint64 endReadPos);
    bool getErrorCode();
};

#endif // SOURCEFILE_H
