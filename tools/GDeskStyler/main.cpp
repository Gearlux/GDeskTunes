#include "gdeskstyler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("GDeskStyler");
    QApplication::setApplicationDisplayName("GDeskStyler");
    QApplication::setApplicationVersion("0.3.7");
    QApplication::setOrganizationName("GearLux");

    GDeskStyler w;
    w.show();

    return a.exec();
}
