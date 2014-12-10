#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "aboutdialog.h"
#include "settings.h"

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
#include "mac/macutils.h"
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
    quitting(false),
    do_move(false)
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
    qDebug() << "Tweaking application for mac";
    ui->menuView->removeAction(ui->actionShow_menu);
#endif
#ifdef Q_OS_WIN
    qDebug() << "Tweaking application for windows";
    ui->menuView->addSeparator();
    ui->menuView->addAction(ui->actionSwitch_mini);

    // Translate
    ui->actionQuit_GDeskTunes->setText("Exit");
    ui->actionQuit_GDeskTunes->setShortcut(QKeySequence());
    ui->actionEnter_Full_Screen->setShortcut(QKeySequence("Ctrl+Shift+F"));

    ui->menuWindow->deleteLater();
#endif

#ifndef Q_OS_MAC
    // In Windows when the menu bar is hidden, shortcuts do not reach this application
    addAction(ui->actionPlay);
    addAction(ui->actionPrevious);
    addAction(ui->actionNext);
    addAction(ui->actionPreferences);
    addAction(ui->actionQuit_GDeskTunes);
    addAction(ui->actionSwitch_mini);
    addAction(ui->actionEnter_Full_Screen);
    addAction(ui->actionMinimize);
    addAction(ui->actionIncrease_Volume);
    addAction(ui->actionDecrease_Volume);
    addAction(ui->actionShow_menu);
#endif
}

#if defined Q_OS_WIN && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
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
    bool visible = ui->menuBar->isHidden();
    qDebug() << "MainWindow::switchMenu() visible=" << visible;
    setMenuVisible(visible);
}

void MainWindow::setMenuVisible(bool visible)
{
    qDebug() << "MainWindow::setMenuVisible(" << visible << ")";
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
        mii.dwTypeData = "Hide Menu Bar\tCtrl+B";
    }
    else
    {
        mii.dwTypeData = "Show Menu Bar\tCtrl+B";
    }
    ::SetMenuItemInfoA(hMenu, IDM_SHOW_MENU, false, &mii);
#endif
#if defined Q_OS_WIN && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
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
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_MediaPlay:
    {
        emit play();
    }
        break;
    case Qt::Key_MediaNext:
    {
        emit next();
    }
        break;
    case Qt::Key_MediaPrevious:
    {
        emit previous();
    }
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
   QMainWindow::keyReleaseEvent(event);
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange)
    {
        if (windowState() == Qt::WindowFullScreen)
        {
            qDebug() << "Disable mini";
            ui->actionSwitch_mini->setEnabled(false);
            ui->actionEnter_Full_Screen->setText("Exit Full Screen");
        }
        else
        {
            qDebug() << "Enable mini";
            ui->actionEnter_Full_Screen->setText("Enter Full Screen");
            ui->actionSwitch_mini->setEnabled(true);
        }

        ui->actionClose_Window->setDisabled(windowState() == Qt::WindowMinimized);
        ui->actionZoom->setDisabled(windowState() == Qt::WindowMinimized);
        ui->actionBring_All_To_Front->setDisabled(windowState() == Qt::WindowMinimized);
        ui->actionMinimize->setDisabled(windowState() == Qt::WindowMinimized);

        updateJumpList();
    }
 }

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    qDebug() << "eventFilter: " << object << " " << event;
    if (event->type() == QEvent::KeyPress)
    {
        qDebug() << object;
    }
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
            fullscreen_menu->setTitle("Enter Full Screen");
        else
            fullscreen_menu->setTitle("Exit Full Screen");
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
            switchmini_menu->setTitle("Switch from Miniplayer");
        else
            switchmini_menu->setTitle("Switch to Miniplayer");
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
            emit play();
            break;
        case 19:
            emit next();
            break;
        case 20:
            emit previous();
            break;
        }
    }
}

void MainWindow::show()
{
    // Compute the windows_offset if necessary
    if (windows_offset.isNull())
    {
        windows_offset = geometry().topLeft() - frameGeometry().topLeft();
        qDebug() << "windows offset" << windows_offset;
    }

    QMainWindow::show();
}


void MainWindow::about()
{
    QDialog *about = new AboutDialog();
    Qt::WindowFlags flags;
    flags |= Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    flags |= Qt::CustomizeWindowHint;
    about->setWindowFlags(flags);
    about->setAttribute( Qt::WA_DeleteOnClose, true );
    about->show();
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
        connect(playToolButton, SIGNAL(clicked()), this, SIGNAL(play()));

        QWinThumbnailToolButton* forwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
        forwardToolButton->setToolTip(tr("Fast forward"));
        forwardToolButton->setIcon(QIcon(":/icons/32x32/next.png"));
        connect(forwardToolButton, SIGNAL(clicked()), this, SIGNAL(next()));

        QWinThumbnailToolButton* backwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
        backwardToolButton->setToolTip(tr("Rewind"));
        backwardToolButton->setIcon(QIcon(":/icons/32x32/prev.png"));
        connect(backwardToolButton, SIGNAL(clicked()), this, SIGNAL(previous()));

        QWinThumbnailToolButton* separatorButton = new QWinThumbnailToolButton(thumbnailToolBar);
        separatorButton->setEnabled(false);

        QIcon icon(":/icons/32x32/preferences-other.png");
        QWinThumbnailToolButton* settingsButton = new QWinThumbnailToolButton(thumbnailToolBar);
        settingsButton->setToolTip(tr("Preferences"));
        settingsButton->setIcon(icon);
        connect(settingsButton, SIGNAL(clicked()), this, SIGNAL(changeSettings()));

        thumbnailToolBar->addButton(backwardToolButton);
        thumbnailToolBar->addButton(playToolButton);
        thumbnailToolBar->addButton(forwardToolButton);
        thumbnailToolBar->addButton(separatorButton);
        thumbnailToolBar->addButton(settingsButton);
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

void MainWindow::closeWindow()
{
#ifdef Q_OS_MAC
    // FIXME
    // hideMac();
#endif
}

void MainWindow::closeEvent(QCloseEvent *event)
{
#ifdef Q_OS_MAC
    if (quitting || !event->spontaneous())
    {
        event->accept();
    }
    else
    {
        // FIXME
        // hideMac();
        event->accept();
    }
#else
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    jumplist->tasks()->clear();
#endif
    event->accept();
#endif
}

void MainWindow::switchFullScreen()
{
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void MainWindow::setRepeat(QString mode)
{
    repeat_all->setChecked(mode == "LIST_REPEAT");
    repeat_one->setChecked(mode == "SINGLE_REPEAT");
    repeat_off->setChecked(mode == "NO_REPEAT");
}

void MainWindow::setShuffle(QString mode)
{
    shuffle_on->setChecked(mode == "ALL_SHUFFLE");
    shuffle_off->setChecked(mode == "NO_SHUFFLE");
}

void MainWindow::isPlaying(bool playing)
{
    ui->actionNext->setEnabled(playing);
    ui->actionStop->setEnabled(playing);
    ui->actionPrevious->setEnabled(playing);
}

void MainWindow::activateWindow()
{
    qDebug() << "MainWindow::activateWindow()";
    QMainWindow::activateWindow();
    show();
    raise();
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
