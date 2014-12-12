#include "gdesktunes.h"
#include "settings.h"
#include "googlemusicapp.h"
#include "lastfm.h"
#include "cookiejar.h"
#include "networkmanager.h"
#include "systemtrayicon.h"
#include "miniplayer.h"
#include "application.h"
#include "qutils.h"

#include "ui_mainwindow.h"
#include "ui_settings.h"

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
#include <QtWinExtras>
#endif
#endif

#ifdef Q_OS_DARWIN
#include "mac/mediakeys.h"
#endif

#include "thirdparty/qt-solutions/qtsingleapplication/src/qtsingleapplication.h"

#include <QApplication>
#include <QtNetwork/QNetworkCookie>
#include <QNetworkAccessManager>
#include <QtCore/QDebug>
#include <QSettings>
#include <QtCore/QDir>
#include <QWebFrame>


/*
 * Program entry point.
 *
 * Parameters are forwarded to the single application.
 * If this is the first application, parameters are ignored.
 */
int main(int argc, char *argv[])
{
    int exit_result = 0;

    // Set some global application properties
    QApplication::setApplicationName("GDeskTunes");
    QApplication::setApplicationVersion("0.3-beta");
    QApplication::setOrganizationName("GearLux");

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    // Capture multi media keys
    QApplication::setAttribute(Qt::AA_CaptureMultimediaKeys);
#endif
#ifdef Q_OS_MAC
    // Make sure no icons are shown in menus on Mac
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif


    // Create a single application
    Application a(argc, argv);
    a.setObjectName("Application");

    if (a.isRunning())
    {
        // Get the command line arguments and remove the first parameter
        // which is the program name
        QStringList args = a.arguments();
        args.removeAt(0);
        qDebug() << "Send activate to other application " + args.join(" ");
        a.sendMessage("activate " + args.join(" "));
    }
    else
    {
        // Create the main window and all the components it interacts with
        qDebug() << "Create MainWindow";
        GDeskTunes *w = new GDeskTunes();
        w->setObjectName("GDeskTunes");
        w->setWindowIcon(QIcon(":/icons/gdesktunes.iconset/icon_128x128.png"));
        qDebug() << "Create LastFM";
        LastFM *last_fm = new LastFM(w);
        last_fm->setObjectName("LastFM");
        qDebug() << "Create GoogleMusicApp";
        GoogleMusicApp *app = new GoogleMusicApp(w);
        app->setObjectName("GoogleMusicApp");
        qDebug() << "Create Settings";
        Settings *settings = new Settings(w);
        settings->setObjectName("Settings");
        qDebug() << "Create CookeJar";
        CookieJar *jar = new CookieJar();
        jar->setObjectName("CookieJar");
        qDebug() << "Create SystemTrayIcon";
        SystemTrayIcon *trayIcon = new SystemTrayIcon(w);
        trayIcon->setObjectName("SystemTrayIcon");
        trayIcon->show();
        qDebug() << "Create MiniPlayer";
        MiniPlayer *miniplayer = new MiniPlayer(w);
        miniplayer->setObjectName("MiniPlayer");

        w->ui->webView->setPage(app);

#ifdef Q_OS_DARWIN
        qDebug() << "Mac application";
        MediaKeys *keys = new MediaKeys(w);
        QObject::connect(keys, SIGNAL(keyReceived(int,bool,bool)), w, SLOT(receiveMacMediaKey(int, bool, bool)));
#endif
        // Notify changes of the google app to the application
        connectSlotsByName(app, w);
        connectSlotsByName(w, app);
        connectUI(w, w);
        connectUI(w, app);

        // Connect settings and jar
        connectUI(settings, jar);

        // Connect settings and last.fm
        connectSlotsByName(app, last_fm);
        connectSlotsByName(last_fm, settings);
        connectSlotsByName(settings, last_fm);
        connectUI(settings, last_fm);

        // Connect trayIcon
        connectUI(settings, trayIcon);
        connectSlotsByName(app, trayIcon);

        // Connect the mini player
        connectSlotsByName(trayIcon, miniplayer);
        connectSlotsByName(app, miniplayer);

        // Save status on exit
        connect(&a, SIGNAL(aboutToQuit()), w, SLOT(save()));
        connect(&a, SIGNAL(aboutToQuit()), w, SLOT(saveState()));
        connect(&a, SIGNAL(aboutToQuit()), jar, SLOT(save()));
        connect(&a, SIGNAL(aboutToQuit()), last_fm, SLOT(save()));
        connect(&a, SIGNAL(aboutToQuit()), trayIcon, SLOT(save()));

        // Apply website customizations
        connect(w->ui->webView, SIGNAL(loadFinished(bool)), app, SLOT(loadFinished(bool)));

        // Special workflow for last_fm authorization
        connect(last_fm, SIGNAL(lastFMPassword(QString)), settings->ui->last_fm_password_text, SLOT(setText(QString)));
        connect(last_fm, SIGNAL(lastFMUserName(QString)), settings->ui->last_fm_user_name_text, SLOT(setText(QString)));

        // Appearance needs to be updated
        connect(w, SIGNAL(keepLinks(bool)), w, SLOT(updateAppearance()));
        connect(w, SIGNAL(keepLogo(bool)), w, SLOT(updateAppearance()));
        connect(w, SIGNAL(navigationButtons(bool)), w, SLOT(updateAppearance()));

        // Actions that need to be performed when the clear button is clicked
        connect(settings->ui->clear, SIGNAL(clicked()), jar, SLOT(deleteAllCookies()));
        connect(settings->ui->clear, SIGNAL(clicked()), w, SLOT(loadUrl()));

        // Connect ui to open dialog box
        connect(w, SIGNAL(changeSettings()), settings, SLOT(activateAndRaise()));

        // Special action for the window
        connect(w->ui->actionBring_All_To_Front, SIGNAL(triggered()), w, SLOT(activateWindow()));

        // Show the trayIcon when requested
        connect(trayIcon, SIGNAL(trayIcon(bool)), trayIcon, SLOT(setVisible(bool)));

        // State changes for the mini player
        connect(trayIcon, SIGNAL(showMainWindow()), w, SLOT(activateWindow()));
        connect(&a, SIGNAL(applicationStateChanged(Qt::ApplicationState)), miniplayer, SLOT(applicationStateChanged(Qt::ApplicationState)));

        qDebug() << "Starting application";
        w->load();
        last_fm->load();
        jar->load();
        trayIcon->load();

        NetworkManager* manager = new NetworkManager();
        manager->setCookieJar(jar);

        a.setActivationWindow(w);
        connect(&a, SIGNAL(messageReceived(const QString&)), w, SLOT(receiveMessage(const QString&)));

        QWebView *web_view = w->ui->webView;
        web_view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
        web_view->page()->setNetworkAccessManager(manager);
        web_view->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

        qDebug() << "Showing application";
        w->loadUrl();
        w->restore();
        w->show();

        exit_result = a.exec();
    }

    return exit_result;
}
