#define QT_NO_DEBUG_OUTPUT

#include "systemtrayicon.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QDateTime>
#include <QTimer>

SystemTrayIcon::SystemTrayIcon(MainWindow *parent) :
    QSystemTrayIcon(parent),
    hide_when_active(false),
    use_album_art(false),
    itunes_like(false),
    tray_icon(false),
    last_activation(0)
{
    qDebug() << "SystemTrayIcon::SystemTrayIcon()";
    setIcon(QIcon(":/icons/gdesktunes.iconset/icon_16x16.png"));

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));

#ifndef Q_OS_MAC
    // Bug in Qt (5.4?), left click also triggers context menu
    menu = new QMenu(parent);
    menu->addAction(parent->ui->actionPreferences);
    menu->addAction(parent->ui->actionAbout);
    menu->addSeparator();
    menu->addAction(parent->ui->actionPlay);
    menu->addAction(parent->ui->actionPrevious);
    menu->addAction(parent->ui->actionNext);
    menu->addSeparator();
    menu->addAction(parent->ui->actionQuit_GDeskTunes);

    setContextMenu(menu);
#else
    mac_center = new MacNotificationCenter();
    QObject::connect(mac_center, SIGNAL(next()), this, SIGNAL(next()));
#endif
}

void SystemTrayIcon::nowPlaying(QString title, QString artist, QString album, QString art, int duration)
{
    qDebug() << "SystemTrayIcon::nowPlaying(" << title << "," << artist << "," << album << "," << duration << ")";

    bool show = true;
    if (hide_when_active)
    {
        QGuiApplication *app = dynamic_cast<QGuiApplication*>(QApplication::instance());
        if (app != 0)
        {
            show = app->applicationState() != Qt::ApplicationActive;
        }
    }

    if (show_notifications && show)
    {
#ifdef Q_OS_MAC
        mac_center->showMessage(title, artist + " (" + album + ")", art, itunes_like, use_album_art);
#else
        Q_UNUSED(art);
        showMessage(title, artist + " (" + album + ")", QSystemTrayIcon::MessageIcon(NoIcon), 1000);
#endif
    }
}

void SystemTrayIcon::save()
{
    qDebug() << "SystemTrayIcon::save()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.setValue("show_notifications", this->show_notifications);
    settings.setValue("tray_icon", this->tray_icon);
    settings.setValue("album_art_notifications", this->use_album_art);
    settings.setValue("itunes_notifications", this->itunes_like);
    settings.setValue("hide_when_active", this->hide_when_active);
}

void SystemTrayIcon::load()
{
    qDebug() << "SystemTrayIcon::load()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    this->setTrayIcon(settings.value("tray_icon", false).toBool());
    this->setShowNotifications(settings.value("show_notifications", true).toBool());
    this->setAlbumArtNotifications(settings.value("album_art_notifications", true).toBool());
    this->setITunesNotifications(settings.value("itunes_notifications", true).toBool());
    this->setHideWhenActive(settings.value("hide_when_active", true).toBool());

    emit trayIcon(this->tray_icon);
}

void SystemTrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    qDebug() << "SystemTrayIcon::onActivated(" << reason << ")";
    qint64 current_activation = QDateTime::currentMSecsSinceEpoch();
    if (current_activation < last_activation + 100)
    {
        current_activation = last_activation;
        return;
    }
    current_activation = last_activation;
    if (reason == QSystemTrayIcon::DoubleClick)
    {
        // Also emit the trigger signal (at least for windows)
#ifndef Q_OS_LINUX
        emit triggered();
#else
        QRect geom = this->geometry();
        QPoint pt = geom.topLeft();
        emit placeMiniPlayer(pt);
#endif
        emit doubleClicked();
        return;
    }
    if (reason == QSystemTrayIcon::Trigger)
    {
        QRect geom = this->geometry();
        QPoint pt = geom.topLeft();

        emit placeMiniPlayer(pt);
        emit triggered();
    }
}

void SystemTrayIcon::setVisible(bool visible)
{
    qDebug() << "SystemTrayIcon::setVisible("<< visible << ")";
    QSystemTrayIcon::setVisible(visible);
}
