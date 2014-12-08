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

public slots:
    void setTrayIcon(bool icon) { if (icon == tray_icon) return; tray_icon = icon; emit trayIcon(icon); }
    void setShowNotifications(bool show) { if (show == show_notifications) return; show_notifications = show; emit showNotifications(show); }

    void nowPlaying(QString title, QString artist, QString album, int duration);

    void load();
    void save();

private:
    bool tray_icon;
    bool show_notifications;
};

#endif // SYSTEMTRAYICON_H
