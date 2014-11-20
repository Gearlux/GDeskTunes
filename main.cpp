#include "mainwindow.h"
#include "googlemusicapp.h"
#include "lastfm.h"
#include "cookiejar.h"
#include "qtsingleapplication.h"
#include <QApplication>
#include <QtNetwork/QNetworkCookie>
#include <QNetworkAccessManager>
#include <QtCore/QDebug>
#include <QSettings>
#include <QtCore/QDir>

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
#include <QtWinExtras>
#endif
#include "logitech.h"
#endif

#ifdef Q_OS_DARWIN
#include "mac/mediakeys.h"
#endif

int main(int argc, char *argv[])
{
    int exit_result = 0;

    QApplication::setApplicationName("GDeskTunes");
    QApplication::setApplicationVersion("0.1");
    QApplication::setOrganizationName("GearLux");

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QApplication::setAttribute(Qt::AA_CaptureMultimediaKeys);
#endif
#ifdef Q_OS_MAC
    QApplication::setAttribute(Qt::AA_DontShowIconsInMenus);
    // QApplication::setAttribute(Qt::AA_MacPluginApplication);
#endif

#ifdef Q_OS_WIN
    Logitech::check();
#endif
    QtSingleApplication a(argc, argv);

    QStringList args = a.arguments();

    bool single = !args.contains("-multiple");

    if (single && a.isRunning())
    {
        args.removeAt(0);
        qDebug() << "Send activate to other application " + args.join(" ");
        a.sendMessage("activate " + args.join(" "));
    }
    else
    {
        MainWindow w;
        w.restoreOptions();

        QNetworkAccessManager* manager = new QNetworkAccessManager();
        CookieJar *jar = new CookieJar();
        jar->load();
        manager->setCookieJar(jar);

        a.setActivationWindow(&w);
        QObject::connect(&a, SIGNAL(messageReceived(const QString&)), &w, SLOT(receiveMessage(const QString&)));

        QWebView *web_view = w.webView();
        web_view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
        web_view->page()->setNetworkAccessManager(manager);
        web_view->load(QUrl("https://play.google.com/music/listen#"));
        web_view->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
        if (w.options.isCustomized())
            w.applyStyle(w.options.getCSS());
        w.restore();
        w.show();

#ifdef Q_OS_DARWIN
        qDebug() << "Mac application";
        MediaKeys *keys = new MediaKeys(web_view);
        QObject::connect(keys, SIGNAL(keyReceived(int,bool,bool)), &w, SLOT(receiveMacMediaKey(int, bool, bool)));
#endif
        QObject::connect(w.app, SIGNAL(nowPlaying(QString, QString, QString, int)), w.last_fm, SLOT(scrobble(QString,QString,QString,int)));

        exit_result = a.exec();

        if (w.options.saveCookies())
        {
            qDebug() << "Saving cookies";
            jar->save();
        }

        if (w.isMini())
            w.saveMini();
        else
            w.save();
    }

    return exit_result;
}
