#ifndef REMOTECLIENT_H
#define REMOTECLIENT_H

#include "client.h"
#include <QPixmap>

class RemoteClient : public Client
{
    Q_OBJECT

public:
    RemoteClient();
    ~RemoteClient();

    Q_INVOKABLE void onPlay();
    Q_INVOKABLE void onPrevious();
    Q_INVOKABLE void onNext();
    Q_INVOKABLE void onVolume(float);

    Q_INVOKABLE void onServerPlaying(int playing);
    Q_INVOKABLE void onServerNowPlaying(QString title, QString artist, QString album);
    Q_INVOKABLE void onServerAlbumArt(QString url, QPixmap pixmap);
    Q_INVOKABLE void onServerShuffle(QString mode);
    Q_INVOKABLE void onServerRepeat(QString mode);

    Q_PROPERTY(int mode READ getMode NOTIFY modeChanged)
    Q_PROPERTY(QString title READ getTitle NOTIFY titleChanged)
    Q_PROPERTY(QString artist READ getArtist NOTIFY artistChanged)
    Q_PROPERTY(QString album READ getAlbum NOTIFY albumChanged)
    Q_PROPERTY(QString albumArt READ getAlbumArt NOTIFY albumArtChanged)
    Q_PROPERTY(int shuffle READ getShuffle NOTIFY shuffleChanged)
    Q_PROPERTY(int repeat READ getRepeat NOTIFY repeatChanged)


signals:
    // DISCONNECTED:  -1,
    // STOPPED:    0,
    // PAUSED:     1,
    // PLAYING:    2,
    void modeChanged(int mode);
    void titleChanged(QString title);
    void artistChanged(QString artist);
    void albumChanged(QString album);
    void albumArtChanged(QString albumArt);

    void shuffleChanged(int mode);
    void repeatChanged(int mode);

public slots:
    void bonjourConnect(const QList<BonjourRecord> &records);
    void onInfo();
    void onDisconnected();

public:
    int getMode();
    QString getTitle();
    QString getArtist();
    QString getAlbum();
    QString getAlbumArt();
    int getShuffle();
    int getRepeat();

private:
    int mode;
    QString title;
    QString artist;
    QString album;
    QString albumArt;
    int shuffle;
    int repeat;
};

#endif // REMOTECLIENT_H
