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

public slots:
    void miniPlayerOnTop(bool on_top);

    void style(QString style);
    void miniStyle(QString style);
    void clearCookies();
    void doNotSaveCookies(bool do_not_save);
    void customize(bool customize);
    void lastFMChanged();

    void authorize();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::Settings *ui;
    MainWindow *main_window;
    Options &options;
};

#endif // SETTINGS_H
