#ifndef GOOGLEMUSICAPP_H
#define GOOGLEMUSICAPP_H

#define USE_WEBKIT

#ifdef USE_WEBKIT
#include <QWebPage>
#else
#include <QWebEnginePage>
#endif

class GoogleMusicApp :
#ifdef USE_WEBKIT
        public QWebPage
#else
        public QWebEnginePage
#endif
{
    Q_OBJECT
public:
    explicit GoogleMusicApp(QObject *parent);

signals:
    void isPlaying(int playing);
    void nowPlaying(QString title, QString artist, QString album, QString art, int duration);
    void albumArt(QString url, QPixmap pixmap);
    void rating(int rating);
    void playbackTime(int current, int total);
    void love(QString title, QString artist, QString album);
    void unlove(QString title, QString artist, QString album);
    void volume(int vol);

    void forwardEnabled(int mode);
    void rewindEnabled(int mode);

    void repeat(QString mode);
    void shuffle(QString mode);

    void switchToMiniPlayer();
    void switchToCompactPlayer();

public slots:
    void volChanged(int vol);
    void volumeChanged(int vol);
    int getVolume();
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

    void changePlaybackTime(int ms);

    void play();
    void next();
    void previous();

    void on_miniButton_clicked();
    void on_compactButton_clicked();

    void info();

public:
    int getBodyWidth();
    int getBodyHeight();
    QVariant evaluateJavaScript(const QString &script);
    QColor getBackgroundColor();

private slots:
    void onDownloaded(QByteArray &data);

private:
    QString current_title;
    QString current_artist;
    QString current_album;
    QString current_art;
    int current_rating;

    QString current_repeat;
    QString current_shuffle;

};

#endif // GOOGLEMUSICAPP_H
