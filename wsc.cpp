#include "wsc.h"
#include <QUrl>
#include <QWebSocket>
#include <QDebug>
wsc::wsc()
{
QWebSocket * p;
QUrl url("ws://ddns.minemc.top:54321?id=wscTest");
p->setParent(this);
p->open(url);




}
wsc::~wsc()
{

}
