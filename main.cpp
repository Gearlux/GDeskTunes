#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
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

void restore()
{
    qDebug() << "restore()";

    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    qDebug() << "Preferences: " << settings.fileName();

}

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
        MainWindow *w = new MainWindow();
        qDebug() << "Create LastFM";
        LastFM *last_fm = new LastFM(w);
        qDebug() << "Create Settings";
        Settings *settings = new Settings(w);
        qDebug() << "Create GoogleMusicApp";
        GoogleMusicApp *app = new GoogleMusicApp(w);

        w->webView()->setPage(app);

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

        QObject::connect(&(w->options), SIGNAL(lastFMUserName(QString)), last_fm, SLOT(setLastFMUserName(QString)));
        QObject::connect(&(w->options), SIGNAL(lastFMPassword(QString)), last_fm, SLOT(setLastFMPassword(QString)));
        QObject::connect(&(w->options), SIGNAL(scrobbled(bool)), last_fm, SLOT(setScrobbled(bool)));
        QObject::connect(&(w->options), SIGNAL(autoLiked(bool)), last_fm, SLOT(setAutoLiked(bool)));

        // If last_fm is authorized, we want to know this to store the status of last_fm when quitting
        QObject::connect(last_fm, SIGNAL(authorized(bool)), &(w->options), SLOT(setAuthorized(bool)));
        // On application startup, options requests login if the previous session was authorized
        QObject::connect(&(w->options), SIGNAL(login()), last_fm, SLOT(login()));

        // If last_fm is authorized, we want the settings dialog to update the
        QObject::connect(last_fm, SIGNAL(authorized(bool)), settings, SLOT(setAuthorized(bool)));
        QObject::connect(settings, SIGNAL(login()),last_fm, SLOT(login()));
        QObject::connect(settings, SIGNAL(logout()), last_fm, SLOT(logout()));
        QObject::connect(w->ui->actionPreferences, SIGNAL(triggered()), settings, SLOT(activateAndRaise()));

        // Notify changes of the google app to the application
        QObject::connect(app, SIGNAL(repeat(QString)), w, SLOT(setRepeat(QString)));
        QObject::connect(app, SIGNAL(shuffle(QString)), w, SLOT(setShuffle(QString)));

        QObject::connect(w->webView(), SIGNAL(loadFinished(bool)), app, SLOT(loadFinished(bool)));

        qDebug() << "Starting application";
        w->restoreOptions();

        QNetworkAccessManager* manager = new QNetworkAccessManager();
        CookieJar *jar = new CookieJar();
        jar->load();
        manager->setCookieJar(jar);

        a.setActivationWindow(w);
        QObject::connect(&a, SIGNAL(messageReceived(const QString&)), w, SLOT(receiveMessage(const QString&)));

        QWebView *web_view = w->webView();
        web_view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
        web_view->page()->setNetworkAccessManager(manager);
        web_view->load(QUrl("https://play.google.com/music/listen#"));
        web_view->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
        if (w->options.isCustomized())
            w->applyStyle(w->options.getCSS());


        qDebug() << "Showing application";
        w->restore();
        w->show();

        exit_result = a.exec();

        if (w->options.saveCookies())
        {
            qDebug() << "Saving cookies";
            jar->save();
        }

        if (w->isMini())
            w->saveMini();
        else
            w->save();
    }

    return exit_result;
}
