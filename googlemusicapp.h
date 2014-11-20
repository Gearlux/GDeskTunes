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

public slots:
    void increaseVolume();
    void decreaseVolume();

    void playbackTimeChanged(int current, int total);
    void repeatChanged(QString mode);
    void shuffleChanged(QString mode);
    void playbackChanged(int mode);
    void ratingChanged(int rating);

    void notifySong(QString title, QString artist, QString album, QString art, int duration);

    QString getShuffle();
    void shuffleOff();
    void shuffleOn();

    QString getRepeat();
    void repeatOff();
    void repeatAll();
    void repeatOne();

private:
    MainWindow* main_window;

};

#endif // GOOGLEMUSICAPP_H
