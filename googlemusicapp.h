#ifndef GOOGLEMUSICAPP_H
#define GOOGLEMUSICAPP_H

#include <QObject>

class MainWindow;

class GoogleMusicApp : public QObject
{
    Q_OBJECT
public:
    explicit GoogleMusicApp(MainWindow *parent);

signals:
    void nowPlaying(QString title, QString artist, QString album, int duration);
    void love(QString title, QString artist, QString album);
    void unlove(QString title, QString artist, QString album);

    void repeat(QString mode);
    void shuffle(QString mode);

public slots:
    void increaseVolume();
    void decreaseVolume();

    void playbackTimeChanged(int current, int total);
    void repeatChanged(QString mode);
    void shuffleChanged(QString mode);
    void playbackChanged(int mode);
    void ratingChanged(int rating);

    void notifySong(QString title, QString artist, QString album, QString art, int duration);

    void loadFinished(bool status);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    void addWindowObjects();
#endif

    QString getShuffle();
    void on_shuffle_off_triggered();
    void on_shuffle_on_triggered();

    QString getRepeat();
    void on_repeat_off_triggered();
    void on_repeat_all_triggered();
    void on_repeat_one_triggered();

private:
    MainWindow* main_window;

    QString current_title;
    QString current_artist;
    QString current_album;
    int current_rating;

    QString current_repeat;
    QString current_shuffle;

};

#endif // GOOGLEMUSICAPP_H
