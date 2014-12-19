#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <QSystemTrayIcon>

class SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit SystemTrayIcon(QObject *parent = 0);

signals:
    void trayIcon(bool);
    void showNotifications(bool);

    void placeMiniPlayer(QPoint &pt);
    void doubleClicked();
    void triggered();

public slots:
    void setTrayIcon(bool icon) { if (icon == tray_icon) return; tray_icon = icon; emit trayIcon(icon); }
    void setShowNotifications(bool show) { if (show == show_notifications) return; show_notifications = show; emit showNotifications(show); }

    void nowPlaying(QString title, QString artist, QString album, int duration);

    void onActivated(QSystemTrayIcon::ActivationReason);

    void load();
    void save();

    void setVisible(bool visible);

private:
    bool tray_icon;
    bool show_notifications;

    // Hack: toggling the tray icon on and off, seems to trigger multiple signal
    qint64 last_activation;

};

#endif // SYSTEMTRAYICON_H
