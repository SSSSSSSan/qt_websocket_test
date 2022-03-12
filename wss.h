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
    bool init(uint16_t port);
private:
    uint16_t port;
    static wss * wsstp;
    QWebSocketServer * server=nullptr;
    QWebSocket * socket=nullptr;
    int connectNum=0;
    QHash<QString, QWebSocket*> _hashIpPort2PWebSocket;
signals:
    void close();
private slots:
    void onNewConnection();
    void processTextMessage(const QString & msg);
    void processBinauyMessage(const QByteArray &msg);
    void socketDisconnected();

private:
    void slot_sendText(QString ip, uint16_t port, QString &msg);
    void slot_sendData(QString ip, uint16_t port, QByteArray &buffer);
    void slot_discennect();
};

#endif // WSS_H
