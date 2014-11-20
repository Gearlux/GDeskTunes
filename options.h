#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>
#include <QSettings>
#include <QKeySequence>

// Forward declarations
class MainWindow;

class Options
{
    friend class MainWindow;

public:
    Options(MainWindow *mainWindow);

public:
    void save(QSettings &settings);
    void restore(QSettings &settings);

    void setHideMenu(bool hide);
    bool isHideMenu();

    void setMiniPlayerOnTop(bool on_top);
    bool isMiniPlayerOnTop();

    void setSaveCookies(bool save);
    bool saveCookies();

    void setCustomize(bool customize);
    bool isCustomized();

    QString getCSS();
    void setCSS(QString css);

    QString getMiniCSS();
    void setMiniCSS(QString css);

    QString getStyle(QString name, QString subdir = QString::null);
    QStringList getStyles(QString subdir = QString::null);

    Qt::WindowFlags miniPlayerFlags();

    void setScrobble(bool scrobble) { this->scrobble = scrobble; }
    bool isScrobble() { return this->scrobble; }

    void setAutoLike(bool like) { this->auto_like = like; }
    bool isAutoLike() { return this->auto_like; }

    void setLastFMUserName(QString name) { this->last_fm_user_name = name; }
    QString getLastFMUserName() { return this->last_fm_user_name; }
    void setLastFMPassword(QString passwd) { this->last_fm_password = passwd; }
    QString getLastFMPassword() { return this->last_fm_password; }

private:
    MainWindow *main_window;

private:
    bool hide_menu;
    bool mini_player_on_top;
    bool save_cookies;
    bool customize;
    QString css;
    QString mini_css;
    bool scrobble;
    bool auto_like;
    QString last_fm_user_name;
    QString last_fm_password;
};

#endif // OPTIONS_H
