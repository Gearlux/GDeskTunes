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

public slots:
    void onClientPlay();
    void onClientNext();
    void onClientPrevious();

    void isPlaying(int playing);
    void nowPlaying(QString title, QString artist, QString album, QString art, int duration);
    void albumArt(QString url, QPixmap pixmap);

    void repeat(QString mode);
    void shuffle(QString mode);

private:
    // State (to limit network communication)
    int state_playing;
    QString state_repeat;
    QString state_shuffle;
};

#endif // REMOTESERVER_H
