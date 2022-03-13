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

    QWebSocket * pWebSocket(this->server->nextPendingConnection());
    QString key=QString("%1-%2")
            .arg(pWebSocket->peerAddress().toString())
            .arg(pWebSocket->peerPort());    
    connect(pWebSocket,&QWebSocket::textMessageReceived,
            this, &wss::processTextMessage);
    connect(pWebSocket, &QWebSocket::binaryMessageReceived,
            this, &wss::processBinauyMessage);
    connect(pWebSocket,&QWebSocket::disconnected,
            this, &wss::socketDisconnected);
    qDebug()<<"newConnection"<<QString("%1-%2")
              .arg(pWebSocket->peerAddress().toString())
              .arg(pWebSocket->peerPort());
    hashIpPort2PWebSocket.insert(key,pWebSocket);
    emit signal_newConnection(pWebSocket->peerAddress().toString(),pWebSocket->peerPort(),pWebSocket->requestUrl());
}
void wss::processTextMessage(const QString & msg)//收到文字数据
{
    QWebSocket *pWebSocket = dynamic_cast<QWebSocket *>(sender());
    if(!pWebSocket)
    {
        return;
    }
    qDebug()<<"new Text msg:"<<pWebSocket->peerAddress().toString()
            <<pWebSocket->peerPort()
            <<msg;
    //复读机
//    QString rmsg="rap"+msg;
//    this->slot_sendText(pWebSocket->peerAddress().toString()
//                  ,pWebSocket->peerPort()
//                  ,rmsg);
    emit signal_processTextMessage(pWebSocket->peerAddress().toString()
                                   ,pWebSocket->peerPort()
                                   ,msg);
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
    //复读机
//    QByteArray rmsg=msg;
//    this->slot_sendData(pWebSocket->peerAddress().toString()
//                  ,pWebSocket->peerPort()
//                  ,msg);
    emit signal_processBinauyMessage(pWebSocket->peerAddress().toString()
                                         ,pWebSocket->peerPort()
                                         ,msg);
}
int wss::slot_sendText(QString ip, quint16 port, QString msg)
{
    qDebug()<<"sendText"<<QString("%1-%2").arg(ip).arg(port)<<msg;
    QString key = QString("%1-%2").arg(ip).arg(port);
    if(hashIpPort2PWebSocket.contains(key))
    {
        qint64 size=hashIpPort2PWebSocket.value(key)->sendTextMessage(msg);
        qDebug()<<"发送了"<<size<<"大小的数据";
        return 0;
    }else{
        qDebug()<<"rep text fail";
    }
    return -1;
}
int wss::slot_sendData(QString ip, quint16 port, QByteArray msg)
{
    qDebug()<<"sendData"<<QString("%1-%2").arg(ip).arg(port)<<msg;
    QString key = QString("%1-%2").arg(ip).arg(port);
    if(hashIpPort2PWebSocket.contains(key))
    {
        qint64 size=hashIpPort2PWebSocket.value(key)->sendBinaryMessage(msg);
        qDebug()<<"发送了"<<size<<"大小的数据";
        return 0;
    }else{
        qDebug()<<"rep binary fail";
    }
    return -1;
}
int wss::slot_discennect(QString ip, quint16 port)
{
    QString key = QString("%1-%2").arg(ip).arg(port);
    if(hashIpPort2PWebSocket.contains(key))
    {
        hashIpPort2PWebSocket.value(key)->close();
        return 0;
    }else{
        return -1;
    }
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
    hashIpPort2PWebSocket.remove(QString("%1-%2")
                                  .arg(pWebSocket->peerAddress().toString())
                                  .arg(QString::number(pWebSocket->peerPort()))
                                  );
    emit signal_pDiscennect(pWebSocket->peerAddress().toString()
                            ,pWebSocket->peerPort());
}
