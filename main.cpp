#include <QCoreApplication>
#include "wss.h"
#include "wsc.h"
#include "control.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //wss * mywss = new wss();
    //mywss->init(12345);
    control * wssConcrol = new control;
    wssConcrol->init();
    return a.exec();
}

