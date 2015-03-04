#include "gdeskstyler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GDeskStyler w;
    w.show();

    return a.exec();
}
