#include <QCoreApplication>
#include "wss.h"
#include "wsc.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //demo();
    wss * mywss = new wss();
    mywss->init(12345);

    return a.exec();
}

