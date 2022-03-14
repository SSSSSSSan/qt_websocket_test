#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include "wss.h"
class room : public QObject
{
    Q_OBJECT
public:
    QStringList ids;
    void userOut(QString id)
    {
        for(int i=0;i<ids.size();++i)
        {
            if (ids.at(i)==id);
            ids.remove(i);
            break;
        }
    };
    void userIn(QString id);
};
class userInfo : public QObject
{
    Q_OBJECT
public:
    QString userAddress = 0;
    quint16 ip = 0;
    QString id = "";
    bool inroom=false;
    room * joining;
    QStringList teams;

    ~userInfo()
    {
        disconnect(this,0,0,0);
        disconnect(0,0,this,0);
        if(joining)exitRoom();
    };
    room * createRoom()
    {
        joining = new room;
        joining->ids<<id;
        qDebug()<<id<<"createroom"<<joining;
        return joining;
    };
    void joinRoom(room * uroom)
    {
        joining = uroom;
        joining->ids<<id;
        qDebug()<<id<<"join"<<uroom;
    };
    void exitRoom()
    {
        joining->userOut(this->id);
        qDebug()<<id<<"exit"<<joining;
        if(joining->ids.size()==0)
        {
            qDebug()<<id<<joining<<"isClosed";
            delete joining;
        }
    };

private:

signals:
    void sendText(QString msg);
    void sendData(QByteArray msg);
    void receivedtext(QString msg);
    void receivedData(QByteArray msg);
public slots:

};
class control : public QObject
{
    Q_OBJECT

public:
    control();
    ~control();
    bool init();
private:

    void msgControl(QJsonDocument jdc,QString id);
private:
    wss * mywss=nullptr;

    QHash<QString, userInfo *> _hashUserInfo;
    QHash<QString, QString *> _hashId2Key;
    QHash<QString, room *> _hashRoom;
signals:
    void signal_sendText(QString address,quint16 ip,QString msg);
    void signal_sendData(QString address,quint16 ip,QByteArray msg);
    void signal_userSignOut(QString id);
public slots:
    void slot_connectToUser(QString id1,QString id2);
    void slot_disConnectUser(QString id1,QString id2);
private slots:
    void slot_receivedText(QString address,quint16 ip,QString msg);//处理文字信息 rpn?
    void slot_receivedData(QString address,quint16 ip,QByteArray msg);//处理二进制 基本只转发
    void slot_newU(QString ip,quint16 port,QUrl url);
    void slot_delU(QString ip,quint16 port);
};

#endif // CONTROL_H
