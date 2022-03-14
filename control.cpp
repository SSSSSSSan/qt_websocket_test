#include "control.h"
#include <QDebug>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

//class room
int room::userIn(QString id)
{
    ids<<id;
}
void room::userOut(QString id)
{
    for(int i=0;i<ids.size();++i)
    {
        if (ids.at(i)==id);
        ids.remove(i);
        break;
    }
}
void room::sendText(QString msg)
{
    userInfo * senderId = dynamic_cast<userInfo *>(sender());
    for(int i=0;i<ids.size();++i)
    {
        if(ids.at(i)!=senderId->id)
        {
            emit textForward(ids.at(i),msg);
        }
    }
}
void room::sendData(QByteArray msg)
{

}
//class userInfo
userInfo::~userInfo()
{
    disconnect(this,0,0,0);
    disconnect(0,0,this,0);
    if(joining)exitRoom();
};
room * userInfo::createRoom()
{
    joining = new room;
    joining->ids<<id;
    qDebug()<<id<<"createroom"<<joining;
    inroom = true;
    connect(this,&userInfo::sendText
            ,joining,&room::sendText);
    connect(this,&userInfo::sendData
            ,joining,&room::sendData);
    return joining;

};
void userInfo::joinRoom(room * uroom)
{
    joining = uroom;
    joining->ids<<id;
    connect(this,&userInfo::sendText
            ,joining,&room::sendText);
    connect(this,&userInfo::sendData
            ,joining,&room::sendData);
    inroom = true;
    qDebug()<<id<<"join"<<uroom;
};
void userInfo::exitRoom()
{
    joining->userOut(this->id);
    qDebug()<<id<<"exit"<<joining;
    if(joining->ids.size()==0)
    {
        qDebug()<<id<<joining<<"isClosed";
        delete joining;
    }
    inroom = false;
};
//void userInfo::receivedtext(QString msg)
//{

//}
//void userInfo::receivedData(QByteArray msg)
//{

//}
//class control
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
    if(rebool)
    {
        connect(mywss,&wss::signal_newConnection
                ,this,&control::slot_newU);
        connect(mywss,&wss::signal_pDiscennect
                ,this,&control::slot_delU);
        connect(mywss,&wss::signal_processTextMessage
                ,this,&control::slot_receivedText);
        connect(this,&control::signal_sendText
                ,mywss,&wss::slot_sendText);
        connect(this,&control::signal_sendData
                ,mywss,&wss::slot_sendData);
    }

    return rebool;
}
void control::slot_newU(QString address,quint16 ip,QUrl url)
{
    qDebug()<<address<<ip<<url;
    QUrlQuery query;
    query.setQuery( url.query());
    //qDebug()<<query.queryItemValue("id")<<query.queryItemValue("password");
    userInfo * puserInfo = new userInfo;
    puserInfo->address=address;
    puserInfo->ip=ip;
    puserInfo->id=query.queryItemValue("id");
    _hashId2Key.insert(query.queryItemValue("id"),new QString(address+QString::number(ip)));
    _hashUserInfo.insert(address+QString::number(ip),puserInfo);
}
void control::slot_delU(QString address,quint16 ip)
{
    QString key=address+QString::number(ip);
    if(_hashUserInfo.contains(key))
    {
        QString id=_hashUserInfo.value(key)->id;
        qDebug()<<key<<id;
        emit this->signal_userSignOut(id);
        _hashId2Key.value(id)->~QString();
        _hashUserInfo.value(key)->~userInfo();
        _hashId2Key.remove(id);
        _hashUserInfo.remove(key);
        //qDebug()<<"userSignOut";
    }else{
        qDebug()<<"未找到"+key;
    }
}
void control::slot_receivedText(QString address,quint16 ip,QString msg)
{
    QJsonParseError err_rpt;
    QJsonDocument  Doc = QJsonDocument::fromJson(msg.toLocal8Bit(), &err_rpt);//字符串格式化为JSON
    if(err_rpt.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON格式错误";
        //emit _hashUserInfo.value(address+QString::number(ip))->sendText(msg);
        return;
    }else{
        this->msgControl(Doc,_hashUserInfo.value(address+QString::number(ip))->id);
    }

}
void control::slot_receivedData(QString address,quint16 ip,QByteArray msg)
{

}
void control::msgControl(QJsonDocument jdc,QString id)
{
    qDebug()<<id<<_hashId2Key.contains(id);
    if(!_hashId2Key.contains(id))return;
    //qDebug()<<jdc.object().find("connectTo");
    if(jdc.object().find("massage")->isString())
    {
        QString msg=jdc.object().find("massage")->toString();
        qDebug()<<id<<"发来消息"<<msg;
        if(_hashUserInfo.value(_hashId2Key.value(id)->toStdString().c_str())->inroom)
        {
            qDebug()<<"向room"<<_hashUserInfo.value(_hashId2Key.value(id)->toStdString().c_str())
                      ->joining<<"发送消息";
           emit _hashUserInfo.value(_hashId2Key.value(id)->toStdString().c_str())
                    ->sendText(msg);
        }
        return;//普通消息转发立即返回
    }
    if(jdc.object().find("connectTo")->isString())
    {

        QString connectToId=jdc.object().value("connectTo").toString();
        qDebug()<<id<<"发起对"<<connectToId<<"的connectTo请求";
        if(_hashId2Key.contains(connectToId))
        {
            this->slot_connectToUser(id,connectToId);
        }else{
            qDebug()<<"can't connect to "<<connectToId<<"noFound";
        }
    }

}
void control::slot_connectToUser(QString id1,QString id2)
{
    qDebug()<<"room work"<<id1<<id2;
    bool b1 = _hashUserInfo.value(_hashId2Key.value(id1)->toStdString().c_str())->inroom;
    bool b2 = _hashUserInfo.value(_hashId2Key.value(id2)->toStdString().c_str())->inroom;
    qDebug()<<b1<<b2;
    if( b1 & b2 )
    {
        qDebug()<<"双方均在各自的房间里";
        return;
    }else if(b1 | b2)
    {
        if(b1)
        {
            //u1在房间里 u2加入u1
            _hashUserInfo.value(_hashId2Key.value(id1)->toStdString().c_str())->joinRoom(
                        _hashUserInfo.value(_hashId2Key.value(id2)->toStdString().c_str())->joining
                        );
        }else{
            //u2在房间里 u1加入u2
            _hashUserInfo.value(_hashId2Key.value(id2)->toStdString().c_str())->joinRoom(
                        _hashUserInfo.value(_hashId2Key.value(id1)->toStdString().c_str())->joining
                        );
        }
    }else{
        //u1创房 u2加入
        _hashUserInfo.value(_hashId2Key.value(id2)->toStdString().c_str())->joinRoom(
                    _hashUserInfo.value(_hashId2Key.value(id1)->toStdString().c_str())->createRoom()
                    );
    }
    connect(_hashUserInfo.value(_hashId2Key.value(id1)->toStdString().c_str())->joining,&room::textForward
            ,this,&control::slot_textForward);

}
void control::slot_textForward(QString toId,QString msg)
{
    if(_hashId2Key.contains(toId))
    {
        QString address=_hashUserInfo.value(_hashId2Key.value(toId)->toStdString().c_str())->address;
        quint16 ip = _hashUserInfo.value(_hashId2Key.value(toId)->toStdString().c_str())->ip;
        emit signal_sendText(address,ip,msg);
    }
}
void control::slot_dataForward(QString toId,QByteArray msg)
{

}
void control::slot_disConnectUser(QString id1,QString id2)
{

}
