#ifndef WSS_H
#define WSS_H

#include <QObject>
#include <QDebug>
#include <QWebSocketServer>
#include <QWebSocket>

//test
#include <QTimer>
class wss : public QObject
{
    Q_OBJECT
public:
    wss();
    ~wss();

    int connections(){return _hashIpPort2PWebSocket.size();};//当前连接数
private:
    uint16_t port;
    QWebSocketServer * server=nullptr;
    QHash<QString, QWebSocket*> _hashIpPort2PWebSocket;
public:
    bool init(quint16 port);
private:

signals:
    void signal_newConnection(QString ip,quint16 port);//新链接
    void signal_processTextMessage(QString ip, quint16 port, QString msg);//收到文字信息
    void signal_processBinauyMessage(QString ip, quint16 port, QByteArray buffer);//收到二进制数据
    void signal_pDiscennect(QString ip, quint16 port);//该客户端断开链接
public slots:
    //bool slot_connectionTo(QString ip, quint16 port);//创建新链接在wsc做
    int slot_discennect(QString ip, quint16 port);//主动关闭某个链接
    int slot_sendText(QString ip, quint16 port, QString msg);//主动向客户端发送文字信息
    int slot_sendData(QString ip, quint16 port, QByteArray buffer);//主动向客户端发送二进制信息
private slots:
    void onNewConnection();
    void processTextMessage(const QString & msg);
    void processBinauyMessage(const QByteArray & msg);
    void socketDisconnected();
};

#endif // WSS_H
