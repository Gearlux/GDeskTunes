#include "gdesktunes.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "ui_settings.h"
#include "googlemusicapp.h"
#include "lastfm.h"
#include "cookiejar.h"

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
#include <QtWinExtras>
#endif
#include "logitech.h"
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
    QApplication::setApplicationVersion("0.2");
    QApplication::setOrganizationName("GearLux");

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    // Capture multi media keys
    QApplication::setAttribute(Qt::AA_CaptureMultimediaKeys);
#endif
#ifdef Q_OS_MAC
    // Make sure no icons are shown in menus on Mac
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

#ifdef Q_OS_WIN
    // FIXME
    // Check if the Logitech SetPoint application is configured correctly
    Logitech::check();
#endif

    // Create a single application
    QtSingleApplication a(argc, argv);

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
        qDebug() << "Creating components";
        qDebug() << "Create MainWindow";
        GDeskTunes *w = new GDeskTunes();
        w->setWindowIcon(QIcon(":/icons/gdesktunes.iconset/icon_128x128.png"));
        qDebug() << "Create LastFM";
        LastFM *last_fm = new LastFM(w);
        qDebug() << "Create GoogleMusicApp";
        GoogleMusicApp *app = new GoogleMusicApp(w);
        qDebug() << "Create Settings";
        Settings *settings = new Settings(w);
        qDebug() << "Create CookeJar";
        CookieJar *jar = new CookieJar();

        w->ui->webView->setPage(app);

        QObject::connect(w->shuffle_off, SIGNAL(triggered()), app, SLOT(shuffleOff()));
        QObject::connect(w->shuffle_on, SIGNAL(triggered()), app, SLOT(shuffleOn()));
        QObject::connect(w->repeat_off, SIGNAL(triggered()), app, SLOT(repeatOff()));
        QObject::connect(w->repeat_one, SIGNAL(triggered()), app, SLOT(repeatOne()));
        QObject::connect(w->repeat_all, SIGNAL(triggered()), app, SLOT(repeatOne()));
        QObject::connect(w->ui->actionIncrease_Volume, SIGNAL(triggered()), app, SLOT(increaseVolume()));
        QObject::connect(w->ui->actionDecrease_Volume, SIGNAL(triggered()), app, SLOT(decreaseVolume()));

#ifdef Q_OS_DARWIN
        qDebug() << "Mac application";
        MediaKeys *keys = new MediaKeys(w);
        QObject::connect(keys, SIGNAL(keyReceived(int,bool,bool)), w, SLOT(receiveMacMediaKey(int, bool, bool)));
#endif
        qDebug() << "Connecting components";
        QObject::connect(app, SIGNAL(nowPlaying(QString, QString, QString, int)), last_fm, SLOT(nowPlaying(QString,QString,QString,int)));
        QObject::connect(app, SIGNAL(love(QString, QString, QString)), last_fm, SLOT(love(QString,QString,QString)));
        QObject::connect(app, SIGNAL(unlove(QString, QString, QString)), last_fm, SLOT(unlove(QString,QString,QString)));

        // If last_fm is authorized, we want to know this to store the status of last_fm when quitting
        QObject::connect(last_fm, SIGNAL(authorized(bool)), settings, SLOT(setAuthorized(bool)));
        // On application startup, options requests login if the previous session was authorized
        QObject::connect(settings, SIGNAL(login(QString, QString)), last_fm, SLOT(login(QString, QString)));
        QObject::connect(settings, SIGNAL(logout()), last_fm, SLOT(logout()));

        // If last_fm is authorized, we want the settings dialog to update the
        QObject::connect(last_fm, SIGNAL(authorized(bool)), settings, SLOT(setAuthorized(bool)));
        QObject::connect(last_fm, SIGNAL(scrobbled(bool)), settings->ui->scrobble, SLOT(setChecked(bool)));
        QObject::connect(settings->ui->scrobble, SIGNAL(toggled(bool)), last_fm, SLOT(setScrobbled(bool)));
        QObject::connect(last_fm, SIGNAL(autoLiked(bool)), settings->ui->auto_love, SLOT(setChecked(bool)));
        QObject::connect(settings->ui->auto_love, SIGNAL(toggled(bool)), last_fm, SLOT(setAutoLiked(bool)));
        QObject::connect(last_fm, SIGNAL(lastFMPassword(QString)), settings->ui->last_fm_password_text, SLOT(setText(QString)));
        QObject::connect(last_fm, SIGNAL(lastFMUserName(QString)), settings->ui->last_fm_user_name_text, SLOT(setText(QString)));
        QObject::connect(jar, SIGNAL(dontSaveCookies(bool)), settings->ui->cookies, SLOT(setChecked(bool)));
        QObject::connect(settings->ui->cookies, SIGNAL(checked(bool)), jar, SLOT(setDontSaveCookies(bool)));
        QObject::connect(w->ui->actionPreferences, SIGNAL(triggered()), settings, SLOT(activateAndRaise()));

        // Notify changes of the google app to the application
        QObject::connect(app, SIGNAL(repeat(QString)), w, SLOT(setRepeat(QString)));
        QObject::connect(app, SIGNAL(shuffle(QString)), w, SLOT(setShuffle(QString)));
        QObject::connect(app, SIGNAL(isPlaying(bool)), w, SLOT(isPlaying(bool)));

        QObject::connect(w->ui->webView, SIGNAL(loadFinished(bool)), app, SLOT(loadFinished(bool)));

        // Connect the ui and the application
        QObject::connect(w->ui->actionPlay, SIGNAL(triggered()), app, SLOT(play()));
        QObject::connect(w->ui->actionPrevious, SIGNAL(triggered()), app, SLOT(previous()));
        QObject::connect(w->ui->actionNext, SIGNAL(triggered()), app, SLOT(next()));
        QObject::connect(w->ui->actionAbout, SIGNAL(triggered()), w, SLOT(about()));
        QObject::connect(w->ui->actionSwitch_mini, SIGNAL(triggered()), w, SLOT(switchMini()));
        QObject::connect(w->ui->actionQuit_GDeskTunes, SIGNAL(triggered()), w, SLOT(quitGDeskTunes()));
        QObject::connect(w->ui->actionEnter_Full_Screen, SIGNAL(triggered()), w, SLOT(switchFullScreen()));
        QObject::connect(w->ui->actionMinimize, SIGNAL(triggered()), w, SLOT(showMinimized()));
        QObject::connect(w->ui->actionZoom, SIGNAL(triggered()), w, SLOT(zoom()));
        QObject::connect(w->ui->actionBring_All_To_Front, SIGNAL(triggered()), w, SLOT(activateWindow()));
 #ifndef Q_OS_MAC
        QObject::connect(w->ui->actionShow_menu, SIGNAL(triggered()), w, SLOT(switchMenu()));
#endif
#ifndef Q_OS_WIN
        QObject::connect(w->ui->actionClose_Window, SIGNAL(triggered()), w, SLOT(closeWindow()));
#endif
        QObject::connect(&a, SIGNAL(aboutToQuit()), w, SLOT(save()));
        QObject::connect(&a, SIGNAL(aboutToQuit()), w, SLOT(saveState()));
        QObject::connect(&a, SIGNAL(aboutToQuit()), jar, SLOT(save()));
        QObject::connect(&a, SIGNAL(aboutToQuit()), last_fm, SLOT(save()));

        QObject::connect(w, SIGNAL(play()), app, SLOT(play()));
        QObject::connect(w, SIGNAL(next()), app, SLOT(next()));
        QObject::connect(w, SIGNAL(previous()), app, SLOT(previous()));
        QObject::connect(w, SIGNAL(changeSettings()), settings, SLOT(activateAndRaise()));

        qDebug() << "Starting application";
        w->load();
        last_fm->load();
        jar->load();

        QNetworkAccessManager* manager = new QNetworkAccessManager();
        manager->setCookieJar(jar);

        a.setActivationWindow(w);
        QObject::connect(&a, SIGNAL(messageReceived(const QString&)), w, SLOT(receiveMessage(const QString&)));

        QWebView *web_view = w->ui->webView;
        web_view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
        web_view->page()->setNetworkAccessManager(manager);
        web_view->load(QUrl("https://play.google.com/music/listen#"));
        web_view->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

        qDebug() << "Showing application";
        w->restore();
        w->show();

        exit_result = a.exec();
    }

    return exit_result;
}
