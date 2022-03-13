#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include "wss.h"
class userInfo : public QObject
{
    Q_OBJECT
public:
    QString userAddress = 0;
    quint16 ip = 0;
    QStringList teams;
signals:
    void sendText(QString address,quint16 ip,QString msg);
    void sendData(QString address,quint16 ip,QByteArray msg);
public slots:
    void receivedtext(QString address,quint16 ip,QString msg);
    void receivedData(QString address,quint16 ip,QByteArray msg);
};
class control : public QObject
{
    Q_OBJECT

public:
    control();
    ~control();
    bool init();

private:
    wss * mywss=nullptr;

    QHash<QString, userInfo *> _hashUserInfo;
signals:
    void signal_sendText(QString address,quint16 ip,QString msg);
    void signal_sendData(QString address,quint16 ip,QByteArray msg);
public slots:
    void slot_connectTo(QString id,QString toID);
private slots:
    void slot_receivedText(QString address,quint16 ip,QString msg);//处理文字信息 rpn?
    void slot_receivedData(QString address,quint16 ip,QByteArray msg);//处理二进制 基本只转发
    void slot_newU(QString ip,quint16 port,QUrl url);
};

#endif // CONTROL_H
