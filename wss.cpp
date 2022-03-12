#include "wss.h"
wss::wss()
{

}
wss::~wss()
{
    server->close();
    if(server)delete server;
    qDebug()<<"wss"<<port<<"isclose";
}
bool wss::init(uint16_t port)
{
    this->port=port;
    server = new QWebSocketServer("server",QWebSocketServer::NonSecureMode,this);
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
    connect(socket,&QWebSocket::textMessageReceived,
            this, &wss::processTextMessage);
    connect(socket, &QWebSocket::binaryMessageReceived,
            this, &wss::processBinauyMessage);
    connect(socket,&QWebSocket::disconnected,
            this, &wss::socketDisconnected);
    qDebug()<<"newConnection"<<QString("%1-%2")
              .arg(socket->peerAddress().toString())
              .arg(socket->peerPort());
    ++connections;
    _hashIpPort2PWebSocket.insert(QString("%1-%2")
                                  .arg(socket->peerAddress().toString())
                                  .arg(socket->peerPort())
                                  ,socket);
    qDebug()<<connections<<_hashIpPort2PWebSocket.size();
}
void wss::processTextMessage(const QString & msg)//收到文字数据
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }
    qDebug()<<"new binauy msg:"<<pWebSocket->peerAddress().toString()
            <<pWebSocket->peerPort()
            <<msg;
    emit signal_processTextMessage(pWebSocket->peerAddress().toString()
                                   ,pWebSocket->peerPort()
                                   ,msg);
    return;
    //复读机
    QString rmsg="rap"+msg;
    this->slot_sendText(pWebSocket->peerAddress().toString()
                  ,pWebSocket->peerPort()
                  ,rmsg);
}
void wss::processBinauyMessage(const QByteArray & msg)//收到二进制数据
{

    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }
    qDebug()<<"new binauy msg:"<<pWebSocket->peerAddress().toString()
            <<pWebSocket->peerPort()
            <<msg;
    emit signal_processBinauyMessage(pWebSocket->peerAddress().toString()
                                         ,pWebSocket->peerPort()
                                         ,msg);
    //return;
    //复读机
    QByteArray rmsg=msg;
    this->slot_sendData(pWebSocket->peerAddress().toString()
                  ,pWebSocket->peerPort()
                  ,rmsg);
}
int wss::slot_sendText(QString ip, quint16 port, QString msg)
{
    QString key = QString("%1-%2").arg(ip).arg(port);
    if(_hashIpPort2PWebSocket.contains(key))
    {
        qint64 size=_hashIpPort2PWebSocket.value(key)->sendTextMessage(msg);
        qDebug()<<"发送了"<<size<<"大小的数据";
        return 0;
    }else{
        qDebug()<<"rep text fail";
    }
    return -1;
}
int wss::slot_sendData(QString ip, quint16 port, QByteArray msg)
{
    QString key = QString("%1-%2").arg(ip).arg(port);
    if(_hashIpPort2PWebSocket.contains(key))
    {
        qint64 size=_hashIpPort2PWebSocket.value(key)->sendBinaryMessage(msg);
        qDebug()<<"发送了"<<size<<"大小的数据";
        return 0;
    }else{
        qDebug()<<"rep binary fail";
    }
    return -1;
}
void wss::slot_discennect(QString ip, quint16 port)
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
    --connections;
    qDebug()<<connections<<_hashIpPort2PWebSocket.size();
    emit signal_pDiscennect(pWebSocket->peerAddress().toString()
                            ,pWebSocket->peerPort());
}
