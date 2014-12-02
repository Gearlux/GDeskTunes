#ifndef GOOGLEMUSICAPP_H
#define GOOGLEMUSICAPP_H

#include <QWebPage>

class GoogleMusicApp : public QWebPage
{
    Q_OBJECT
public:
    explicit GoogleMusicApp(QObject *parent);

signals:
    void isPlaying(bool playing);
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
    void addWindowObjects();

    QString getShuffle();
    void shuffleOff();
    void shuffleOn();

    QString getRepeat();
    void repeatOff();
    void repeatAll();
    void repeatOne();

    void play();
    void next();
    void previous();

private:
    QString current_title;
    QString current_artist;
    QString current_album;
    int current_rating;

    QString current_repeat;
    QString current_shuffle;

};

#endif // GOOGLEMUSICAPP_H
