#include "Decoder.h"

Decoder::Decoder(QObject *parent) :
    QObject(parent)
{
    temporaryPcmData.clear();
}
