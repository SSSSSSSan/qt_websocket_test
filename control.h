#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include "wss.h"
class room : public QObject
{
    Q_OBJECT
public:
    //room();
    //~room();
    QStringList ids;
    void userOut(QString id);
    int userIn(QString id);
signals:
    //此房间应该转发的消息提交给control处理
    void textForward(QString toId,QString msg);
    void dataForward(QString toId,QByteArray msg);
public slots:
    //此房间应该转发的消息
    void sendText(QString msg);
    void sendData(QByteArray msg);

};
class userInfo : public QObject
{
    Q_OBJECT
public:
    QString address = 0;
    quint16 ip = 0;
    QString id = "";
    bool inroom=false;
    room * joining;
    QStringList teams;

    ~userInfo();
    room * createRoom();
    void joinRoom(room * uroom);
    void exitRoom();

private:

signals:
    //转发此用户的消息
    void sendText(QString msg);
    void sendData(QByteArray msg);
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
    void msgControl_control(QJsonObject job,QString id);
private:
    wss * mywss=nullptr;

    QHash<QString, userInfo *> _hashUserInfo;
    QHash<QString, QString *> _hashId2Key;
    //QHash<QString, room *> _hashRoom;
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
    void slot_textForward(QString toId,QString msg);
    void slot_dataForward(QString toId,QByteArray msg);
};

#endif // CONTROL_H
