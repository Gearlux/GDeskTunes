#ifndef REMOTESERVER_H
#define REMOTESERVER_H

#include "server.h"
#include <QPixmap>

class RemoteServer : public Server
{
    Q_OBJECT

public:
    RemoteServer();
    ~RemoteServer();

    int convertRepeat(QString mode);
    int convertShuffle(QString mode);
signals:
    void play();
    void next();
    void previous();
    void volumeChanged(int volume);
    void shuffleOff();
    void shuffleOn();
    void repeatOne();
    void repeatOff();
    void repeatAll();
    void info();

public slots:
    void onClientPlay();
    void onClientNext();
    void onClientPrevious();
    void onClientInfo();
    void onClientVolume(int volume);
    void onClientShuffle(int shuffle);
    void onClientRepeat(int repeat);

    void isPlaying(int playing);
    void nowPlaying(QString title, QString artist, QString album, QString art, int duration);
    void albumArt(QString url, QPixmap pixmap);
    void volume(int vol);

    void repeat(QString mode);
    void shuffle(QString mode);

private:
    // State (to limit network communication)
    int state_playing;
    QString state_title;
    QString state_artist;
    QString state_album;
    QString state_url;
    QPixmap state_pixmap;
    int state_repeat;
    int state_shuffle;
    int state_volume;
};

#endif // REMOTESERVER_H
