#include "application.h"

#include <QDebug>

Application::Application(int &argc, char **argv, bool GUIenabled) :
    QtSingleApplication(argc,argv, GUIenabled)
{
}

bool Application::event(QEvent *ev)
{
    qDebug() << ev->type() << " " << sender();
    return QtSingleApplication::event(ev);
}
