#ifndef WSS_H
#define WSS_H

#include <QObject>
#include <QDebug>
#include <QWebSocketServer>
#include <QWebSocket>
class wss : public QObject
{
    Q_OBJECT
public:
    wss();
    ~wss();

    int connections=0;//当前连接数
private:
    uint16_t port;
    QWebSocketServer * server=nullptr;
    QWebSocket * socket=nullptr;
    QHash<QString, QWebSocket*> _hashIpPort2PWebSocket;
public:
    bool init(quint16 port);
private:

signals:
    int signal_processTextMessage(QString ip, quint16 port, QString msg);//收到文字信息
    int signal_processBinauyMessage(QString ip, quint16 port, QByteArray buffer);//收到二进制数据
    void signal_pDiscennect(QString ip, quint16 port);//该客户端断开链接
public slots:
    //bool slot_connectionTo(QString ip, quint16 port);//创建新链接在wsc做
    void slot_discennect(QString ip, quint16 port);//主动关闭某个链接-貌似不好做
    int slot_sendText(QString ip, quint16 port, QString msg);//主动向客户端发送文字信息
    int slot_sendData(QString ip, quint16 port, QByteArray buffer);//主动向客户端发送二进制信息
private slots:
    void onNewConnection();
    void processTextMessage(const QString & msg);
    void processBinauyMessage(const QByteArray & msg);
    void socketDisconnected();
};

#endif // WSS_H
