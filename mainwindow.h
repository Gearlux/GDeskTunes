#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebView>

#include "options.h"

// Forward declarations
namespace Ui {
class GDeskTunes;
}
class Settings;
class Options;

// The menu is declared within the MainWindow class and forwards
// key events to the mainwindow to get the media keys working
class Menu;
class GoogleMusicApp;
class LastFM;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend class Menu;
    friend class Options;
    friend class Settings;
    friend class GoogleMusicApp;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QWebView* webView();

public:
    void restore();
    void save();
    void restoreMini();
    void saveMini();

    bool isMini();
    void setMini(bool toMini);

    void restoreOptions();
    void applyStyle(QString css, QString subdir = QString::null);
    void disableStyle(QString css, QString subdir = QString::null);

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);

#if defined Q_OS_WIN && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif

public slots:
    void receiveMessage(const QString& msg);
    bool eventFilter(QObject *sender, QEvent *event);

    void play();
    void next();
    void previous();

    void changeSettings();
    void settingsDestroyed();
    void switchMiniPlayer();
    void switchMenu();
    void switchFullScreen();
    void setMenuVisible(bool visible);

    void about();
    void show();

    void receiveMacMediaKey(int key, bool repeat, bool pressed);

    void quitGDeskTunes();
    void closeWindow();

    void addWindowObjects();
    void finishedLoad(bool);
    void updateUI();

    void evaluateJavaScriptFile(QString filename);

private:
    void createThumbnailToolBar();
    void createJumpList();

    void setupActions();
    void checkAction(QKeyEvent *event, QAction *action);

public:
    Options options;

    Ui::GDeskTunes *ui;
    GoogleMusicApp *app;
    LastFM *last_fm;
    Settings *settings;

private:
    QMenu *shuffle_menu;
    QAction *shuffle_on;
    QAction *shuffle_off;

    QMenu *repeat_menu;
    QAction *repeat_off;
    QAction *repeat_all;
    QAction *repeat_one;

    // Variables related to mini player switching
    QPoint windows_offset;
    bool mini;
    Qt::WindowFlags normal_flags;

    bool quitting;

    bool do_move;
    int mouse_click_x_coordinate;
    int mouse_click_y_coordinate;
};

#endif // MAINWINDOW_H
