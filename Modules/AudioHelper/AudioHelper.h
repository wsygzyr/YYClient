#ifndef AUDIOHELPER_H
#define AUDIOHELPER_H

#define AUDIOHELPER_DEBUG 1

#if AUDIOHELPER_DEBUG
#include <QDebug>
#define AudioHelperDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define AudioHelperDebug(format, ...)
#endif

class AudioHelper
{
public:
    static bool AudioInputIsOK();
    static bool AudioOutputIsOK();

private:
    AudioHelper();
    ~AudioHelper();
};

#endif // AUDIOHELPER_H
