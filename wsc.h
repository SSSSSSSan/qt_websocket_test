#ifndef WSC_H
#define WSC_H
#include <QObject>
#include <QWebSocket>
class wsc : public QObject
{
     Q_OBJECT
public:
    wsc();
    ~wsc();
};

#endif // WSC_H
