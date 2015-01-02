#ifndef APPLICATION_H
#define APPLICATION_H

#include "thirdparty/qt-solutions/qtsingleapplication/src/QtSingleApplication"

// Forward declarations
class QMainWindow;
class QSystemTrayIcon;

/**
 * @brief The DockedApplication class.
 *
 * The DockedApplication class implements functionality specifically for Mac to
 * implement the dockClicked() signal.
 *
 * In order to implement this, the application needs to be aware of the system tray
 * icon and all other active windows. Use the appropriate methods to register them.
 *
 * If done correctly, you can safely use the dockClicked() signal to connect your
 * application's functionality.
 */
class DockedApplication : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit DockedApplication(int &argc, char **argv, bool GUIenabled = true);

signals:
    void dockClicked();

public:
    void setTrayIcon(QSystemTrayIcon *tray);
    void addWindow(QMainWindow *window);

private slots:
    // Connected slots
    void onApplicationStateChanged(Qt::ApplicationState state);
    void onTrayIconTriggered();
    void onWindowDestroyed(QObject *);

private:
    qint64 tray_icon_time;
    QList<QMainWindow *> windows;
};

#endif // APPLICATION_H
