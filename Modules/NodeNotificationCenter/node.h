#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtCore>

#define NODE_DEBUG 1

#if NODE_DEBUG
#include <QDebug>
#define NodeDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define NodeDebug(format, ...)
#endif

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node();
    ~Node();

private:
    QTcpServer *server;
private slots:
    void handleServerNewConnection();

private:
    QTcpSocket *socket;
private slots:
    void handleSocketReadyRead();
    void handleSocketDisconnected();
    void handleSocketError();

/*
 *  API
 */
signals:
    void onRecvMsg(const QByteArray &msgData);
    void onConnected();     //if server has a connect, emit this signal,
    void onDisconnected();  //if disconnected,emit this signal
    void onConnectResult(int recode);  //0:succeed, 1:failed

public slots:
    bool startListen(int port);                   //start server listen
    void stopListen();
    bool connectToServer(QString IP, int port);   //client connect to server
    bool connectToServer(QString IP, int port, bool isNeedReconn, int reconnTimes);//overload with reconnect
    void disconnectFromServer();        //disconnected
public:
    bool isConnected(); //judge connect state is connected
    bool sendMsg(const QByteArray &msgData);
};

#endif // YYTCPPIPE_H
