#define QT_NO_DEBUG_OUTPUT

#include "application.h"

#include <QDebug>
#include <QWindow>
#include <QWidget>
#include <QDateTime>

Application::Application(int &argc, char **argv, bool GUIenabled) :
    QtSingleApplication(argc,argv, GUIenabled),
    tray_icon_time(0)
{
    QObject::connect(this, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(onApplicationStateChanged(Qt::ApplicationState)));
}

void Application::onApplicationStateChanged(Qt::ApplicationState state)
{
    switch(state)
    {
    case  Qt::ApplicationActive:
    {
        qint64 current = QDateTime::currentMSecsSinceEpoch();
        bool just_clicked = tray_icon_time + 100 > current;
        bool with_focus = activeWindow() != 0;
        qDebug() << "Application::onApplicationStateChanged(ApplicationActive)";

        qDebug() << "Application::onDockClick(" << just_clicked << "," << with_focus << ")" << current << tray_icon_time;
        if (just_clicked && with_focus)
            emit onDockTrueTrue();
        if (!just_clicked && with_focus)
            emit onDockFalseTrue();
        if (just_clicked && !with_focus)
            emit onDockTrueFalse();
        if (!just_clicked && !with_focus)
            emit onDockFalseFalse();
    }
        break;
    case Qt::ApplicationInactive:
        qDebug() << "Application::onApplicationStateChanged(ApplicationInactive)";
        emit applicationInActivated();
        break;
    case Qt::ApplicationHidden:
        qDebug() << "Application::onApplicationStateChanged(ApplicationHidden)";
        emit applicationHidden();
        break;
    case Qt::ApplicationSuspended:
        qDebug() << "Application::onApplicationStateChanged(ApplicationSuspended)";
        emit applicationSuspended();
        break;
    default:
        break;
    }
}

void Application::trayIconClicked()
{
    tray_icon_time = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "Application::onTrayIconClicked()" << tray_icon_time;
}
