#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "googlemusicapp.h"
#include "settings.h"
#include "options.h"
#include "aboutdialog.h"
#include "lastfm.h"

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
#include "mac/macutils.h"
#endif

#ifdef Q_OS_WIN
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
#include <QtWinExtras>
#endif
#include "windows.h"
#endif

//
// This class only serves to route the events from the menu
// to the main window.
//
// A menu is added to support the thumbnail bar in windows 7
//
class Menu: public QMenu
{
public:
    Menu(MainWindow *parent) : QMenu(parent), window(parent)
    {
    }

    void keyPressEvent(QKeyEvent *event)
    {
        window->keyPressEvent(event);
    }

private:
    MainWindow *window;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    options(this),
    ui(new Ui::GDeskTunes),
    mini(false),
    quitting(false),
    windows_offset(0,0),
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

    // Setup webview and windows interaction
    connect(webView(), SIGNAL(loadFinished(bool)), this, SLOT(finishedLoad(bool)));
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
    qDebug() << "Tweakin application for windows";
    ui->menuView->addSeparator();
    ui->menuView->addAction(ui->actionSwitch_mini);

    // Translate
    ui->actionQuit_GDeskTunes->setText("Exit");
    ui->actionQuit_GDeskTunes->setShortcut(QKeySequence());
    ui->actionEnter_Full_Screen->setShortcut(QKeySequence("Ctrl+Shift+F"));

    ui->menuWindow->deleteLater();
#endif

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

#ifndef Q_OS_MAC
    addAction(ui->actionShow_menu);
#endif
#ifndef Q_OS_WIN
    addAction(ui->actionClose_Window);
#endif

    connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(play()));
    connect(ui->actionPrevious, SIGNAL(triggered()), this, SLOT(previous()));
    connect(ui->actionNext, SIGNAL(triggered()), this, SLOT(next()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionQuit_GDeskTunes, SIGNAL(triggered()), this, SLOT(quitGDeskTunes()));
    connect(ui->actionSwitch_mini, SIGNAL(triggered()), this, SLOT(switchMiniPlayer()));
    connect(ui->actionEnter_Full_Screen, SIGNAL(triggered()), this, SLOT(switchFullScreen()));
    connect(ui->actionMinimize, SIGNAL(triggered()), this, SLOT(showMinimized()));
    connect(ui->actionZoom, SIGNAL(triggered()), this, SLOT(showMaximized()));
    connect(ui->actionBring_All_To_Front, SIGNAL(triggered()), this, SLOT(activateWindow()));
#ifndef Q_OS_MAC
    connect(ui->actionShow_menu, SIGNAL(triggered()), this, SLOT(switchMenu()));
#endif
#ifndef Q_OS_WIN
    connect(ui->actionClose_Window, SIGNAL(triggered()), this, SLOT(closeWindow()));
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
            switchMiniPlayer();
            return (true);
         }
    }
    return false;
}
#endif

void MainWindow::switchMiniPlayer()
{
    qDebug() << "switchMiniPlayer()" << !isMini();
    setMini(!isMini());
}

void MainWindow::switchMenu()
{
    bool visible = ui->menuBar->isHidden();
    qDebug() << "switchMenu" << visible;
    setMenuVisible(visible);
    options.hide_menu = !visible;
}

void MainWindow::setMenuVisible(bool visible)
{
    ui->menuBar->setVisible(visible);
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
}

QWebView* MainWindow::webView()
{
    return ui->webView;
}


void MainWindow::play()
{
    qDebug() << "Play";
    QWebView *web_view = webView();
    QWebElement elt = web_view->page()->mainFrame()->findFirstElement("*[data-id='play-pause']");
    elt.evaluateJavaScript("this.click();");
}

void MainWindow::next()
{
    qDebug() << "Next";
    QWebView *web_view = webView();
    QWebElement elt = web_view->page()->mainFrame()->findFirstElement("*[data-id='forward']");
    elt.evaluateJavaScript("this.click();");
}

void MainWindow::previous()
{
    qDebug() << "Previous";
    QWebView *web_view = webView();
    QWebElement elt = web_view->page()->mainFrame()->findFirstElement("*[data-id='rewind']");
    elt.evaluateJavaScript("this.click();");
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_MediaPlay:
    {
        play();
    }
        break;
    case Qt::Key_MediaNext:
    {
        next();
    }
        break;
    case Qt::Key_MediaPrevious:
    {
        previous();
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

void MainWindow::checkAction(QKeyEvent *event, QAction *action)
{
    QKeySequence shortcut = action->shortcut();

    int keyInt = event->key();
    Qt::Key key = static_cast<Qt::Key>(keyInt);
    if(key == Qt::Key_unknown){
        qDebug() << "Unknown key from a macro probably";
        return;
    }
    // the user have clicked just and only the special keys Ctrl, Shift, Alt, Meta.
    if(key == Qt::Key_Control ||
            key == Qt::Key_Shift ||
            key == Qt::Key_Alt ||
            key == Qt::Key_Meta)
    {
        qDebug() << "Single click of special key: Ctrl, Shift, Alt or Meta";
        qDebug() << "New KeySequence:" << QKeySequence(keyInt).toString(QKeySequence::NativeText);
        return;
    }

    // check for a combination of user clicks
    Qt::KeyboardModifiers modifiers = event->modifiers();
    QString keyText = event->text();
    // if the keyText is empty than it's a special key like F1, F5, ...
    qDebug() << "Pressed Key:" << keyText;

    if(modifiers & Qt::ShiftModifier)
        keyInt += Qt::SHIFT;
    if(modifiers & Qt::ControlModifier)
        keyInt += Qt::CTRL;
    if(modifiers & Qt::AltModifier)
        keyInt += Qt::ALT;
    if(modifiers & Qt::MetaModifier)
        keyInt += Qt::META;

    qDebug() << "New KeySequence:" << QKeySequence(keyInt).toString(QKeySequence::NativeText);
    if (shortcut.matches(QKeySequence(keyInt)))
    {
        action->trigger();
        qDebug() << action->text();
    }
}

void MainWindow::receiveMessage(const QString &msg)
{
    QStringList commands = msg.split(" ");
    QString cmd = commands.at(0);

    qDebug() << "Received message" << msg;
    if (commands.contains("--change_style"))
    {
        QString application_dir = QCoreApplication::applicationDirPath();
        QString css_string = application_dir + QDir::separator() + "userstyles" + QDir::separator() + "dark.css";
        qDebug() << "Changing style " << css_string;

    }
    if (commands.contains("--mini"))
    {
        setMini(!mini);
    }
    if (commands.contains("--settings"))
    {
        emit ui->actionPreferences->triggered();
    }
}

void MainWindow::createJumpList()
{
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    QWinJumpList jumplist;

    QWinJumpListItem *change_style = new QWinJumpListItem(QWinJumpListItem::Link);
    change_style->setTitle(tr("Change style"));
    change_style->setFilePath(QDir::toNativeSeparators(QCoreApplication::applicationFilePath()));
    change_style->setArguments(QStringList("--change_style"));
    jumplist.tasks()->addItem(change_style);

    jumplist.tasks()->setVisible(true);

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
            play();
            break;
        case 19:
            next();
            break;
        case 20:
            previous();
            break;
        }
    }
}

void MainWindow::setMini(bool toMini)
{
    if (toMini)
    {
        // Save the current geometry and state if not mini
        if (!this->mini)
        {
            save();
            normal_flags = windowFlags();
        }
        hide();

        ui->actionSwitch_mini->setText("Switch from Miniplayer");

        // Apply the mini style
        QString mini_style = options.getMiniCSS();
        applyStyle(mini_style, "mini");

        // Find the body element which must contain the width and height of the mini player
        QWebElement elt = ui->webView->page()->mainFrame()->documentElement().findFirst("body");
        QString width = elt.styleProperty("width", QWebElement::ComputedStyle);
        QString height = elt.styleProperty("height", QWebElement::ComputedStyle);
        int w = width.replace("px", "").toInt(0);
        int h = height.replace("px", "").toInt(0);
        qDebug() << "Size of mini player: " << w << " " << h;

        Qt::WindowFlags flags = options.miniPlayerFlags();
        setWindowFlags(flags);
        // Apply window flags
        show();
        hide();

        // Changing flags loses geometry, save position into pos variable
        restoreMini();
        QPoint pos = this->pos();
        qDebug() << pos;
        this->mini = toMini;


        // Apply new geometry
        move(pos - windows_offset);
        resize(w, h);
        qDebug() << this->pos();

        show();
        qDebug() << this->pos();
    }
    else
    {
        if (this->mini)
            saveMini();
        hide();
        this->mini = toMini;

        ui->actionSwitch_mini->setText("Switch to Miniplayer");

        setWindowFlags(normal_flags | Qt::WindowStaysOnBottomHint);
        // Show hide combination is only to apply the window flags
        // This will lose the geometry which will be set after hide()
        show();
        hide();

        restore();
        // Changing window flags loses geometry, save the geometry temporarily
        QPoint pos = this->pos();
        QSize size = this->size();
        if (options.isCustomized())
        {
            disableStyle(options.getMiniCSS(), "mini");
            applyStyle(options.getCSS());
        }

        //move(pos);
        //resize(size);
        show();
    }

}

bool MainWindow::isMini()
{
    return mini;
}

void MainWindow::restoreOptions()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    qDebug() << "Preferences: " << settings.fileName();

    options.restore(settings);
}

void MainWindow::show()
{
#ifndef Q_OS_MAC
    if (isMini())
    {
       setMenuVisible(false);
    }
    else
    {
        setMenuVisible(!options.isHideMenu());

        // Compute the windows_offset if necessary
        if (windows_offset.isNull())
        {
            windows_offset = geometry().topLeft() - frameGeometry().topLeft();
            qDebug() << "windows offset" << windows_offset;
        }
    }
#endif
    ui->toolBar->setVisible(isMini());

    QMainWindow::show();
}

void MainWindow::save()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("rectangle", geometry());
    options.save(settings);
}

void MainWindow::saveMini()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    qDebug() << "Saving" << pos();
    settings.setValue("miniPosition", pos());
    settings.setValue("miniGeometry", saveGeometry());
    qDebug() << geometry() << frameGeometry();
}

void MainWindow::restore()
{
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

void MainWindow::restoreMini()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    if (settings.contains("miniPosition"))
    {
        QPoint pos = settings.value("miniPosition", QPoint(0, 0)).toPoint();
        //qDebug() << "Restore Mini Position" << pos;
        //move(pos);
    }
    if (settings.contains("miniGeometry"))
    {
        qDebug() << "Restore Mini Position";
        restoreGeometry(settings.value("miniGeometry").toByteArray());
    }
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
        QMenu * menu = new Menu(this);
        menu->setHidden(true);
        QtWin::enableBlurBehindWindow(menu);

        QWinThumbnailToolBar* thumbnailToolBar = new QWinThumbnailToolBar(this);
        thumbnailToolBar->setWindow(windowHandle());

        QWinThumbnailToolButton* playToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
        playToolButton->setToolTip(tr("Play"));
        playToolButton->setIcon(QIcon(":/icons/32x32/media-playback-start.png"));
        connect(playToolButton, SIGNAL(clicked()), this, SLOT(play()));

        QWinThumbnailToolButton* forwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
        forwardToolButton->setToolTip(tr("Fast forward"));
        forwardToolButton->setIcon(QIcon(":/icons/32x32/media-skip-forward.png"));
        connect(forwardToolButton, SIGNAL(clicked()), this, SLOT(previous()));

        QWinThumbnailToolButton* backwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
        backwardToolButton->setToolTip(tr("Rewind"));
        backwardToolButton->setIcon(QIcon(":/icons/32x32/media-skip-backward.png"));
        connect(backwardToolButton, SIGNAL(clicked()), this, SLOT(next()));

        QWinThumbnailToolButton* separatorButton = new QWinThumbnailToolButton(thumbnailToolBar);
        separatorButton->setEnabled(false);

        QIcon icon(":/icons/32x32/preferences-other.png");
        QWinThumbnailToolButton* settingsButton = new QWinThumbnailToolButton(thumbnailToolBar);
        settingsButton->setToolTip(tr("Preferences"));
        settingsButton->setIcon(icon);
        connect(settingsButton, SIGNAL(clicked()), this, SLOT(changeSettings()));

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
    if (isMini())
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
    if (isMini() && do_move)
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
    quitting = true;
    close();
}

void MainWindow::closeWindow()
{
#ifdef Q_OS_MAC
    hideMac();
#endif
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (quitting)
    {
        event->accept();
    }
    else
    {
#ifdef Q_OS_MAC
        if (event->spontaneous())
        {
            hideMac();
            event->ignore();
        }
        else
            event->accept();
#else
        event->accept();
#endif
    }
}

void MainWindow::finishedLoad(bool ok)
{
    if (!ok)
        return;

#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../../../js");
#else
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "js");
#endif
    qDebug() << "Directory" << dir;

    QDirIterator it(dir);
    while(it.hasNext())
    {
        QFileInfo nextFile(it.next());
        if (nextFile.isDir())
            continue;
        evaluateJavaScriptFile(nextFile.absoluteFilePath());
    }

    applyStyle(options.getCSS());
}

void MainWindow::evaluateJavaScriptFile(QString name)
{
    qDebug() << "Js: " << name;
    QFile jsFile(name);
    jsFile.open(QFile::ReadOnly);
    QTextStream stream(&jsFile);
    QString script = stream.readAll();
    webView()->page()->mainFrame()->evaluateJavaScript(script);
}

void MainWindow::switchFullScreen()
{
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
}

void MainWindow::applyStyle(QString css, QString subdir)
{
    QString full_css = options.getStyle(css, subdir);
    qDebug() << "Apply style " << css << " " << full_css;

    QFile cssFile(full_css);
    cssFile.open(QFile::ReadOnly);
    QTextStream stream(&cssFile);
    QString css_content = stream.readAll();
    css_content.remove(QRegExp("[\\n\\t\\r]"));
    css_content.replace("\"", "\\\"");

    QString script = QString("Styles.applyStyle(\"%3%2\",\"%1\");").arg(css_content, css, subdir);
    webView()->page()->mainFrame()->evaluateJavaScript(script);
}

void MainWindow::disableStyle(QString css, QString subdir)
{
    webView()->page()->mainFrame()->evaluateJavaScript(QString("Styles.disableStyle(\"%2%1\")").arg(css, subdir));

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
    show();
    raise();
    QMainWindow::activateWindow();
}
