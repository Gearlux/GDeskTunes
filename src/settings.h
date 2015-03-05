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
    void keyPressed(QKeyEvent *event);

public slots:
    void activateAndRaise();
    void authorized(bool authorize);
    void authorize();

    void updateStyles();

public:
    QStringList getStyles(QString subdir = QString::null);

protected:
    void keyPressEvent(QKeyEvent *event);

public:
    Ui::Settings *ui;

private:
    bool last_fm_authorized;
};

#endif // SETTINGS_H
