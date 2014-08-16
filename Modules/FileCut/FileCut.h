#ifndef FILECUT_H
#define FILECUT_H

#include <QObject>
#include <QFile>
#define DEBUG_FILE_CUT   1

#if DEBUG_FILE_CUT
#define FileCutDebug(format,...) qDebug("%s, LINE: %d --->"format,__FUNCTION__, __LINE__, ##__VA_ARGS__)
#include <QDebug>
#else
#define FileCutDebug(format,...)
#endif

class FileCut : public QObject
{
    Q_OBJECT
public:
    explicit FileCut(QObject *parent = 0);
    void setFormat( int sampleRate, int sampleChannel, int sampleBit);
    bool splitFile(QString inFileName, QString outFileName, float startTime, float endTime);
private:
    int sampleRate;
    int sampleChannel;
    int sampleBit;
signals:

public slots:

};

#endif // FILECUT_H
