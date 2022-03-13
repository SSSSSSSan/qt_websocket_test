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

};

#endif // CONTROL_H
