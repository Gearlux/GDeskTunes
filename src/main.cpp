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
#include <QNetworkAccessManager>
#include <QtCore/QDebug>
#include <QSettings>
#include <QtCore/QDir>
#include <QWebFrame>
#include <QFinalState>
#include <QHistoryState>
#include <QSignalTransition>
#include <QSignalMapper>
#include <QMessageBox>

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
    QApplication::setApplicationVersion("0.3.1");
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

    bool debug = a.arguments().contains("-debug");
    qDebug() << "Debug mode: " << debug;
    if (debug)
    {
        qInstallMessageHandler(ignoreSSLMessages);
    }


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


    // It is possible that all windows run in the background
    a.setQuitOnLastWindowClosed(false);

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
        MiniPlayer *miniplayer = new MiniPlayer();
        miniplayer->setObjectName("MiniPlayer");

        QStateMachine*  machine = new QStateMachine();
        machine->setObjectName("StateMachine");

        QFinalState *exit_state = new QFinalState();
        machine->addState(exit_state);

        State *main = new State();
        main->setObjectName("Main");
        machine->addState(main);
        machine->setInitialState(main);

#ifdef Q_OS_MAC
        const char *hide_main_window = SLOT(hide());
#else
        const char *hide_main_window = SLOT(showMinimized());
#endif

        // What to do when the background state is reached
        QObject::connect(main, SIGNAL(entered()), w, SLOT(bringToFront()));
        QObject::connect(main, SIGNAL(entered()), miniplayer, SLOT(hide()));

        State *maintray = new State();
        maintray->setObjectName("MainTray");
        machine->addState(maintray);

        QObject::connect(maintray, SIGNAL(entered()), miniplayer, SLOT(bringToFront()));
        QObject::connect(maintray, SIGNAL(entered()), w, SLOT(bringToFront()));

        State *mini = new State();
        mini->setObjectName("mini");
        machine->addState(mini);

        QObject::connect(mini, SIGNAL(entered()), w, hide_main_window);
        QObject::connect(mini, SIGNAL(entered()), miniplayer, SLOT(bringToFront()));

        State *background = new State();
        background->setObjectName("background");
        machine->addState(background);

        // What to do when the background state is reached
        QObject::connect(background, SIGNAL(entered()), miniplayer, SLOT(hide()));
        QObject::connect(background, SIGNAL(entered()), w, hide_main_window);

        State *tray = new State();
        tray->setObjectName("tray");
        machine->addState(tray);

        QObject::connect(tray, SIGNAL(entered()), w, hide_main_window);
        QObject::connect(tray, SIGNAL(entered()), miniplayer, SLOT(bringToFront()));

        State* mainmini = new State();
        mainmini->setObjectName("mainmini");
        machine->addState(mainmini);

        QObject::connect(mainmini, SIGNAL(entered()), w, SLOT(bringToFront()));
        QObject::connect(mainmini, SIGNAL(entered()), miniplayer, SLOT(bringToFront()));

        // How to switch between background and main
        main->addTransition(w, SIGNAL(minimized()), background);
        main->addTransition(w->ui->actionGDeskTunes, SIGNAL(triggered()), background);
        background->addTransition(w, SIGNAL(normal()), main);

        // How to switch between main and mini
        main->addTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()), mini);
        mini->addTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()), main);
        mini->addTransition(miniplayer->ui->closeMini, SIGNAL(clicked()), background);
        mini->addTransition(miniplayer->ui->showMain, SIGNAL(clicked()), main);
        mini->addTransition(trayIcon, SIGNAL(triggered()), mini);
        mini->addTransition(w, SIGNAL(normal()), mainmini);
        // LINUX
        // main->addTransition(w, SIGNAL(minimized()), mini);

        // How to switch between main and maintray
        main->addTransition(trayIcon, SIGNAL(triggered()), maintray);
        maintray->addTransition(miniplayer->ui->closeMini, SIGNAL(clicked()), main);
        maintray->addTransition(&a, SIGNAL(applicationInActivated()), main);
        maintray->addTransition(miniplayer->ui->showMain, SIGNAL(clicked()), main);
        maintray->addTransition(miniplayer->ui->showMain, SIGNAL(clicked()), main);

        // How to switch betwen background and tray
        background->addTransition(trayIcon, SIGNAL(triggered()), tray);
        tray->addTransition(miniplayer->ui->closeMini, SIGNAL(clicked()), background);
        tray->addTransition(&a, SIGNAL(applicationInActivated()), background);
        tray->addTransition(w, SIGNAL(normal()), maintray);
        tray->addTransition(miniplayer->ui->showMain, SIGNAL(clicked()), main);

        tray->addTransition(miniplayer, SIGNAL(moved()), mini);
        maintray->addTransition(miniplayer, SIGNAL(moved()), mainmini);

        // How to switch between main and mainmini
        main->addTransition(w->ui->actionMiniPlayer, SIGNAL(triggered()), mainmini);
        mainmini->addTransition(w->ui->actionMiniPlayer, SIGNAL(triggered()), main);
        mainmini->addTransition(w->ui->actionGDeskTunes, SIGNAL(triggered()), mini);
        mainmini->addTransition(miniplayer->ui->closeMini, SIGNAL(clicked()), main);
        mainmini->addTransition(w, SIGNAL(minimized()), mini);
        mainmini->addTransition(trayIcon, SIGNAL(triggered()), mainmini);
        mainmini->addTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()), mini);
        mainmini->addTransition(miniplayer->ui->showMain, SIGNAL(clicked()), main);
        mainmini->addTransition(miniplayer->ui->showMain, SIGNAL(clicked()), main);

        // How to reach the exit_state
#ifdef Q_OS_MAC
        main->addTransition(w, SIGNAL(closeSignal()), background);
        mainmini->addTransition(w, SIGNAL(closeSignal()), mini);
        maintray->addTransition(w, SIGNAL(closeSignal()), tray);
        mini->addTransition(w, SIGNAL(closeSignal()), background);

        // Make sure that if we click the dock, the application behaves correctly
        background->addTransition(&a, SIGNAL(onDockFalseFalse()), main);
        mini->addTransition(&a, SIGNAL(onDockFalseTrue()), mainmini);
        tray->addTransition(&a, SIGNAL(onDockFalseTrue()), maintray);
#else
        main->addTransition(w, SIGNAL(closeSignal()), exit_state);
        mainmini->addTransition(w, SIGNAL(closeSignal()), exit_state);
        background->addTransition(w, SIGNAL(closeSignal()), exit_state);
        mini->addTransition(w, SIGNAL(closeSignal()), exit_state);
#endif
        QObject::connect(machine, SIGNAL(finished()), QApplication::instance(), SLOT(quit()));

        // mini->addTransition(&a, SIGNAL(onDockFalseTrue()), mainmini);
        // mini_inactive->addTransition(&a, SIGNAL(onDockFalseTrue()), mainmini);
        // mini_inactive->addTransition(&a, SIGNAL(onDockFalseFalse()), mini);
        // mainmini_inactive->addTransition(&a, SIGNAL(onDockFalseFalse()), mainmini);


        // Update GDeskTunes window status
        QObject::connect(main, &State::entered, [w] { w->ui->actionGDeskTunes->setChecked(true); });
        QObject::connect(mainmini, &State::entered, [w] { w->ui->actionGDeskTunes->setChecked(true); });
        QObject::connect(mini, &State::entered, [w] { w->ui->actionGDeskTunes->setChecked(false); });
        QObject::connect(background, &State::entered, [w] { w->ui->actionGDeskTunes->setChecked(false); });

        // Update Miniplayer window status
        QObject::connect(main, &State::entered, [w] { w->ui->actionMiniPlayer->setChecked(false); });
        QObject::connect(background, &State::entered, [w] { w->ui->actionMiniPlayer->setChecked(false); });
        QObject::connect(mainmini, &State::entered, [w] { w->ui->actionMiniPlayer->setChecked(true); });
        QObject::connect(mini, &State::entered, [w] { w->ui->actionMiniPlayer->setChecked(true); });

        // Update text for mini player
        QObject::connect(main, &State::entered, [w] { w->ui->actionSwitch_miniPlayer->setText("Switch To MiniPlayer"); });
        QObject::connect(mainmini, &State::entered, [w] { w->ui->actionSwitch_miniPlayer->setText("Switch To MiniPlayer"); });
        QObject::connect(mini, &State::entered, [w] { w->ui->actionSwitch_miniPlayer->setText("Switch From MiniPlayer"); });
        QObject::connect(background, &State::entered, [w] { w->ui->actionSwitch_miniPlayer->setText("Switch To MiniPlayer"); });

        // Update close window menu item
        QObject::connect(background, &State::entered, [w] { w->ui->actionClose_Window->setEnabled(false);});
        QObject::connect(mini, &State::entered, [w] { w->ui->actionClose_Window->setEnabled(false);});
        QObject::connect(main, &State::entered, [w] { w->ui->actionClose_Window->setEnabled(true);});
        QObject::connect(mainmini, &State::entered, [w] { w->ui->actionClose_Window->setEnabled(true);});

        // Start the state machine
        machine->start();

        // Make sure the application receives the tray clicks
        connect(trayIcon, SIGNAL(triggered()), &a, SLOT(trayIconClicked()));

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
        connect(&a, SIGNAL(aboutToQuit()), trayIcon, SLOT(hide()));
        connect(&a, SIGNAL(aboutToQuit()), trayIcon, SLOT(save()));
        connect(&a, SIGNAL(aboutToQuit()), w, SLOT(save()));
        connect(&a, SIGNAL(aboutToQuit()), w, SLOT(saveState()));
        connect(&a, SIGNAL(aboutToQuit()), jar, SLOT(save()));
        connect(&a, SIGNAL(aboutToQuit()), last_fm, SLOT(save()));

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
        connect(trayIcon, SIGNAL(doubleClicked()), w, SLOT(showNormal()));
        connect(trayIcon, SIGNAL(doubleClicked()), w, SLOT(bringToFront()));
        connect(trayIcon, SIGNAL(trayIcon(bool)), settings->ui->minimize_to_tray, SLOT(setEnabled(bool)));

        // Use larger album art
        connect(miniplayer->ui->album_art, SIGNAL(clicked()), miniplayer, SLOT(changeBackground()));
        connect(miniplayer, SIGNAL(changePlaybackTime(int)), app, SLOT(changePlaybackTime(int)));

        connect(w, SIGNAL(backgroundColor(QColor)), miniplayer, SLOT(setBackgroundColor(QColor)));
        connect(w, SIGNAL(miniPlayerOnTop(bool)), miniplayer, SLOT(setMiniPlayerOnTop(bool)));

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
        web_view->page()->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, debug);

        qDebug() << "Showing application";
        w->loadUrl();
        w->restore();
        w->show();

        exit_result = a.exec();
    }

    return exit_result;
}
