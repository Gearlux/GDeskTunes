#ifndef APPLICATION_H
#define APPLICATION_H

#include "thirdparty/qt-solutions/qtsingleapplication/src/QtSingleApplication"

class Application : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit Application(int &argc, char **argv, bool GUIenabled = true);

signals:
    void applicationActivated();
    void applicationInActivated();
    void applicationHidden();
    void applicationSuspended();

    void onDockTrueTrue();
    void onDockTrueFalse();
    void onDockFalseTrue();
    void onDockFalseFalse();

public slots:
    void onApplicationStateChanged(Qt::ApplicationState state);
    void trayIconClicked();

private:
    qint64 tray_icon_time;
};

#endif // APPLICATION_H
