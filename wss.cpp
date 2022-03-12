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
        qDebug()<<"listening port="+QString::number(port);
        connect(server,SIGNAL(newConnection()),
                this, SLOT(onNewConnection()));
    }else{
        qDebug()<<"wss init fail";
        return false;
    }
    return true;
}
void wss::onNewConnection()//新链接
{
    qDebug();

    QWebSocket * socket(this->server->nextPendingConnection());
    //    connect(socket,&QWebSocket::textMessageReceived,
    //            this, &wss::processTextMessage,
    //            Qt::QueuedConnection);
    connect(socket,&QWebSocket::textMessageReceived,
            this, &wss::processTextMessage);
    connect(socket, &QWebSocket::binaryMessageReceived,
            this, &wss::processBinauyMessage);
    connect(socket,&QWebSocket::disconnected,
            this, &wss::socketDisconnected);
    //qDebug()<<++connectNum;
    qDebug()<<"newConnection"<<QString("%1-%2")
              .arg(socket->peerAddress().toString())
              .arg(socket->peerPort());
    _hashIpPort2PWebSocket.insert(QString("%1-%2")
                                  .arg(socket->peerAddress().toString())
                                  .arg(socket->peerPort())
                                  ,socket);
}
void wss::processTextMessage(const QString & msg)//收到文字消息
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }
    qDebug()<<"new binauy msg:"<<pWebSocket->peerAddress().toString()
            <<pWebSocket->peerPort()
            <<msg;
}
void wss::processBinauyMessage(const QByteArray & msg)
{

    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }
    qDebug()<<"new binauy msg:"<<pWebSocket->peerAddress().toString()
            <<pWebSocket->peerPort()
            <<msg;
}
void wss::slot_discennect()
{

}
void wss::socketDisconnected()//有个链接被关闭
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }
    qDebug()<<"Disconnected:"
            <<pWebSocket->peerAddress().toString()+"-"+QString::number(pWebSocket->peerPort());
    _hashIpPort2PWebSocket.remove(QString("%1-%2")
                                  .arg(pWebSocket->peerAddress().toString())
                                  .arg(QString::number(pWebSocket->peerPort()))
                                  );
    //qDebug()<<--connectNum;
}
void slot_sendText(QString ip, uint16_t port, QString &msg)
{

}
void wss::slot_sendData(QString ip, uint16_t port, QByteArray &buffer)
{
    QString key = QString("%1-%2").arg(ip).arg(port);

}
