#ifndef GOOGLEMUSICAPP_H
#define GOOGLEMUSICAPP_H

#include <QWebPage>

class GoogleMusicApp : public QWebPage
{
    Q_OBJECT
public:
    explicit GoogleMusicApp(QObject *parent);

signals:
    void isPlaying(int playing);
    void nowPlaying(QString title, QString artist, QString album, int duration);
    void albumArt(QPixmap array);
    void rating(int rating);
    void playbackTime(int current, int total);
    void love(QString title, QString artist, QString album);
    void unlove(QString title, QString artist, QString album);

    void forwardEnabled(int mode);
    void rewindEnabled(int mode);

    void repeat(QString mode);
    void shuffle(QString mode);

public slots:
    void increaseVolume();
    void decreaseVolume();

    // FIXME: Are these real slots ?
    void playbackTimeChanged(int current, int total);
    void repeatChanged(QString mode);
    void shuffleChanged(QString mode);
    void playbackChanged(int mode);
    void ratingChanged(int rating);
    void rewindChanged(int mode);
    void forwardChanged(int mode);

    void notifySong(QString title, QString artist, QString album, QString art, int duration);
    int getRating();

    void loadFinished(bool status);
    void addWindowObjects();

    QString getShuffle();
    void toggleShuffle();
    void shuffleOff();
    void shuffleOn();

    QString getRepeat();
    void toggleRepeat();
    void repeatOff();
    void repeatAll();
    void repeatOne();

    void toggleThumbsUp();
    void toggleThumbsDown();

    void play();
    void next();
    void previous();

private slots:
    void onDownloaded(QByteArray &data);

private:
    QString current_title;
    QString current_artist;
    QString current_album;
    int current_rating;

    QString current_repeat;
    QString current_shuffle;

};

#endif // GOOGLEMUSICAPP_H
