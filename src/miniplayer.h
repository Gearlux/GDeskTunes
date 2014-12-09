#ifndef MINIPLAYER_H
#define MINIPLAYER_H

#include <QDialog>

namespace Ui {
class MiniPlayer;
}

class MiniPlayer : public QDialog
{
    Q_OBJECT

public:
    explicit MiniPlayer(QWidget *parent = 0);
    ~MiniPlayer();

    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void onTriggerMiniPlayer(QPoint& pt);
    void nowPlaying(QString title, QString artist, QString album, int duration);
    void playbackTime(int current, int total);
    void rating(int rate);
    void albumArt(QPixmap pixmap);

    void setRepeat(QString mode);
    void setShuffle(QString mode);

    void changeBackground();
    void enableBackground();
    void disableBackground();

public:
    Ui::MiniPlayer *ui;

private:
    QPixmap album_picture;
    bool large;

    // Variables to drag the mini player without title bar
    bool do_move;
    int mouse_click_x_coordinate;
    int mouse_click_y_coordinate;
};

#endif // MINIPLAYER_H