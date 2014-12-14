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

void ignoreSSLMessages(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        if (msg.startsWith("QSslSocket")) break;
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
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

    // Ignore ssl messages
    // qInstallMessageHandler(ignoreSSLMessages);

    // Set some global application properties
    QApplication::setApplicationName("GDeskTunes");
    QApplication::setApplicationVersion("0.3");
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
        MiniPlayer *miniplayer = new MiniPlayer(w);
        miniplayer->setObjectName("MiniPlayer");

        QStateMachine*  machine = new QStateMachine();
        machine->setObjectName("StateMachine");

        State *main = new State();
        main->setObjectName("Main");

        const char* show_slot = SLOT(show());
        const char* hide_slot = SLOT(hide());

#ifdef Q_OS_WIN
        hide_slot = SLOT(showMinimized());
#endif

        QObject::connect(main, SIGNAL(entered()), w, show_slot);
        QObject::connect(main, SIGNAL(entered()), w, SLOT(raise()));
        QObject::connect(main, SIGNAL(entered()), miniplayer, SLOT(hide()));

        State* background = new State();
        background->setObjectName("Background");

        QObject::connect(background, SIGNAL(entered()), w, hide_slot);
        QObject::connect(background, SIGNAL(entered()), miniplayer, SLOT(hide()));

        State* mainmini = new State();
        mainmini->setObjectName("MainMini");

        QObject::connect(mainmini, SIGNAL(entered()), w, show_slot);
        QObject::connect(mainmini, SIGNAL(entered()), w, SLOT(raise()));
        QObject::connect(mainmini, SIGNAL(entered()), miniplayer, SLOT(show()));
        QObject::connect(mainmini, SIGNAL(entered()), miniplayer, SLOT(raise()));

        State* mini = new State();
        mini->setObjectName("mini");

        QObject::connect(mini, SIGNAL(entered()), w, hide_slot);
        QObject::connect(mini, SIGNAL(entered()), miniplayer, SLOT(show()));
        QObject::connect(mini, SIGNAL(entered()), miniplayer, SLOT(raise()));

        QFinalState *exit_state = new QFinalState();

        State* mini_inactive = new State();
        mini_inactive->setObjectName("miniInactive");

        State* main_inactive = new State();
        main_inactive->setObjectName("mainInactive");

        State* mainmini_inactive = new State();
        mainmini_inactive->setObjectName("mainminiInactive");

        machine->addState(main);
        machine->addState(background);
        machine->addState(mainmini);
        machine->addState(mini);
        machine->addState(mini_inactive);
        machine->addState(mainmini_inactive);
        machine->addState(main_inactive);
        machine->addState(exit_state);
        machine->setInitialState(main);

#ifdef Q_OS_MAC
        main->addTransition(w, SIGNAL(closeSignal()), background);
        main_inactive->addTransition(w, SIGNAL(closeSignal()), background);
#else
        main->addTransition(w, SIGNAL(closeSignal()), exit_state);
        QObject::connect(machine, SIGNAL(finished()), QApplication::instance(), SLOT(quit()));
#endif

        // Transitions from active to inactive
        mini->addTransition(&a, SIGNAL(applicationInActivated()), mini_inactive);
        mainmini->addTransition(&a, SIGNAL(applicationInActivated()), mainmini_inactive);
        main->addTransition(&a, SIGNAL(applicationInActivated()), main_inactive);

        mini_inactive->addTransition(&a, SIGNAL(applicationActivated()), mini);
        main_inactive->addTransition(&a, SIGNAL(applicationActivated()), main);
        mainmini_inactive->addTransition(&a, SIGNAL(applicationActivated()), mainmini);

        // Transitions on using Window > Zoom
        background->addTransition(w->ui->actionZoom, SIGNAL(triggered()), main);
        mini->addTransition(w->ui->actionZoom, SIGNAL(triggered()), mainmini);

        background->addTransition(w->ui->actionSwitch_Full_Screen, SIGNAL(triggered()), main);
        mini->addTransition(w->ui->actionSwitch_Full_Screen, SIGNAL(triggered()), mainmini);

        // Miscelaneous
        background->addTransition(w->ui->actionSwitch_mini, SIGNAL(triggered()), main);
        background->addTransition(w->ui->actionShow_Minimized, SIGNAL(triggered()), main);

        // Transitions when displaying the mini player
        background->addTransition(w->ui->actionMiniPlayer, SIGNAL(triggered()), mini);
        main->addTransition(w->ui->actionMiniPlayer, SIGNAL(triggered()), mainmini);
        mainmini->addTransition(w->ui->actionMiniPlayer, SIGNAL(triggered()), main);
        mini->addTransition(w->ui->actionMiniPlayer, SIGNAL(triggered()), background);

        // Transitions when switching the mini player
        main->addTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()), mini);
        mini->addTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()), main);
        mainmini->addTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()), mini);
        background->addTransition(w->ui->actionSwitch_miniPlayer, SIGNAL(triggered()), mini);

        // Transitions when displaying the main window
        mini->addTransition(w->ui->actionGDeskTunes, SIGNAL(triggered()), mainmini);
        mainmini->addTransition(w->ui->actionGDeskTunes, SIGNAL(triggered()), mini);
        main->addTransition(w->ui->actionGDeskTunes, SIGNAL(triggered()), background);
        background->addTransition(w->ui->actionGDeskTunes, SIGNAL(triggered()), main);

        // Transitions when closing the mini player
        mini->addTransition(miniplayer->ui->closeMini, SIGNAL(clicked()), background);
        mainmini->addTransition(miniplayer->ui->closeMini, SIGNAL(clicked()), main);

        // Transitions when closing the main window
        mainmini->addTransition(w, SIGNAL(closeSignal()), mini);
        mainmini_inactive->addTransition(w, SIGNAL(closeSignal()), mini);

        // Clicking the tray icon, should popup or close the mini player at the tray
        main->addTransition(trayIcon, SIGNAL(triggered()), mainmini);
        mainmini->addTransition(trayIcon, SIGNAL(triggered()), main);
        mainmini_inactive->addTransition(trayIcon, SIGNAL(triggered()), mainmini);
        main_inactive->addTransition(trayIcon, SIGNAL(triggered()), mainmini);
        mini->addTransition(trayIcon, SIGNAL(triggered()), background);
        mini_inactive->addTransition(trayIcon, SIGNAL(triggered()), mini);
        background->addTransition(trayIcon, SIGNAL(triggered()), mini);

        // FIXME: could not get this to work
        QSignalTransition *trans = new QSignalTransition(w, SIGNAL(closeSignal()));
        background->addTransition(trans);
        QObject::connect(trans, SIGNAL(triggered()), w, SLOT(hide()));

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

        // Make sure that if we click the dock, the application behaves correctly
        background->addTransition(&a, SIGNAL(onDockFalseFalse()), main);
        mini->addTransition(&a, SIGNAL(onDockFalseTrue()), mainmini);
        mini_inactive->addTransition(&a, SIGNAL(onDockFalseTrue()), mainmini);
        mini_inactive->addTransition(&a, SIGNAL(onDockFalseFalse()), mini);
        mainmini_inactive->addTransition(&a, SIGNAL(onDockFalseFalse()), mainmini);

        // Start the state machine
        machine->start();

        // Make sure the application receives the tray clicks
        connect(trayIcon, SIGNAL(triggered()), &a, SLOT(trayIconClicked()));

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
        connect(&a, SIGNAL(aboutToQuit()), trayIcon, SLOT(hide()));

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

        // Use larger album art
        connect(miniplayer->ui->album_art, SIGNAL(clicked()), miniplayer, SLOT(changeBackground()));
        connect(miniplayer->ui->maximize, SIGNAL(clicked()), miniplayer, SLOT(changeBackground()));

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
