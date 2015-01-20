#ifndef REMOTECLIENT_H
#define REMOTECLIENT_H

#include "client.h"

class RemoteClient : public Client
{
    Q_OBJECT

public:
    RemoteClient();
    ~RemoteClient();

    Q_INVOKABLE void onPlay();
    Q_INVOKABLE void onPrevious();
    Q_INVOKABLE void onNext();

    Q_INVOKABLE void onServerPlaying(int playing);
    Q_INVOKABLE void onServerNowPlaying(QString title, QString artist, QString album, QString art, int duration);

    Q_PROPERTY(int mode READ getMode NOTIFY modeChanged)
    Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)
    Q_PROPERTY(QString artist READ getArtist NOTIFY artistChanged)
    Q_PROPERTY(QString album READ getAlbum NOTIFY albumChanged)
    Q_PROPERTY(QString albumArt READ getAlbumArt NOTIFY albumArtChanged)

signals:
    // STARTING:  -1,
    // STOPPED:    0,
    // PAUSED:     1,
    // PLAYING:    2,
    void modeChanged(int mode);
    void titleChanged(QString title);
    void artistChanged(QString artist);
    void albumChanged(QString album);
    void albumArtChanged(QString albumArt);

public slots:
    void bonjourConnect(const QList<BonjourRecord> &records);

public:
    int getMode();
    QString getTitle();
    QString getArtist();
    QString getAlbum();
    QString getAlbumArt();

private:
    int mode;
    QString title;
    QString artist;
    QString album;
    QString albumArt;
};

#endif // REMOTECLIENT_H
