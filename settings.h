#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "options.h"

namespace Ui {
class Settings;
}
class MainWindow;

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(MainWindow *window);
    ~Settings();

signals:
    void login();
    void logout();

public slots:
    void activateAndRaise();
    void show();

    void miniPlayerOnTop(bool on_top);

    void style(QString style);
    void miniStyle(QString style);
    void clearCookies();
    void doNotSaveCookies(bool do_not_save);
    void customize(bool customize);

    void setAuthorized(bool authorized);

    void authorize();

    void on_scrobble_toggled(bool);
    void on_auto_love_toggled(bool);

protected:
    void closeEvent(QCloseEvent *event);

private:
    bool last_fm_authorized;
    Ui::Settings *ui;
    MainWindow *main_window;
    Options &options;
};

#endif // SETTINGS_H
