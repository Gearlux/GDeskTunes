#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>
#include <QSettings>
#include <QKeySequence>

// Forward declarations
class MainWindow;

class Options : public QObject
{
    friend class MainWindow;

    Q_OBJECT

public:
    explicit Options(MainWindow *mainWindow);

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

signals:
    void scrobbled(bool scrobble);
    void autoLiked(bool like);
    void lastFMUserName(QString name);
    void lastFMPassword(QString name);

    void login();

public slots:
    void setScrobbled(bool scrobble) { this->scrobble = scrobble; emit scrobbled(scrobble); }
    bool isScrobbled() { return this->scrobble; }

    void setAutoLiked(bool like) { this->auto_like = like; emit autoLiked(like); }
    bool isAutoLiked() { return this->auto_like; }

    void setLastFMUserName(QString name) { this->last_fm_user_name = name; emit lastFMUserName(name); }
    QString getLastFMUserName() { return this->last_fm_user_name; }

    void setLastFMPassword(QString passwd) { this->last_fm_password = passwd; emit lastFMPassword(passwd); }
    QString getLastFMPassword() { return this->last_fm_password; }

    void setAuthorized(bool status) { this->last_fm_authorized = status; }

private:
    MainWindow *main_window;

private:
    bool hide_menu;
    bool mini_player_on_top;
    bool save_cookies;
    bool customize;
    QString css;
    QString mini_css;

    // LastFM functionality
    bool scrobble;
    bool auto_like;
    QString last_fm_user_name;
    QString last_fm_password;

    bool last_fm_authorized;
};

#endif // OPTIONS_H
