#include "Node.h"

#include <QThread>
#include <string.h>

#include <QTime>

Node::Node()
{
    server = NULL;
    socket = NULL;

}

Node::~Node()
{
    NodeDebug("~Node(), objectName is: %s", this->objectName().toUtf8().data());
}

void Node::handleServerNewConnection()
{
    NodeDebug("new connection!");
    if (socket)
    {
        NodeDebug("socket is busy!");
        return;
    }

    socket = server->nextPendingConnection();

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(handleSocketReadyRead()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(handleSocketDisconnected()));

    emit onConnected();
}

#include <string.h>
void Node::handleSocketReadyRead()
{
    NodeDebug("onReadyRead! current thread is: %s", QThread::currentThread()->objectName().toUtf8().data());

    for (;;)
    {
        if (!socket)
        {
            break;
        }
        int len = socket->bytesAvailable();
        if (len <= 0)
        {
            NodeDebug("len <= 0");            
            return;
        }
//        NodeDebug("len is: %d", len);

        char *buf = (char *)malloc(sizeof(char) * len);
        if (!buf)
        {
            NodeDebug("no memory!");
            return;
        }

        socket->peek(buf, len);

        char *p = strchr(buf, '\n');
        if (!p)
        {
            NodeDebug("pkt.data is not ready!");
            free(buf);
            break;
        }

        len = p - buf + 1;
        socket->read(buf, len);

        QByteArray msgData = QByteArray(buf, len);
        free(buf);
        NodeDebug("msgData is: %s", msgData.data());

        emit onRecvMsg(msgData);
    }
}

void Node::handleSocketDisconnected()
{
    NodeDebug("disconnected!");

    if (socket)
    {
        socket->deleteLater();
        socket = NULL;
    }

    NodeDebug("...");

    emit onDisconnected();

    NodeDebug("...");
}

void Node::handleSocketError()
{
    NodeDebug("error! errorString is: %s", socket->errorString().toUtf8().data());

    if (socket)
    {
        socket->deleteLater();
        socket = NULL;
    }

    emit onDisconnected();
}

bool Node::isConnected()
{
    if (!socket)
    {
        return false;
    }

    if (socket->state() != QTcpSocket::ConnectedState)
    {
        return false;
    }

    return true;
}

bool Node::startListen(int port)
{
    if (server != NULL)
    {
        NodeDebug("server is busy!");
        return false;
    }

    server = new QTcpServer;
    QObject::connect(server, SIGNAL(newConnection()), this, SLOT(handleServerNewConnection()));

    if (!server->listen(QHostAddress::Any, port))
    {
        NodeDebug("Failed to bind to port");
        server->deleteLater();
        server = NULL;
        return false;
    }

    NodeDebug("start listen succeed!");

    return true;
}

void Node::stopListen()
{
    if (server == NULL)
    {
        return;
    }
    NodeDebug("stop listening");
    server->close();
    server->deleteLater();
    server = NULL;

}

bool Node::connectToServer(QString IP, int port)
{
    if (socket != NULL)
    {
        NodeDebug("sock is busy!");
        return false;
    }

    socket = new QTcpSocket;
//    socket->connectToHost(QHostAddress::LocalHost,port);
    socket->connectToHost(QHostAddress(IP), port);
    if (!socket->waitForConnected(3000))
    {
        NodeDebug("connect failed!");
        socket->deleteLater();
        socket = NULL;
        return false;
    }

    NodeDebug("connect succeed!");
    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(handleSocketReadyRead()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(handleSocketDisconnected()));
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError()));

    return true;
}

bool Node::connectToServer(QString IP, int port, bool isNeedReconn, int reconnTimes)
{
    if (socket != NULL)
    {
        NodeDebug("sock is busy!");
        return false;
    }
    bool isSucceed = false;
    int cnt = 0;
    int tmp = reconnTimes;
    socket = new QTcpSocket;
    while(tmp-- > 0)
    {
        ++cnt;
        socket->connectToHost(QHostAddress(IP), port);
        if (!socket->waitForConnected(3000))
        {
            NodeDebug("connect %d times failed!", cnt);
            isSucceed = false;
            if(!isNeedReconn)
            {
                NodeDebug("connect server failed, No need reconnect, exit !");
                break;
            }
        }
        else
        {
            isSucceed = true;
            break;
        }
        //sleep 1 sec
        QTime t;
        t.start();
        while(t.elapsed()<1000)
        {
            QCoreApplication::processEvents();
        }
    }

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(handleSocketReadyRead()));
    QObject::connect(socket, SIGNAL(disconnected()), this, SLOT(handleSocketDisconnected()));
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError()));

    if(!isSucceed)
    {
        NodeDebug("connect %d  failed, exit connect!", reconnTimes);
        emit onConnectResult(1);
        socket->deleteLater();
        socket = NULL;
        return false;
    }
    else
    {
        emit onConnectResult(0);
        NodeDebug("connect server succeed");
    }
    return true;
}

void Node::disconnectFromServer()
{
    NodeDebug("disconnect");

    if (socket == NULL)
    {
        return;
    }

    if (socket->state() == QTcpSocket::ConnectedState)
    {
        QObject::disconnect(socket, SIGNAL(readyRead()), this, SLOT(handleSocketReadyRead()));
        QObject::disconnect(socket, SIGNAL(disconnected()), this, SLOT(handleSocketDisconnected()));
        QObject::disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError()));
        socket->close();
    }

    socket->deleteLater();
    socket = NULL;

    NodeDebug("disconnected");
}

bool Node::sendMsg(const QByteArray &msgData)
{
    NodeDebug("sendMsg! msgData is: %s", msgData.data());

    if (socket == NULL)
    {
        NodeDebug("sendMsg! socket == NULL");
        return false;
    }

    if (socket->state() != QTcpSocket::ConnectedState)
    {
        NodeDebug("the tcpsock has not make a connection, send failed!");
        return false;
    }
//    socket->bytesAvailable();

    if(socket->isWritable())
    {
       NodeDebug("QIODevice can be writen");
    }
    else
    {
        NodeDebug("QIODevice can not be writen");
    }

    QByteArray sendData = msgData;
    sendData.append("\n");
    socket->write(sendData.data(), sendData.length());
//    socket->write("\n", 1);
    if (!socket->waitForBytesWritten(250))
    {
        NodeDebug("send message failed, msg=%s! errorString is: %s", socket->errorString().toUtf8().data(), sendData.data());
        return false;
    }
    socket->flush();

    NodeDebug("send message succeed!");

    return true;
}
