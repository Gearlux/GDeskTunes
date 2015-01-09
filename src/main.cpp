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
#include "statemachine.h"
#include "downloader.h"

#include "ui_mainwindow.h"
#include "ui_settings.h"
#include "ui_miniplayer.h"

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
#include <QtCore/QDebug>
#include <QSettings>
#include <QtCore/QDir>
#include <QFinalState>
#include <QHistoryState>
#include <QSignalTransition>
#include <QSignalMapper>
#include <QMessageBox>

#ifdef USE_WEBKIT
#include <QWebFrame>
#include <QNetworkAccessManager>
#else
#include <QWebEngineSettings>
#endif

void ignoreSSLMessages(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toUtf8();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "%s\n", localMsg.constData());
        break;
    case QtWarningMsg:
        if (msg.startsWith("QSslSocket")) break;
        if (msg.startsWith("could not set SSL_CTRL_SET_TLSEXT_HOSTNAME")) break;
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
    fflush(stderr);
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
    QApplication::setApplicationDisplayName("GDeskTunes");
    QApplication::setApplicationVersion("0.3.4");
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
    DockedApplication* a = new DockedApplication(argc, argv);

    // All object should have a name
    a->setObjectName("Application");

    // It is possible that all windows run in the background
    a->setQuitOnLastWindowClosed(false);

    // Check if "-debug" was specified on the command line and
    // install a new error handles to suppress SSL messages
    bool debug = a->arguments().contains("-debug");
    qDebug() << "Debug mode: " << debug;
    if (debug)
    {
        qInstallMessageHandler(ignoreSSLMessages);
    }

    // Check if ssl is supported.
    // This is needed for the Google Play.
    if (!QSslSocket::supportsSsl())
    {
        QMessageBox box;
        box.setTextFormat(Qt::RichText);
        box.setIcon(QMessageBox::Critical);
        box.setText(
          QString() + "<center>"
          "Please install SSL.<br><br>" +
          "You can find an implementation at <br> " +
          "<a href='https://www.openssl.org/'>http:://www.openssl.org</a> <br><br>" +
          "Binaries are available at <br>" +
          "<a href='https://www.openssl.org/about/binaries.html'>https://www.openssl.org/about/binaries.html</a> <br><br>" +
          "More info <a href='http://qt-project.org/doc/qt-4.8/ssl.html#licensing-information'>here</a> <br><br>" +
          "Please respect all legal issues involved in using cryptography software.");
        return box.exec();
    }

    if (a->isRunning())
    {
        // Get the command line arguments and remove the first parameter
        // which is the program name
        QStringList args = a->arguments();
        args.removeAt(0);
        qDebug() << "Send activate to other application " + args.join(" ");
        a->sendMessage("activate " + args.join(" "));
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
        MiniPlayer *miniplayer = new MiniPlayer();
        miniplayer->setObjectName("MiniPlayer");

        qDebug() << "Create Downloader";
        Downloader *downloader = new Downloader(w);
        downloader->setObjectName("Downloader");

        QStateMachine*  machine = new QStateMachine();
        machine->setObjectName("StateMachine");

        QFinalState *exit_state = new QFinalState();
        machine->addState(exit_state);

        QObject::connect(machine, SIGNAL(finished()), QApplication::instance(), SLOT(quit()));

        State *running_state = new State(QState::ParallelStates);
        running_state->setObjectName("Running");
        machine->addState(running_state);
        machine->setInitialState(running_state);

#ifndef Q_OS_MAC
        // For all applications not running on mac, closing the main window exits the application
        running_state->addTransition(w, SIGNAL(closeSignal()), exit_state);
#endif
        State* mainwindow_state = new State(running_state);
        mainwindow_state->setObjectName("MainWindow");
        State* mainwindow_visible = new State(mainwindow_state);
        mainwindow_visible->setObjectName("Visible");
        State* mainwindow_hidden = new State(mainwindow_state);
        mainwindow_hidden->setObjectName("Hidden");

        mainwindow_state->setInitialState(mainwindow_visible);
        mainwindow_visible->addTransition(w, SIGNAL(hidden()), mainwindow_hidden);
        mainwindow_hidden->addTransition(w, SIGNAL(shown()), mainwindow_visible);

        State* miniplayer_state = new State(running_state);
        miniplayer_state->setObjectName("MiniPlayer");
        State* miniplayer_visible = new State(miniplayer_state);
        miniplayer_visible->setObjectName("Visible");
        State* miniplayer_hidden = new State(miniplayer_state);
        miniplayer_hidden->setObjectName("Hidden");

        miniplayer_state->setInitialState(miniplayer_hidden);
        miniplayer_visible->addTransition(miniplayer, SIGNAL(hidden()), miniplayer_hidden);
        miniplayer_hidden->addTransition(miniplayer, SIGNAL(shown()), miniplayer_visible);

        // Combination states is needed to know what to do
        // when pressing switch miniplayer
        State* combination_state = new State(running_state);
        combination_state->setObjectName("Combination");

        State *main = new State(combination_state);
        main->setObjectName("Main");
        combination_state->setInitialState(main);

        State *mini = new State(combination_state);
        mini->setObjectName("Mini");

        State *background = new State(combination_state);
        background->setObjectName("Background");

        State* mainmini = new State(combination_state);
        mainmini->setObjectName("MainMini");

        // Transitions between the combinations
        main->addTransition(w, SIGNAL(hidden()), background);
        main->addTransition(miniplayer, SIGNAL(shown()), mainmini);

        mainmini->addTransition(miniplayer, SIGNAL(hidden()), main);
        mainmini->addTransition(w, SIGNAL(hidden()), mini);

        background->addTransition(w, SIGNAL(shown()), main);
        background->addTransition(miniplayer, SIGNAL(shown()), mini);

        mini->addTransition(w, SIGNAL(shown()), mainmini);
        mini->addTransition(miniplayer, SIGNAL(hidden()), background);

        QSignalTransition *trans;
#ifdef Q_OS_MAC
        QObject::connect(a, SIGNAL(dockClicked()), w, SLOT(show()));
#endif
#ifdef Q_OS_WIN
        trans = new QSignalTransition(miniplayer, SIGNAL(windowDeactivated()));
        miniplayer_visible->addTransition(trans);
        QObject::connect(trans, SIGNAL(triggered()), miniplayer, SLOT(hideTray()));
#endif
        // Connect the UI from the web to the ui, so we only have to implement this once
        QObject::connect(app, SIGNAL(switchToMiniPlayer()), w->ui->actionSwitch_miniPlayer, SLOT(trigger()));
        QObject::connect(app, SIGNAL(switchToCompactPlayer()), w->ui->actionSwitch_mini, SLOT(trigger()));

        // How to switch between background and main
        trans = new QSignalTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()));
        main->addTransition(trans);
        QObject::connect(trans, SIGNAL(triggered()), w, SLOT(hide()));
        QObject::connect(trans, SIGNAL(triggered()), miniplayer, SLOT(show()));

        trans = new QSignalTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()));
        mainmini->addTransition(trans);
        QObject::connect(trans, SIGNAL(triggered()), w, SLOT(hide()));

        trans = new QSignalTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()));
        mini->addTransition(trans);
        QObject::connect(trans, SIGNAL(triggered()), w, SLOT(show()));
        QObject::connect(trans, SIGNAL(triggered()), miniplayer, SLOT(hide()));

        trans = new QSignalTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()));
        background->addTransition(trans);
        QObject::connect(trans, SIGNAL(triggered()), miniplayer, SLOT(show()));

        trans = new QSignalTransition(miniplayer->ui->showMain, SIGNAL(clicked()));
        mainwindow_hidden->addTransition(trans);
#ifdef Q_OS_LINUX
        QObject::connect(trans, SIGNAL(triggered()), w, SLOT(showNormal()));
#else
        QObject::connect(trans, SIGNAL(triggered()), w, SLOT(show()));
#endif

        trans = new QSignalTransition(miniplayer->ui->showMain, SIGNAL(clicked()));
        mainwindow_visible->addTransition(trans);
        QObject::connect(trans, SIGNAL(triggered()), w, SLOT(raise()));

        trans = new QSignalTransition(miniplayer->ui->showMain, SIGNAL(clicked()));
        miniplayer_visible->addTransition(trans);
        QObject::connect(trans, SIGNAL(triggered()), miniplayer, SLOT(hide()));

        trans = new QSignalTransition(w->ui->actionMiniPlayer, SIGNAL(triggered()));
        miniplayer_visible->addTransition(trans);
        QObject::connect(trans, SIGNAL(triggered()), miniplayer, SLOT(hide()));

        trans = new QSignalTransition(w->ui->actionMiniPlayer, SIGNAL(triggered()));
        miniplayer_hidden->addTransition(trans);
        QObject::connect(trans, SIGNAL(triggered()), miniplayer, SLOT(show()));

        // Update GDeskTunes window status
        mainwindow_visible->assignProperty(w->ui->actionGDeskTunes, "checked", true);
        mainwindow_hidden->assignProperty(w->ui->actionGDeskTunes, "checked", false);
        mainwindow_visible->assignProperty(w->ui->actionClose_Window, "enabled", true);
        mainwindow_hidden->assignProperty(w->ui->actionClose_Window, "enabled", false);
        mainwindow_visible->assignProperty(w->ui->actionZoom, "enabled", true);
        mainwindow_hidden->assignProperty(w->ui->actionZoom, "enabled", false);
        mainwindow_visible->assignProperty(w->ui->actionShow_Minimized, "enabled", true);
        mainwindow_hidden->assignProperty(w->ui->actionShow_Minimized, "enabled", false);

        miniplayer_visible->assignProperty(w->ui->actionMiniPlayer, "checked", true);
        miniplayer_hidden->assignProperty(w->ui->actionMiniPlayer, "checked", false);

        main->assignProperty(w->ui->actionSwitch_miniPlayer, "text", "Switch To MiniPlayer");
        mainmini->assignProperty(w->ui->actionSwitch_miniPlayer, "text", "Switch To MiniPlayer");
        background->assignProperty(w->ui->actionSwitch_miniPlayer, "text", "Switch To MiniPlayer");
        mini->assignProperty(w->ui->actionSwitch_miniPlayer, "text", "Switch From MiniPlayer");

        // Start the state machine
        machine->start();

        // Make sure the application receives the tray clicks
        a->setTrayIcon(trayIcon);
        a->addWindow(miniplayer);

        QObject::connect(miniplayer, SIGNAL(keyPressed(QKeyEvent*)), w, SLOT(keyPressEvent(QKeyEvent*)));
        QObject::connect(settings, SIGNAL(keyPressed(QKeyEvent*)), w, SLOT(keyPressEvent(QKeyEvent*)));

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
        connectUI(miniplayer, app);

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
        connect(a, SIGNAL(aboutToQuit()), trayIcon, SLOT(hide()));
        connect(a, SIGNAL(aboutToQuit()), trayIcon, SLOT(save()));
        connect(a, SIGNAL(aboutToQuit()), w, SLOT(save()));
        connect(a, SIGNAL(aboutToQuit()), w, SLOT(saveState()));
        connect(a, SIGNAL(aboutToQuit()), jar, SLOT(save()));
        connect(a, SIGNAL(aboutToQuit()), last_fm, SLOT(save()));

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
        connect(w, SIGNAL(preferences()), settings, SLOT(activateAndRaise()));

        // Special action for the window
        connect(w->ui->actionBring_All_To_Front, SIGNAL(triggered()), w, SLOT(activateWindow()));

        // Show the trayIcon when requested
        connect(trayIcon, SIGNAL(trayIcon(bool)), trayIcon, SLOT(setVisible(bool)));
        connect(trayIcon, SIGNAL(doubleClicked()), w, SLOT(bringToFront()));
        connect(trayIcon, SIGNAL(trayIcon(bool)), settings->ui->minimize_to_tray, SLOT(setEnabled(bool)));

        // Use larger album art
        connect(miniplayer->ui->album_art, SIGNAL(clicked()), miniplayer, SLOT(changeBackground()));
        connect(miniplayer, SIGNAL(changePlaybackTime(int)), app, SLOT(changePlaybackTime(int)));

        connect(w, SIGNAL(backgroundColor(QColor)), miniplayer, SLOT(setBackgroundColor(QColor)));
        connect(w, SIGNAL(miniPlayerOnTop(bool)), miniplayer, SLOT(setMiniPlayerOnTop(bool)));

        connect(miniplayer->ui->closeMini, SIGNAL(clicked(bool)), miniplayer, SLOT(hide()));
        connect(w, SIGNAL(isDraggable(bool)), w->ui->webView, SLOT(setDraggable(bool)));
        connect(trayIcon, SIGNAL(next()), app, SLOT(next()));

        qDebug() << "Starting application";
        w->load();
        last_fm->load();
        jar->load();
        trayIcon->load();

        NetworkManager* manager = new NetworkManager();
        manager->setCookieJar(jar);

        a->setActivationWindow(w);
        connect(a, SIGNAL(messageReceived(const QString&)), w, SLOT(receiveMessage(const QString&)));

        WebView *web_view = w->ui->webView;
#ifdef USE_WEBKIT
        web_view->page()->setNetworkAccessManager(manager);
        web_view->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
        web_view->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, debug);
        web_view->page()->settings()->setAttribute(QWebSettings::NotificationsEnabled, true);
        web_view->page()->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
        web_view->page()->setForwardUnsupportedContent(true);
#else
        // Here we need to set stuff to enable plugins and other things
        // On Mac, this is not supported

        // QWebEngineSettings *web_settings = QWebEngineSettings::globalSettings();
        // web_settings->setAttribute(QWebEngineSettings::PluginsEnabled, true);
#endif
        connect(web_view->page(), SIGNAL(unsupportedContent(QNetworkReply*)), downloader, SLOT(onUnsupportedContent(QNetworkReply*)));

        qDebug() << "Showing application";
        w->loadUrl();
        w->restore();
        w->show();

        exit_result = a->exec();
    }

    return exit_result;
}
