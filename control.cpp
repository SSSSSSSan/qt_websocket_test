#include "control.h"
#include <QDebug>
#include <QUrl>
#include <QUrlQuery>
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
            ,this,&control::slots_newU);
    return rebool;
}
void control::slots_newU(QString address,quint16 ip,QUrl url)
{
    qDebug()<<address<<ip<<url;
    QUrlQuery query;
    query.setQuery( url.query());
    qDebug()<<query.queryItemValue("id")<<query.queryItemValue("password");

}
