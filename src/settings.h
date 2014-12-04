#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class Settings;
}
class GDeskTunes;

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(GDeskTunes *window);
    ~Settings();

signals:
    void login(QString username, QString password);
    void logout();

    void saveCookies(bool saveCookie);

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

public:
    QStringList getStyles(QString subdir = QString::null);

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);

public:
    Ui::Settings *ui;

private:
    bool last_fm_authorized;
    GDeskTunes *main_window;
};

#endif // SETTINGS_H
