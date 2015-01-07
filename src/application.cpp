// #define QT_NO_DEBUG_OUTPUT

#include "application.h"

#include <QMainWindow>
#include <QSystemTrayIcon>

#include <QDebug>
#include <QDateTime>

DockedApplication::DockedApplication(int &argc, char **argv, bool GUIenabled) :
    QtSingleApplication(argc,argv, GUIenabled),
    tray_icon_time(0)
{
    QObject::connect(this, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(onApplicationStateChanged(Qt::ApplicationState)));
}

void DockedApplication::onApplicationStateChanged(Qt::ApplicationState state)
{
    qDebug() << "Application::onApplicationStateChanged(" << state << ")";
    qDebug() << "   activeWindow(): " << activeWindow();
    qDebug() << "   activationWindow():" << activationWindow();
    qDebug() << "   just_clicked:" << ((tray_icon_time + 100) > QDateTime::currentMSecsSinceEpoch());
    switch(state)
    {
    case  Qt::ApplicationActive:
    {
        bool visible_window = false;
        for(int i=0; i<windows.count(); ++i)
        {
            if (windows.at(i)->isVisible())
                visible_window = true;
        }
        qint64 current = QDateTime::currentMSecsSinceEpoch();
        bool just_clicked = tray_icon_time + 100 > current;
        if (activeWindow() == 0 && !just_clicked && !visible_window)
        {
            emit dockClicked();
            return;
        }
        if (activeWindow() != 0 && !just_clicked)
        {
            emit dockClicked();
            return;
        }
        break;
    }
    default:
        break;
    }
}

void DockedApplication::setTrayIcon(QSystemTrayIcon *tray)
{
    QObject::connect(tray, SIGNAL(triggered()), this, SLOT(onTrayIconTriggered()));
}

void DockedApplication::onTrayIconTriggered()
{
    qDebug() << "Application::onTrayIconTriggered()";
    tray_icon_time = QDateTime::currentMSecsSinceEpoch();
}

void DockedApplication::addWindow(QMainWindow *window)
{
    windows.append(window);
    QObject::connect(window, SIGNAL(destroyed(QObject*)), this, SLOT(onWindowDestroyed(QObject*)));
}

void DockedApplication::onWindowDestroyed(QObject *object)
{
    windows.removeAll(dynamic_cast<QMainWindow*>(object));
}
