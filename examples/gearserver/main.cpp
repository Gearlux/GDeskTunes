#include "dialog.h"
#include <QApplication>
#include <QTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();

    // initialize the random library
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    return a.exec();
}
