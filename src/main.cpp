#include "gdesktunes.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "ui_settings.h"
#include "googlemusicapp.h"
#include "lastfm.h"
#include "cookiejar.h"
#include "networkmanager.h"
#include "systemtrayicon.h"
#include "miniplayer.h"
#include "ui_miniplayer.h"
#include "application.h"

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

QSet<QString> connected;

void connect(const QObject *sender, const char* signal,
                                const QObject *receiver, const char* method,
                                Qt::ConnectionType type = Qt::AutoConnection, bool log = true)
{
    QString key;
    QTextStream stream(&key);

    // FIXME: Use QMetaObject::normalized
    stream << sender->objectName()  << "." << QString(signal).remove(QRegExp("(^[1,2]|\\s+)")) << " -> " << receiver->objectName() << "." << QString(method).remove(QRegExp("(^[1,2]|\\s+)"));
    if (connected.contains(key))
    {
        qDebug() << "Not connecting: " << key;
        return;
    }
    if (log)
        qDebug() << "Connecting: " << key;
    connected.insert(key);
    QObject::connect(sender, signal, receiver, method, type);
}

void connect(const QObject *sender, const QMetaMethod &signal,
                                const QObject *receiver, const QMetaMethod &method,
                                Qt::ConnectionType type = Qt::AutoConnection, bool log = true)
{
    QString key;
    QTextStream stream(&key);

    // FIXME: Use QMetaObject::normalized
    stream << sender->objectName()  << "." << QString(signal.methodSignature()).remove(QRegExp("(^[1,2]|\\s+)")) << " -> " << receiver->objectName() << "." <<QString(method.methodSignature()).remove(QRegExp("(^[1,2]|\\s+)"));

    if (connected.contains(key))
    {
        qDebug() << "Not connecting: " << key;
        return;
    }
    if (log)
        qDebug() << "Connecting: " << key;
    connected.insert(key);
    QObject::connect(sender, signal, receiver, method, type);
}

void connectUI(QObject *ui, QObject *object)
{
    if (!ui) return;
    if (!object) return;

    const QMetaObject *mobject = object->metaObject();

    const QObjectList list = ui->findChildren<QObject *>(QString());

    for(int o=0; o<list.length(); ++o)
    {
        const QObject *ui_object = list.at(o);
        const QMetaObject *mui = ui_object->metaObject();

        QString objectName = ui_object->objectName();
        int underscore = objectName.indexOf('_');
        while(underscore != -1)
        {
            objectName.remove(underscore, 1);
            objectName[underscore] = objectName[underscore].toUpper();
            underscore = objectName.indexOf('_');
        }

        QString slot_method, argument;
        int signal_index = -1;

        // Not the best programming practice, but this code is more elegant than solving it with if/else
        if (qobject_cast<const QAction*>(ui_object) != 0) goto _connect_action;

        // Connect GUI elements
        signal_index = mui->indexOfSignal("clicked()");
        if (signal_index != -1) { slot_method = QString::null; argument = "()"; goto _connect;}

        signal_index = mui->indexOfSignal("toggled(bool)");
        if (signal_index != -1) { slot_method = "setChecked(bool)"; argument = "(bool)"; goto _connect;}

        signal_index = mui->indexOfSignal("currentIndexChanged(QString)");
        if (signal_index != -1) { slot_method = "setCurrentText(QString)"; argument = "(QString)"; goto _connect;}

_connect:
        if (signal_index != -1)
        {
            QString setter;
            if (slot_method == QString::null)
            {
                setter = objectName + argument;
            }
            else
            {
                setter = "set" + objectName + argument;
                setter[3] = setter[3].toUpper();
            }

            int slot_index = mobject->indexOfMethod(setter.toUtf8().constData());
            if (slot_index != -1)
            {
                connect(ui_object, mui->method(signal_index), object, mobject->method(slot_index));
            }
        }

        if (slot_method == QString::null) continue;

        signal_index = mobject->indexOfSignal((objectName + argument).toUtf8().constData());
        if (signal_index != -1)
        {
            int slot_index = mui->indexOfSlot(slot_method.toUtf8().constData());
            if (slot_index != -1)
            {
                connect(object, mobject->method(signal_index), ui_object, mui->method(slot_index));
            }
        }
        // No need to check if this is an action
        continue;

        // Connect actions
_connect_action:
        signal_index = mui->indexOfSignal("triggered()");
        if (signal_index != -1)
        {
            QString slotMethod;
            if (objectName.startsWith("action"))
                slotMethod = objectName.mid(6);
            else
                slotMethod = objectName;
            slotMethod[0] = slotMethod[0].toLower();

            int slot_index = mobject->indexOfMethod((slotMethod + "()").toUtf8().constData());
            if (slot_index != -1)
            {
                connect(ui_object, mui->method(signal_index), object, mobject->method(slot_index));
            }
        }
    }
}

void connectSlotsByName(QObject *sender, QObject *receiver)
{
    if (!sender) return;
    if (!receiver) return;

    const QMetaObject *msender = sender->metaObject();
    const QMetaObject *mreceiver = receiver->metaObject();

    // list of all objects to look for matching signals including...
    // all children of 'o'...
    // and the object 'o' itself
    // const QObjectList list = o->findChildren<QObject *>(QString()) << o;

    for(int s=0; s<msender->methodCount(); ++s)
    {
        QMetaMethod ms = msender->method(s);
        if (ms.methodType() != QMetaMethod::Signal) continue;
        const QByteArray signalSignature = ms.methodSignature();
        const char *signal = signalSignature.constData();

        if (strncmp(signal,"destroyed", 9) == 0) continue;
        if (strncmp(signal,"deleteLater", 11) == 0) continue;
        if (strncmp(signal,"objectNameChanged", 16) == 0) continue;
        if (strncmp(signal,"_q_reregisterTimers", 18) == 0) continue;

        for(int m=0; m<mreceiver->methodCount(); ++m)
        {
            QMetaMethod mr = mreceiver->method(m);
            if (mr.methodType() != QMetaMethod::Slot) continue;
            const QByteArray slotSignature = mr.methodSignature();
            const char *slot = slotSignature.constData();

            if (strcmp(signal, slot) == 0)
            {
                connect(sender, ms, receiver, mr, Qt::AutoConnection, true);
            }
        }
    }

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
