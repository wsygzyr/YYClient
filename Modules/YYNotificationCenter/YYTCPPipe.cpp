#include "YYTCPPipe.h"

#include <QThread>
#include <string.h>
#include <QTime>
#define MAX_PIPE_MSG_LEN 1024*10
typedef struct PipeMsg
{
    char name[32];
    int dataLen;
    char data[MAX_PIPE_MSG_LEN];
}PipeMsg;

YYTCPPipe::YYTCPPipe()
{
    server = NULL;
    pipe = NULL;

}

YYTCPPipe::~YYTCPPipe()
{

}


void YYTCPPipe::handleServerNewConnection()
{
    YYTCPPipeDebug("new connection!");
    pipe = server->nextPendingConnection();

    QObject::connect(pipe, SIGNAL(readyRead()), this, SLOT(handlePipeReadyRead()));
    QObject::connect(pipe, SIGNAL(disconnected()), this, SLOT(handlePipeDisconnected()));
    QObject::connect(pipe, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handlePipeError()));

    emit onConnected();
}

void YYTCPPipe::handlePipeReadyRead()
{
//    YYTCPPipeDebug("onReadyRead! current thread is: %s", QThread::currentThread()->objectName().toUtf8().data());

    for (;;)
    {
        PipeMsg pkt = {0};

        int len = pipe->bytesAvailable();
 //       YYTCPPipeDebug("len is: %d", len);
        if (len < sizeof(pkt.name) + sizeof(pkt.dataLen))
        {
    //        YYTCPPipeDebug("not enough data!");
            break;
        }

        pipe->peek((char *)&pkt, sizeof(pkt));
//        YYTCPPipeDebug("pkt.name is: %s, pkt.dataLen is: %d", pkt.name, pkt.dataLen);

        if (len < sizeof(pkt.name) + sizeof(pkt.dataLen) + pkt.dataLen)
        {
            YYTCPPipeDebug("pkt.data is not ready!");
            break;
        }

        pipe->read((char *)&pkt, sizeof(pkt.name) + sizeof(pkt.dataLen) + pkt.dataLen);

        YYTCPPipeDebug("pkt.name is: %s, pkt.dataLen is: %d, pkt.data is: %s",
                     pkt.name, pkt.dataLen, pkt.data);

        QString msgName = QString(pkt.name);
        QByteArray msgData = QByteArray(pkt.data, pkt.dataLen);

        emit onRecvMsg(msgName,msgData);
    }
}

void YYTCPPipe::handlePipeDisconnected()
{
    YYTCPPipeDebug("disconnected!");

    emit onDisconnected();
}

void YYTCPPipe::handlePipeError()
{
    YYTCPPipeDebug("error! errorString is: %s", pipe->errorString().toUtf8().data());

    emit onDisconnected();
}

bool YYTCPPipe::isConnected()
{
    if (!pipe)
    {
        return false;
    }

    if (pipe->state() != QTcpSocket::ConnectedState)
    {
        return false;
    }

    return true;
}

bool YYTCPPipe::startListen(int port)
{
    YYTCPPipeDebug("YYTCPPipe startListen!");
    if (server != NULL)
    {
        YYTCPPipeDebug("server is busy!");
        return false;
    }

    server = new QTcpServer;
    QObject::connect(server, SIGNAL(newConnection()), this, SLOT(handleServerNewConnection()));

    if (!server->listen(QHostAddress::LocalHost, port))
    {
        YYTCPPipeDebug("Failed to bind to port");
        server->deleteLater();
        server = NULL;
        return false;
    }

    YYTCPPipeDebug("start listen succeed!");

    return true;
}

void YYTCPPipe::stopListen()
{
    YYTCPPipeDebug("YYTCPPipe stopListen!");
    if (server == NULL)
    {
        return;
    }
    server->close();
    server->deleteLater();
    server = NULL;
}

bool YYTCPPipe::connectToServer(int port)
{
    YYTCPPipeDebug("YYTCPPipe connectToServer!");
    if (pipe != NULL)
    {
        YYTCPPipeDebug("pipe is busy!");
        return false;
    }

    pipe = new QTcpSocket;
//    QObject::connect(pipe, SIGNAL(readyRead()), this, SLOT(handlePipeReadyRead()));


    pipe->connectToHost(QHostAddress::LocalHost,port);
    if (!pipe->waitForConnected(250))
    {
        YYTCPPipeDebug("connect failed!");
        pipe->deleteLater();
        pipe = NULL;
        return false;
    }

    YYTCPPipeDebug("connect succeed!");
    QObject::connect(pipe, SIGNAL(readyRead()), this, SLOT(handlePipeReadyRead()));
    QObject::connect(pipe, SIGNAL(disconnected()), this, SLOT(handlePipeDisconnected()));
    QObject::connect(pipe, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handlePipeError()));

    return true;
}

void YYTCPPipe::disconnectFromServer()
{
    YYTCPPipeDebug("YYTCPPipe disconnect");

    if (pipe == NULL)
    {
        return;
    }

    if (pipe->state() == QTcpSocket::ConnectedState)
    {
        pipe->close();
    }

    pipe->deleteLater();
    pipe = NULL;
}

bool YYTCPPipe::sendMsg(const QString &msgName, const QByteArray &msgData)
{
    YYTCPPipeDebug("sendMsg! current thread is: %s", QThread::currentThread()->objectName().toUtf8().data());

    if (pipe == NULL)
    {
        YYTCPPipeDebug("pipe is NULL!");
        return false;
    }

    if (pipe->state() != QTcpSocket::ConnectedState)
    {
        YYTCPPipeDebug("the tcpPipe has not make a connection, send failed!");
        return false;
    }

    PipeMsg pkt = {0};
    strcpy_s(pkt.name, sizeof(pkt.name), msgName.toUtf8().data());
    pkt.dataLen = msgData.length();
    memcpy(pkt.data, msgData.data(), msgData.length());

    YYTCPPipeDebug("pkt.name is : %s, pkt.dataLen is: %d ,pkt.data is %s",
                 pkt.name, pkt.dataLen,pkt.data);

    pipe->write((const char*)&pkt, sizeof(pkt.name) + sizeof(pkt.dataLen) + pkt.dataLen);

    if (!pipe->waitForBytesWritten(50))
    {
        YYTCPPipeDebug("send message failed! errorString is: %s", pipe->errorString().toUtf8().data());
        return false;
    }

    YYTCPPipeDebug("send message succeed!");

    return true;
}
