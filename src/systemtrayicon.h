#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>
#include <QMenu>

#include "mainwindow.h"

#ifdef Q_OS_MAC
#include <mac/notificationcenter.h>
#endif

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit SystemTrayIcon(MainWindow *parent = 0);

signals:
    void trayIcon(bool);
    void showNotifications(bool);

    void placeMiniPlayer(QPoint &pt);
    void doubleClicked();
    void triggered();

    void next();
    void albumArtNotifications(bool);
    void iTunesNotifications(bool);
public slots:
    void setTrayIcon(bool icon) { if (icon == tray_icon) return; tray_icon = icon; emit trayIcon(icon); }
    void setShowNotifications(bool show) { if (show == show_notifications) return; show_notifications = show; emit showNotifications(show); }

    void nowPlaying(QString title, QString artist, QString album, QString art, int duration);

    void onActivated(QSystemTrayIcon::ActivationReason);

    void load();
    void save();

    void setVisible(bool visible);

    void setAlbumArtNotifications(bool album_art) { if (album_art == use_album_art) return; emit albumArtNotifications(album_art); this->use_album_art = album_art; }
    void setITunesNotifications(bool itunes) { if (itunes == itunes_like) return; emit iTunesNotifications(itunes); this->itunes_like = itunes; }


private:
    bool use_album_art;
    bool itunes_like;
    bool tray_icon;
    bool show_notifications;

    // Hack: toggling the tray icon on and off, seems to trigger multiple signal
    qint64 last_activation;

    // Holder for the menu
    QMenu *menu;

#ifdef Q_OS_MAC
    MacNotificationCenter* mac_center;
#endif

};

#endif // SYSTEMTRAYICON_H
