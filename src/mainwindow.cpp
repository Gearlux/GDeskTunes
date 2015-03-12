#define QT_NO_DEBUG_OUTPUT

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "settings.h"
#include "gdeskstyler.h"

#include <QtCore>
#include <QtGui>
#include <QMenu>
#include <QtWebKit/QWebElement>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QWebFrame>
#else
#include <QtWebKitWidgets/QWebFrame>
#endif

#ifdef Q_OS_MAC
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
extern void qt_mac_set_dock_menu(QMenu *menu);
#endif
#endif

#ifdef Q_OS_WIN
#include "windows.h"
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
#include <QtWinExtras>
#endif
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GDeskTunes),
    windows_offset(0,0),
    draggable(false),
    hide_menu(false),
    tray_icon(false),
    minimize_to_tray(false),
    do_move(false),
    ignore_media_keys(false)
{
    ui->setupUi(this);

    shuffle_menu = ui->menuControls->addMenu("Shuffle");
    shuffle_on = shuffle_menu->addAction("On");
    shuffle_on->setObjectName("shuffle_on");
    shuffle_off = shuffle_menu->addAction("Off");
    shuffle_off->setObjectName("shuffle_off");
    shuffle_on->setCheckable(true);
    shuffle_off->setCheckable(true);

    repeat_menu = ui->menuControls->addMenu("Repeat");
    repeat_off = repeat_menu->addAction("Off");
    repeat_off->setObjectName("repeat_off");
    repeat_all = repeat_menu->addAction("All");
    repeat_all->setObjectName("repeat_all");
    repeat_one = repeat_menu->addAction("One");
    repeat_one->setObjectName("repeat_one");
    repeat_off->setCheckable(true);
    repeat_all->setCheckable(true);
    repeat_one->setCheckable(true);

    setupActions();

    createJumpList();
    createThumbnailToolBar();

#ifdef Q_OS_WIN
#define IDM_PREFERENCES 0x0010
#define IDM_SWITCH_MINI 0x0020
#define IDM_SHOW_MENU   0x0030
    qDebug() << "Adding windows system menu";
    HMENU hMenu = ::GetSystemMenu((HWND)winId(), FALSE);
    if (hMenu != NULL)
    {
        ::InsertMenuA(hMenu, 6, MF_STRING | MF_BYPOSITION, IDM_SWITCH_MINI, "Switch to MiniPlayer\tCtrl+Shift+M");
        ::InsertMenuA(hMenu, 7, MF_STRING | MF_BYPOSITION, IDM_PREFERENCES, "Preferences ...\tCtrl+,");
        ::InsertMenuA(hMenu, 8, MF_STRING | MF_BYPOSITION, IDM_SHOW_MENU, "Show Menu Bar\tCtrl+B");
        ::InsertMenuA(hMenu, 9, MF_SEPARATOR | MF_BYPOSITION, 0, 0);
    }
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupActions()
{
#ifdef Q_OS_MAC
    // qDebug() << "Tweaking application for mac";
    ui->menuView->removeAction(ui->actionSwitch_menu);
#endif
#ifdef Q_OS_WIN
    // qDebug() << "Tweaking application for windows";
    ui->menuView->addSeparator();
    ui->menuView->addAction(ui->actionSwitch_mini);

    // Translate
    ui->actionQuit_GDeskTunes->setText("Exit");
    ui->actionQuit_GDeskTunes->setShortcut(QKeySequence());
    ui->actionSwitch_Full_Screen->setShortcut(QKeySequence("Ctrl+Shift+F"));

    // ui->menuWindow->deleteLater();
#endif

#ifndef Q_OS_MAC
    // In Windows when the menu bar is hidden, shortcuts do not reach this application
    addAction(ui->actionPlay);
    addAction(ui->actionPrevious);
    addAction(ui->actionNext);
    addAction(ui->actionPreferences);
    addAction(ui->actionQuit_GDeskTunes);
    addAction(ui->actionSwitch_mini);
    addAction(ui->actionSwitch_Full_Screen);
    addAction(ui->actionShow_Minimized);
    addAction(ui->actionIncrease_Volume);
    addAction(ui->actionDecrease_Volume);
    addAction(ui->actionSwitch_menu);
#endif
}

bool MainWindow::checkAction(QKeyEvent *event, QAction *action)
{
    if (!action->isEnabled()) return false;

    QKeySequence shortcut = action->shortcut();

    int keyInt = event->key();
    Qt::Key key = static_cast<Qt::Key>(keyInt);
    if(key == Qt::Key_unknown){
        // qDebug() << "Unknown key from a macro probably";
        return false;
    }
    // the user have clicked just and only the special keys Ctrl, Shift, Alt, Meta.
    if(key == Qt::Key_Control ||
            key == Qt::Key_Shift ||
            key == Qt::Key_Alt ||
            key == Qt::Key_Meta)
    {
        // qDebug() << "Single click of special key: Ctrl, Shift, Alt or Meta";
        // qDebug() << "New KeySequence:" << QKeySequence(keyInt).toString(QKeySequence::NativeText);
        return false;
    }

    // check for a combination of user clicks
    Qt::KeyboardModifiers modifiers = event->modifiers();
    QString keyText = event->text();
    // qDebug() << "Pressed Key:" << keyText;

    if(modifiers & Qt::ShiftModifier)
        keyInt += Qt::SHIFT;
    if(modifiers & Qt::ControlModifier)
        keyInt += Qt::CTRL;
    if(modifiers & Qt::AltModifier)
        keyInt += Qt::ALT;
    if(modifiers & Qt::MetaModifier)
        keyInt += Qt::META;

    // qDebug() << "New KeySequence:" << QKeySequence(keyInt).toString(QKeySequence::NativeText);
    if (shortcut.matches(QKeySequence(keyInt)) == QKeySequence::ExactMatch)
    {
        // qDebug() << "Triggering" << action->objectName();
        action->trigger();
        return true;
    }

    return false;
}

#if defined Q_OS_WIN && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
bool MainWindow::nativeEvent(const QByteArray &, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    if (msg->message == WM_SYSCOMMAND)
      {
         if ((msg->wParam & 0xfff0) == IDM_PREFERENCES)
         {
            *result = 0;
            // open preferences dialog
            emit ui->actionPreferences->triggered();
            return (true);
         }

         if ((msg->wParam & 0xfff0) == IDM_SHOW_MENU)
         {
            *result = 0;
            switchMenu();
            return (true);
         }

         if ((msg->wParam & 0xfff0) == IDM_SWITCH_MINI)
         {
            *result = 0;
            switchMini();
            return (true);
         }
    }
    return false;
}
#endif

void MainWindow::switchMenu()
{
#ifndef Q_OS_MAC
    bool visible = ui->menuBar->isHidden();
    // qDebug() << "MainWindow::switchMenu() visible=" << visible;
    setMenuVisible(visible);
#endif
}

void MainWindow::setMenuVisible(bool visible)
{
    // qDebug() << "MainWindow::setMenuVisible(" << visible << ")";
    ui->menuBar->setVisible(visible);
    this->hide_menu = !visible;
#ifdef Q_OS_WIN
    // Change the text of the system menu
    HMENU hMenu = ::GetSystemMenu((HWND)winId(), FALSE);
    MENUITEMINFOA mii = {0};

    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STRING;
    if (visible)
    {
        mii.dwTypeData = (char*)"Hide Menu Bar\tCtrl+B";
    }
    else
    {
        mii.dwTypeData = (char*)"Show Menu Bar\tCtrl+B";
    }
    ::SetMenuItemInfoA(hMenu, IDM_SHOW_MENU, false, &mii);
#endif
#if defined Q_OS_WIN && QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    if (visible)
    {
        if (show_hide_menu)
            show_hide_menu->setTitle("Hide Menu Bar");
    }
    else
    {
        if (show_hide_menu)
            show_hide_menu->setTitle("Show Menu Bar");
    }
#endif
#ifdef Q_OS_LINUX
    if (visible)
        ui->actionSwitch_menu->setText("Hide Menu Bar");
    else
        ui->actionSwitch_menu->setText("Show Menu Bar");
#endif
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "MainWindow::keyPressEvent(" << event << ")";
    qDebug() << "   ignoreMediaKeys: " << ignore_media_keys;
    switch(event->key())
    {
    case Qt::Key_MediaTogglePlayPause:
    case Qt::Key_MediaPlay:
    {
        if (!ignore_media_keys)
        {
            ui->actionPlay->trigger();
            event->accept();
        }
    }
    break;
    case Qt::Key_MediaNext:
    {
        if (!ignore_media_keys)
        {
            ui->actionNext->trigger();
            event->accept();
        }
    }
    break;
    case Qt::Key_MediaPrevious:
    {
        if (!ignore_media_keys)
        {
            ui->actionPrevious->trigger();
            event->accept();
        }
    }
    break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
    // qDebug() << "Mainwindow::keyPressEvent() ignored.";

    const QList<QAction*> list = this->findChildren<QAction *>(QString());
    for(int i=0; i<list.size(); ++i)
    {
        QAction* action = list.at(i);
        if (checkAction(event, action))
        {
            event->accept();
            break;
        }
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
   QMainWindow::keyReleaseEvent(event);
}

void MainWindow::hideEvent(QHideEvent *event)
{
    qDebug() << "emit MainWindow::hidden()";
    emit hidden();
    QMainWindow::hideEvent(event);
}

void MainWindow::showEvent(QShowEvent *event)
{
    qDebug() << "emit MainWindow::shown()";
    emit shown();
    QMainWindow::showEvent(event);
}

void MainWindow::changeEvent(QEvent *event)
{
    qDebug() << "MainWindow::changeEvent(" << event->type()  << ")";

    switch(event->type()) {
    case QEvent::WindowStateChange:
    {
        if (this->windowState() & Qt::WindowMaximized)
        {
            qDebug() << "emit MainWindow::stateMaximized()";
            emit stateMaximized();
        }
        if (this->windowState() == Qt::WindowNoState)
        {
            qDebug() << "emit MainWindow::stateNone()";
            emit stateNone();
        }
        if (this->windowState() & Qt::WindowActive)
        {
            qDebug() << "emit MainWindow::stateActive()";
            emit stateActivated();
        }
         if (this->windowState() & Qt::WindowMinimized)
         {
            if (this->minimize_to_tray && this->tray_icon)
            {
                qDebug() << "Hide window";
                QTimer::singleShot(0, this, SLOT(hide()));
                event->ignore();
                return;
            }
            qDebug() << "emit MainWindow::stateMinimized()";
            emit stateMinimized();
         }
         if (windowState() & Qt::WindowFullScreen)
         {
             ui->actionSwitch_Full_Screen->setText("Exit Full Screen");
             qDebug() << "emit MainWindow::stateFullscreen()";
             emit stateFullscreen();
         }
         else
         {
             ui->actionSwitch_Full_Screen->setText("Enter Full Screen");
         }
         bool enabled = windowState() != Qt::WindowFullScreen;

         ui->actionClose_Window->setEnabled(enabled);
         ui->actionSwitch_mini->setEnabled(enabled);
         ui->actionSwitch_miniPlayer->setEnabled(enabled);
         ui->actionMiniPlayer->setEnabled(enabled);
         ui->actionGDeskTunes->setEnabled(enabled);

         ui->actionBring_All_To_Front->setDisabled(windowState() == Qt::WindowMinimized);
         ui->actionZoom->setEnabled(enabled && windowState() != Qt::WindowMinimized);
         ui->actionShow_Minimized->setEnabled(enabled && windowState() != Qt::WindowMinimized);

         updateJumpList();
         break;
    }
    case QEvent::ActivationChange:
        if (isActiveWindow())
        {
            qDebug() << "emit MainWindow::windowActivated()";
            emit windowActivated();
        }
        else
        {
            qDebug() << "emit MainWindow::windowDeactivated()";
            emit windowDeactivated();
        }
        break;
    default:
        break;
    }
    QMainWindow::changeEvent(event);
 }

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    qDebug() << "eventFilter: " << object << " " << event;
    return QMainWindow::eventFilter(object, event);
}

void MainWindow::populateJumpList()
{
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    change_style = new QWinJumpListItem(QWinJumpListItem::Link);
    change_style->setTitle("Preferences...");
    change_style->setIcon(QIcon(":/icons/32x32/preferences-other.png"));
    change_style->setFilePath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    change_style->setArguments(QStringList("--settings"));
    jumplist->tasks()->addItem(change_style);


    if (isMini())
    {
        show_hide_menu = 0;
        fullscreen_menu = 0;
    }
    else
    {
        show_hide_menu = new QWinJumpListItem(QWinJumpListItem::Link);
        show_hide_menu->setTitle("Show Menu Bar");
        show_hide_menu->setIcon(QIcon(":/icons/32x32/show-menu.png"));
        show_hide_menu->setFilePath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        show_hide_menu->setArguments(QStringList("--menu"));
        jumplist->tasks()->addItem(show_hide_menu);

        fullscreen_menu = new QWinJumpListItem(QWinJumpListItem::Link);
        if (windowState() == Qt::WindowFullScreen)
            fullscreen_menu->setTitle("Exit Full Screen");
        else
            fullscreen_menu->setTitle("Enter Full Screen");
        fullscreen_menu->setIcon(QIcon(":/icons/32x32/view-fullscreen.png"));
        fullscreen_menu->setFilePath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        fullscreen_menu->setArguments(QStringList("--fullscreen"));
        jumplist->tasks()->addItem(fullscreen_menu);
    }

    if (windowState() == Qt::WindowFullScreen)
    {
        switchmini_menu = 0;
    }
    else
    {
        switchmini_menu = new QWinJumpListItem(QWinJumpListItem::Link);
        if (isMini())
            switchmini_menu->setTitle("Switch from Compact Layout");
        else
            switchmini_menu->setTitle("Switch to Compact Layout ");
        switchmini_menu->setIcon(QIcon(":/icons/32x32/multimedia-player-apple-ipod.png"));
        switchmini_menu->setFilePath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
        switchmini_menu->setArguments(QStringList("--mini"));
        jumplist->tasks()->addItem(switchmini_menu);
    }

    about_menu = new QWinJumpListItem(QWinJumpListItem::Link);
    about_menu->setTitle("About GDeskTunes");
    about_menu->setIcon(QIcon(":/icons/gdesktunes.iconset/icon_32x32@2x.png"));
    about_menu->setFilePath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    about_menu->setArguments(QStringList("--about"));
    jumplist->tasks()->addItem(about_menu);

    jumplist->tasks()->setVisible(true);
#endif
}

void MainWindow::updateJumpList()
{
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    jumplist->tasks()->clear();

    populateJumpList();

#endif
}

void MainWindow::createJumpList()
{
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    jumplist = new QWinJumpList(this);

    populateJumpList();
#endif


#ifdef Q_OS_MAC
    QMenu *menu = new QMenu(this);
    menu->addAction(ui->actionPreferences);
    menu->addSeparator();
    menu->addAction(ui->actionPlay);
    menu->addAction(ui->actionPrevious);
    menu->addAction(ui->actionNext);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    qt_mac_set_dock_menu(menu);
#else
    menu->setAsDockMenu();
#endif
#endif
}

void MainWindow::receiveMacMediaKey(int key, bool repeat, bool pressed)
{
    if (pressed && !repeat)
    {
        switch(key)
        {
        case 16:
            ui->actionPlay->trigger();
            break;
        case 19:
            ui->actionNext->trigger();
            break;
        case 20:
            ui->actionPrevious->trigger();
            break;
        }
    }
}

void MainWindow::show()
{
    qDebug() << "MainWindow::show()";
    ui->searchbar->hide();

    // Compute the windows_offset if necessary
    if (windows_offset.isNull())
    {
        windows_offset = geometry().topLeft() - frameGeometry().topLeft();
        qDebug() << "windows offset" << windows_offset;
    }

    QMainWindow::show();
}

void MainWindow::raise()
{
    qDebug() << "MainWindow::raise()";
    QMainWindow::raise();

}

void MainWindow::hide()
{
    qDebug() << "MainWindow::hide()";
    QMainWindow::hide();
}


void MainWindow::about(bool show)
{
    qDebug() << "MainWindow::about(" << show << ")";
    AboutDialog *about = new AboutDialog();
    about->setAttribute( Qt::WA_DeleteOnClose, true );
    if (show)
    {
        about->show();
    }
    about->checkForUpdates();
}

void MainWindow::createThumbnailToolBar()
{
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    if (QtWin::isCompositionEnabled())
    {
        // Needed to get the buttons to show
        QMenu * menu = new QMenu(this);
        menu->setHidden(true);
        QtWin::enableBlurBehindWindow(menu);

        QWinThumbnailToolBar* thumbnailToolBar = new QWinThumbnailToolBar(this);
        thumbnailToolBar->setWindow(windowHandle());

        QWinThumbnailToolButton* playToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
        playToolButton->setToolTip(tr("Play"));
        playToolButton->setIcon(QIcon(":/icons/32x32/play.png"));
        connect(playToolButton, SIGNAL(clicked()), ui->actionPlay, SIGNAL(triggered()));

        QWinThumbnailToolButton* forwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
        forwardToolButton->setToolTip(tr("Fast forward"));
        forwardToolButton->setIcon(QIcon(":/icons/32x32/next.png"));
        connect(forwardToolButton, SIGNAL(clicked()), ui->actionNext, SIGNAL(triggered()));

        QWinThumbnailToolButton* backwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
        backwardToolButton->setToolTip(tr("Rewind"));
        backwardToolButton->setIcon(QIcon(":/icons/32x32/prev.png"));
        connect(backwardToolButton, SIGNAL(clicked()), ui->actionPrevious, SIGNAL(triggered()));

        /*
         * Keep this code, if we want to pimp the mini player more.
         *
        QWinThumbnailToolButton* separatorButton = new QWinThumbnailToolButton(thumbnailToolBar);
        separatorButton->setEnabled(false);

        QIcon icon(":/icons/32x32/preferences-other.png");
        QWinThumbnailToolButton* settingsButton = new QWinThumbnailToolButton(thumbnailToolBar);
        settingsButton->setToolTip(tr("Preferences"));
        settingsButton->setIcon(icon);
        connect(settingsButton, SIGNAL(clicked()), ui->actionPreferences, SIGNAL(changeSettings()));
        */

        thumbnailToolBar->addButton(backwardToolButton);
        thumbnailToolBar->addButton(playToolButton);
        thumbnailToolBar->addButton(forwardToolButton);
        /*
        thumbnailToolBar->addButton(separatorButton);
        thumbnailToolBar->addButton(settingsButton);
        */
    }
#endif
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (draggable)
    {
        mouse_click_x_coordinate = event->x();
        mouse_click_y_coordinate = event->y();
        do_move = true;
        event->accept();
    }
    else
        QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (draggable && do_move)
    {
        this->move(event->globalX() - mouse_click_x_coordinate, event->globalY() - mouse_click_y_coordinate);
        event->accept();
    }
    else
    {
        QMainWindow::mouseMoveEvent(event);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    do_move = false;
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::quitGDeskTunes()
{
   QCoreApplication::exit(0);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug() << "MainWindow::closeEvent("<< event->spontaneous() << ")";
#ifdef Q_OS_MAC
    // FIXME MAC: actually the button needs to be disabled
    if (isFullScreen())
    {
        QTimer::singleShot(250, this, SLOT(restore()));
        event->accept();
        return;
    }
    event->ignore();
    hide();
#else
    #if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
        // I hate it when the jumplist keeps the commands of a not running system
        jumplist->tasks()->clear();
    #endif
    emit closeSignal();
    event->accept();
#endif

}

void MainWindow::showFullScreen()
{
    qDebug() << "MainWindow::showFullScreen()";
    QMainWindow::showFullScreen();
}

void MainWindow::showNormal()
{
    qDebug() << "MainWindow::showNormal()";
    QMainWindow::showNormal();
}

void MainWindow::showMinimized()
{
    qDebug() << "MainWindow::showMinimized()";
    QMainWindow::showMinimized();
}

void MainWindow::switchFullScreen()
{
     qDebug() << "MainWindow::switchFullScreen()";
    if (isFullScreen())
    {
        qDebug() << windowState() << windowFlags();

        if (isMaximized())
            showMaximized();
        else
            showNormal();
    }
    else
    {
        showFullScreen();
    }
}

void MainWindow::repeat(QString mode)
{
    repeat_all->setChecked(mode == "LIST_REPEAT");
    repeat_one->setChecked(mode == "SINGLE_REPEAT");
    repeat_off->setChecked(mode == "NO_REPEAT");
}

void MainWindow::shuffle(QString mode)
{
    shuffle_on->setChecked(mode == "ALL_SHUFFLE");
    shuffle_off->setChecked(mode == "NO_SHUFFLE");
}

void MainWindow::isPlaying(int mode)
{
    ui->actionPlay->setEnabled(mode != 0);
    if (mode == 1)
        ui->actionPlay->setText("Play");
    else
        ui->actionPlay->setText("Pause");
}

void MainWindow::rewindEnabled(int mode)
{
    qDebug() << "MainWindow::forwardEnabled(" << mode << ")";
   ui->actionPrevious->setEnabled(mode != 0);
}

void MainWindow::forwardEnabled(int mode)
{
    qDebug() << "MainWindow::forwardEnabled(" << mode << ")";
    ui->actionNext->setEnabled(mode != 0);
}

void MainWindow::zoom()
{
    if (isMaximized())
    {
        showNormal();
    }
    else
    {
        showMaximized();
    }
}

// Needed to bootstrap the windows jump list
bool MainWindow::isMini()
{
    return false;
}

// Needed to make a slot
void MainWindow::activateWindow()
{
    qDebug() << "MainWindow::activateWindow()";
    QMainWindow::activateWindow();
}

void MainWindow::closeWindow()
{
    qDebug() << "MainWindow::closeWindow()";
    close();
}

void MainWindow::saveState()
{
    // qDebug() << "GDeskTunes::saveState()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    if (isMini())
    {
        settings.setValue("miniGeometry", saveGeometry());
    }
    else
    {
        settings.setValue("geometry", saveGeometry());
        settings.setValue("state", QMainWindow::saveState());
    }
}

void MainWindow::restore()
{
    // qDebug() << "GDeskTunes::restore()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    qDebug() << "Preferences: " << settings.fileName();

    QString key = "geometry";
    if (settings.contains(key))
    {
        restoreGeometry(settings.value(key).toByteArray());
    }
    key = "state";
    if (settings.contains(key))
    {
        restoreState(settings.value(key).toByteArray());
    }
}

void MainWindow::bringToFront()
{
#ifdef Q_OS_WIN
    // ::SetWindowPos((HWND)effectiveWinId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    // ::SetWindowPos((HWND)effectiveWinId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    if (this->minimize_to_tray && this->tray_icon)
        show();
#endif

    if (!isFullScreen())
        showNormal();
    raise();
    activateWindow();
}

void MainWindow::on_actionGDeskTunes_triggered()
{
    qDebug() << "MainWindow::on_actionGDeskTunes_triggered()";
    qDebug() << isVisible() << isMinimized();
    if (isVisible() && !isMinimized())
    {
        hide();
    }
    else
    {
        if (isMinimized())
            showNormal();
        else
            show();
    }
}

void MainWindow::on_actionStyleEditor_triggered()
{
    qDebug() << "Showing editor";
    GDeskStyler* styler = new GDeskStyler(0);
    styler->show();
    styler->setGDeskTunes(this);
}


