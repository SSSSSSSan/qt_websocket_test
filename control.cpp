#include "control.h"
#include <QDebug>
#include <QUrl>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QThread>
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
            qDebug()<<ids.at(i)<<msg;
            emit textForward(ids.at(i),msg);
        }
    }
}
void room::sendData(QByteArray msg)
{
    userInfo * senderId = dynamic_cast<userInfo *>(sender());
    for(int i=0;i<ids.size();++i)
    {
        if(ids.at(i)!=senderId->id)
        {
            emit dataForward(ids.at(i),msg);
        }
    }
}
//class userInfo
userInfo::~userInfo()
{
    this->disconnect();
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
    userInfo * puserInfo = new userInfo;
    puserInfo->address=address;
    puserInfo->ip=ip;
    puserInfo->id=query.queryItemValue("id");
    //qDebug()<<query.queryItemValue("id")<<query.queryItemValue("password");
    //QThread * tuserInfo;
    //puserInfo->moveToThread(tuserInfo);
    _hashId2Key.insert(query.queryItemValue("id"),new QString(address+QString::number(ip)));
    _hashUserInfo.insert(address+QString::number(ip),puserInfo);
}
void control::slot_delU(QString address,quint16 ip)
{
    QString key=address+QString::number(ip);
    if(_hashUserInfo.contains(key))
    {
        QString id=_hashUserInfo.value(key)->id;
        qDebug()<<"delete"<<key<<id;
        emit this->signal_userSignOut(id);
        //_hashId2Key.value(id)->~QString();
        //_hashUserInfo.value(key)->~userInfo();
        _hashUserInfo.value(key)->deleteLater();
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
    QJsonDocument  Doc = QJsonDocument::fromJson(msg.toStdString().c_str(), &err_rpt);//字符串格式化为JSON
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
    if(_hashUserInfo.value(address+QString::number(ip))->inroom)
    {
        emit _hashUserInfo.value(address+QString::number(ip))
                 ->sendData(msg);
    }

}
void control::msgControl(QJsonDocument jdc,QString id)
{
    //qDebug()<<id<<_hashId2Key.contains(id);
    if(!_hashId2Key.contains(id))return;
    if(jdc.object().find("message2room")->isString())
    {
        QString msg=jdc.object().find("message2room")->toString();
        qDebug()<<id<<"发来消息"<<msg;
        if(_hashUserInfo.value(_hashId2Key.value(id)->toStdString().c_str())->inroom)
        {
            qDebug()<<"向room"<<_hashUserInfo.value(_hashId2Key.value(id)->toStdString().c_str())
                      ->joining<<"发送消息"<<msg;
            QJsonDocument jdc;
            QJsonObject job;
            job.insert("version",1);
            job.insert("message",msg);
            job.insert("sender",id);
            job.insert("type","room");
            jdc.setObject(job);
           emit _hashUserInfo.value(_hashId2Key.value(id)->toStdString().c_str())
                    ->sendText(jdc.toJson().toStdString().c_str());
        }
        return;//转发立即返回

    }
    if(jdc.object().find("message2id")->isObject())
    {
        /*
        {"message2id":{"toId":"toId","message":"message"}}
        */
        QString toId=jdc.object().find("message2id")->toObject().find("toId")->toString();
        if(!_hashId2Key.contains(toId))return;
        QString msg=jdc.object().find("message2id")->toObject().find("message")->toString();
        QJsonDocument msgjdc;
        QJsonObject job;
        job.insert("version",1);
        job.insert("message",msg);
        job.insert("sender",id);
        job.insert("type","user");
        msgjdc.setObject(job);
        emit signal_sendText(
                    _hashUserInfo.value(_hashId2Key.value(toId)->toStdString().c_str())->address,
                    _hashUserInfo.value(_hashId2Key.value(toId)->toStdString().c_str())->ip,
                    msgjdc.toJson().toStdString().c_str()
                    );
        return;
    }
    if(!jdc.object().find("control")->isNull())this->msgControl_control(jdc.object().find("control")->toObject(),id);




}
void control::msgControl_control(QJsonObject job, QString id)
{
    if(!job.find("exitRoom")->toInt()==1)
    {
        if(_hashId2Key.contains(id))
        {
            if(_hashUserInfo.value(_hashId2Key.value(id)->toStdString().c_str())->inroom)
            {
                _hashUserInfo.value(_hashId2Key.value(id)->toStdString().c_str())->exitRoom();
            }
        }
    }

    if(job.find("connectTo")->isString())
        //{"control":{"connectTo":"id"}}
    {

        QString connectToId=job.value("connectTo").toString();
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
            _hashUserInfo.value(_hashId2Key.value(id2)->toStdString().c_str())->joinRoom(
                        _hashUserInfo.value(_hashId2Key.value(id1)->toStdString().c_str())->joining
                        );
        }else{
            //u2在房间里 u1加入u2
            _hashUserInfo.value(_hashId2Key.value(id1)->toStdString().c_str())->joinRoom(
                        _hashUserInfo.value(_hashId2Key.value(id2)->toStdString().c_str())->joining
                        );
        }
    }else{
        //u1创房 u2加入
        _hashUserInfo.value(_hashId2Key.value(id2)->toStdString().c_str())->joinRoom(
                    _hashUserInfo.value(_hashId2Key.value(id1)->toStdString().c_str())->createRoom()
                    );
        connect(_hashUserInfo.value(_hashId2Key.value(id1)->toStdString().c_str())->joining
                ,&room::textForward
                ,this,&control::slot_textForward);
        connect(_hashUserInfo.value(_hashId2Key.value(id1)->toStdString().c_str())->joining
                ,&room::dataForward
                ,this,&control::slot_dataForward);
    }


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
    if(_hashId2Key.contains(toId))
    {
        QString address=_hashUserInfo.value(_hashId2Key.value(toId)->toStdString().c_str())->address;
        quint16 ip = _hashUserInfo.value(_hashId2Key.value(toId)->toStdString().c_str())->ip;
        emit signal_sendData(address,ip,msg);
    }
}
void control::slot_disConnectUser(QString id1,QString id2)
{

}
