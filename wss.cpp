#include "wss.h"
wss::wss()
{

}
wss::~wss()
{
    qDebug()<<"wss"<<port<<"isclose";
}
bool wss::init(uint16_t port)
{
    this->port=port;
    server =new QWebSocketServer("server",QWebSocketServer::NonSecureMode,this);
    if(server->listen(QHostAddress::Any, port))
    {
        connect(server,SIGNAL(newConnection()),
                this, SLOT(onNewConnection()));




    }else{
        qDebug()<<"wss init fail";
        return false;
    }
    return true;
}
void wss::onNewConnection()
{
    qDebug()<<"newConnection";
    QWebSocket * socket(this->server->nextPendingConnection());
    connect(socket,&QWebSocket::textMessageReceived,
            this, &wss::processTextMessage);
//    connect(socket,&QWebSocket::textMessageReceived,
//            this, &wss::processTextMessage,
//            Qt::QueuedConnection);
    connect(socket,&QWebSocket::disconnected,
            this, &wss::socketDisconnected);
    //this->clients.append(socket);//当前链接列表QList
    //qDebug()<<clients.size();
    qDebug()<<connectNum++;
}
void wss::processTextMessage(const QString & msg)
{
    qDebug()<<"new text msg:"<<msg;
}
void wss::socketDisconnected()
{
    qDebug()<<"Connection is disconnected";
    qDebug()<<connectNum--;
}
