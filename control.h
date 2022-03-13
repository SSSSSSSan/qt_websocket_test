#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include "wss.h"
class control : public QObject
{
    Q_OBJECT

public:
    control();
    ~control();
    bool init();

private:
    wss * mywss=nullptr;

signals:

public slots:

private slots:
    void slots_newU(QString ip,quint16 port,QUrl url);
};

#endif // CONTROL_H
