#ifndef MINIPLAYER_H
#define MINIPLAYER_H

#include <QMainWindow>
#include <QAbstractButton>

namespace Ui {
class MiniPlayer;
}

class MiniPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MiniPlayer(QWidget *parent = 0);
    ~MiniPlayer();

    bool event(QEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void keyPressed(QKeyEvent *event);
    void moved();
    void changePlaybackTime(int ms);

public slots:
    void placeMiniPlayer(QPoint& pt);

    void nowPlaying(QString title, QString artist, QString album, int duration);
    void playbackTime(int current, int total);
    void rating(int rate);
    void albumArt(QPixmap pixmap);

    void repeat(QString mode);
    void shuffle(QString mode);

    void changeBackground();
    void enableBackground();
    void disableBackground();

    void activateWindow();
    void hide();
    void show();
    void raise();

    void bringToFront();

    void setBackgroundColor(QColor color);
    void setMiniPlayerOnTop(bool on_top);

    void isPlaying(int mode);
    void rewindEnabled(int mode);
    void forwardEnabled(int mode);

    void determineCorner(QScreen *screen);
    void resize(int w, int h);

    void on_slider_sliderPressed();
    void on_slider_sliderReleased();
    void on_slider_valueChanged(int);

private:
    void invert(bool inv);
    void showElements(bool visible);
    void setIcon(QAbstractButton* button, QString base);

public:
    Ui::MiniPlayer *ui;

private:
    // This is a real hack
    // The mini player has two positions
    // - Invoked from the trayIcon
    // - Invoked when calling from the menus or shortcuts
    // We assume that when we invoke the mini player from the trayIcon
    // The timing to show it is less then 100ms.
    // Otherwise the userPosition is used
    //
    // Both points are initialized to (
    qint64 trayIconTiming;
    QPoint trayIconPosition;
    qint64 userIconTiming;
    QPoint userPosition;

    QPixmap album_picture;
    bool large;

    bool on_top;

    // Variables to drag the mini player without title bar
    bool do_move;
    bool has_moved;
    int mouse_click_x_coordinate;
    int mouse_click_y_coordinate;

    // Set this when we inverted the colors
    bool inverted;
    int gray;
    QString _shuffle;
    QString _repeat;
    int _rating;
    QString style;

    // Corner (1 is topleft, 2 is topright, ..., 0: undefined)
    int corner;

    // Only the last update should be triggered
    bool slider_moving;

};

#endif // MINIPLAYER_H
