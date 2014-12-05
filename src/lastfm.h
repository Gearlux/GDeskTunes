#ifndef LASTFM_H
#define LASTFM_H

#include <QObject>
#include "thirdparty/liblastfm/src/Audioscrobbler.h"
#include "thirdparty/liblastfm/src/Track.h"

class LastFM : public QObject
{
    Q_OBJECT
public:
    explicit LastFM(QObject *parent = 0);

    bool isAuthorized();

    QString getLastFMUserName();
    QString getLastFMPassword();
    bool isScrobbled() { return this->scrobble; }
    bool isAutoLiked() { return this->auto_like; }

signals:
    void authorized(bool status);
    void scrobbled(bool scrobble);
    void autoLiked(bool auto_like);
    void lastFMUserName(QString name);
    void lastFMPassword(QString passwd);

public slots:
    void setScrobbled(bool scrobble)  { this->scrobble = scrobble; emit scrobbled(scrobble); }
    void setAutoLiked(bool like) { this->auto_like = like; emit autoLiked(like); }

    void login(QString username, QString password);
    void logout();

    void nowPlaying(QString title, QString artist, QString album, int duration);
    void love(QString title, QString artist, QString album);
    void unlove(QString title, QString artist, QString album);

    void save();
    void load();

private slots:
    void loggedIn();

private:
    lastfm::Audioscrobbler *audioscrobbler;
    lastfm::MutableTrack current_track;
    QString password;
    QString md5_password;
    bool scrobble;
    bool auto_like;
};

#endif // LASTFM_H
