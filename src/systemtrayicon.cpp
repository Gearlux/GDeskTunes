// #define QT_NO_DEBUG_OUTPUT

#include "systemtrayicon.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QDateTime>

SystemTrayIcon::SystemTrayIcon(MainWindow *parent) :
    QSystemTrayIcon(parent),
    tray_icon(false),
    last_activation(0)
{
    qDebug() << "SystemTrayIcon::SystemTrayIcon()";
    setIcon(QIcon(":/icons/gdesktunes.iconset/icon_16x16.png"));

    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));

    menu = new QMenu(parent);
    menu->addAction(parent->ui->actionPreferences);

#ifndef Q_OS_MAC
    // Bug in Qt (5.4?), left click also triggers context menu
    setContextMenu(menu);
#endif
}

void SystemTrayIcon::nowPlaying(QString title, QString artist, QString album, int duration)
{
    qDebug() << "SystemTrayIcon::nowPlaying(" << title << "," << artist << "," << album << "," << duration << ")";
    if (show_notifications)
        showMessage(title, artist + " (" + album + ")", QSystemTrayIcon::MessageIcon(NoIcon), 1000);
}

void SystemTrayIcon::save()
{
    qDebug() << "SystemTrayIcon::save()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.setValue("show_notifications", this->show_notifications);
    settings.setValue("tray_icon", this->tray_icon);
}

void SystemTrayIcon::load()
{
    qDebug() << "SystemTrayIcon::load()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    this->setTrayIcon(settings.value("tray_icon", false).toBool());
    this->setShowNotifications(settings.value("show_notifications", true).toBool());

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
        emit triggered();
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
