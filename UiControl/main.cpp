#include "widget.h"

#include <QApplication>
#include "testkitnetwork/networkclientwidget.h"
#include "testkitnetwork/networkserverwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

#if 1
    NetworkServerWidget ns;
    ns.show();

    NetworkClientWidget nc;
    nc.show();
#endif

    return a.exec();
}
