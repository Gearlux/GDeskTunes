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

public slots:
    void onTriggerMiniPlayer(QPoint& pt);
    void nowPlaying(QString title, QString artist, QString album, int duration);
    void playbackTime(int current, int total);

public:
    Ui::MiniPlayer *ui;
};

#endif // MINIPLAYER_H
