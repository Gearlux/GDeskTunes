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

signals:
    void play();
    void next();
    void previous();
    void volume(int volume);

public slots:
    void onClientPlay();
    void onClientNext();
    void onClientPrevious();
    void onClientInfo();
    void onClientVolume(int volume);

    void isPlaying(int playing);
    void nowPlaying(QString title, QString artist, QString album, QString art, int duration);
    void albumArt(QString url, QPixmap pixmap);

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
    QString state_repeat;
    QString state_shuffle;
};

#endif // REMOTESERVER_H
