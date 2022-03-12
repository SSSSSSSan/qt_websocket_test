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
    QList<QWebSocket*> clients;
signals:
    void close();
private slots:
    void onNewConnection();
    void processTextMessage(const QString & msg);
    void socketDisconnected();
};

#endif // WSS_H
