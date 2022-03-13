#include "control.h"
#include <QDebug>
#include <QUrl>
#include <QUrlQuery>
//class userInfo

void userInfo::receivedtext(QString address,quint16 ip ,QString msg)
{

}
void userInfo::receivedData(QString address,quint16 ip ,QByteArray msg)
{

}
control::control()
{

}
control::~control()
{
    if(mywss!=nullptr)delete mywss;
}
bool control::init()
{
    wss * mywss = new wss();
    bool rebool=mywss->init(12345);
    connect(mywss,&wss::signal_newConnection
            ,this,&control::slot_newU);
    return rebool;
}
void control::slot_newU(QString address,quint16 ip,QUrl url)
{
    qDebug()<<address<<ip<<url;
    QUrlQuery query;
    query.setQuery( url.query());
    //qDebug()<<query.queryItemValue("id")<<query.queryItemValue("password");
    userInfo * puserInfo = new userInfo;
    puserInfo->userAddress=address;
    puserInfo->ip=ip;
    _hashUserInfo.insert(query.queryItemValue("id"),puserInfo);
}
void control::slot_connectTo(QString id,QString toId)
{
    if(!_hashUserInfo.contains(id)|!_hashUserInfo.contains(toId))
    {
        qDebug()<<"未找到某个ID";
    }else{
        //_hashUserInfo.value(id)->connectToId.
    }
}
void control::slot_receivedText(QString address,quint16 ip,QString msg)
{

}
void control::slot_receivedData(QString address,quint16 ip,QByteArray msg)
{

}
