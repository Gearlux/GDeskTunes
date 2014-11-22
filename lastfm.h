#ifndef LASTFM_H
#define LASTFM_H

#include <QObject>
#include "thirdparty/liblastfm/src/Audioscrobbler.h"

#include "mainwindow.h"

class LastFM : public QObject
{
    Q_OBJECT
public:
    explicit LastFM(QObject *parent = 0);

    bool isAuthorized();

signals:
    void authorized(bool status);

public slots:
    void setLastFMUserName(QString username);
    void setLastFMPassword(QString password);
    void setScrobbled(bool);
    void setAutoLiked(bool);

    void login();
    void logout();

    void nowPlaying(QString title, QString artist, QString album, int duration);
    void love(QString title, QString artist, QString album);
    void unlove(QString title, QString artist, QString album);

private slots:
    void loggedIn();

private:
    lastfm::Audioscrobbler audioscrobbler;
    QString md5_password;
    bool scrobbled;
    bool auto_liked;
};

#endif // LASTFM_H
