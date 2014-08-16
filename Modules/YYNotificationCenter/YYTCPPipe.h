#ifndef YYTCPPIPE_H
#define YYTCPPIPE_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#define YYTCPPIPE_DEBUG 1

#if YYTCPPIPE_DEBUG
#include <QDebug>
#define YYTCPPipeDebug(format, ...) qDebug("%s,LINE:%d--->"format,__FUNCTION__,__LINE__,##__VA_ARGS__)
#else
#define YYTCPPipeDebug(format, ...)
#endif

class YYTCPPipe : public QObject
{
    Q_OBJECT
public:
    explicit YYTCPPipe();
    ~YYTCPPipe();

private:
    QTcpServer *server;
private slots:
    void handleServerNewConnection();

private:
    QTcpSocket *pipe;
private slots:
    void handlePipeReadyRead();
    void handlePipeDisconnected();
    void handlePipeError();

/*
 *  API
 */
signals:
    void onRecvMsg(const QString &cmdName, const QByteArray &cmdData);
    void onConnected();     //if server has a connect, emit this signal,
    void onDisconnected();  //if disconnected,emit this signal

public slots:
    bool startListen(int);                   //start server listen
    void stopListen();
    bool connectToServer(int);   //client connect to server
    void disconnectFromServer();        //disconnected
public:
    bool isConnected(); //judge connect state is connected
    bool sendMsg(const QString &msgName, const QByteArray &msgData);

};

#endif // YYTCPPIPE_H
