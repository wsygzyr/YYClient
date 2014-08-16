#ifndef SOURCEDATA_H
#define SOURCEDATA_H

#include <QObject>

class SourceData : public QObject
{
    Q_OBJECT
public:
    explicit SourceData(QObject *parent = 0);

signals:

public slots:

public:
    virtual qint64  getSourceFileLength();
    virtual QByteArray readFileData(qint64 dataLength,
                                   qint64 startReatePos,
                                   qint64 endReadPos);
    virtual bool getErrorCode();
};

#endif // SOURCEDATA_H
