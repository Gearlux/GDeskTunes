#include "lastfm.h"

#include "thirdparty/liblastfm/src/ws.h"
#include "thirdparty/liblastfm/src/misc.h"
#include "thirdparty/liblastfm/src/XmlQuery.h"
#include "thirdparty/liblastfm/src/Track.h"

LastFM::LastFM(QObject *parent) :
    QObject(parent),
    audioscrobbler("GDeskTunes")
{
    lastfm::ws::SessionKey = QString::null;
}

void LastFM::setLastFMUserName(QString username)
{
    qDebug() << "LastFM::setLastFMUserName(" << username << ")";
    lastfm::ws::Username = username;
}

void LastFM::setLastFMPassword(QString password)
{
    qDebug() << "LastFM::setLastFMPassword(" << password << ")";
    this->md5_password = lastfm::md5( password.toUtf8() );
}

void LastFM::setScrobbled(bool scrobbled)
{
    qDebug() << "LastFM::setScrobbled(" << scrobbled << ")";
    this->scrobbled = scrobbled;
}

void LastFM::setAutoLiked(bool auto_liked)
{
    qDebug() << "LastFM::setAutoLiked(" << auto_liked << ")";
    this->auto_liked = auto_liked;
}

bool LastFM::isAuthorized()
{
    return lastfm::ws::SessionKey != QString::null;
}

void LastFM::login()
{
    qDebug() << "LastFM::login()";

    lastfm::ws::ApiKey = "6d6878dfb071f682d6cbe4f76cd7955a";
    lastfm::ws::SharedSecret = "97e8de952255bab40ea4443e84eda3d6";

    QMap<QString, QString> params;
    params["method"] = "auth.getMobileSession";
    params["username"] = lastfm::ws::Username;
    params["authToken"] = lastfm::md5( (lastfm::ws::Username + md5_password).toUtf8() );

    QNetworkReply* reply = lastfm::ws::post( params );

    connect(reply, SIGNAL(finished()), this, SLOT(loggedIn()));
}

void LastFM::loggedIn()
{
    qDebug() << "LastFM::loggedIn()";
   QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
   lastfm::XmlQuery lfm;
   lfm.parse(reply->readAll());

   qDebug() << lfm;
   bool status = false;
   if (lfm.attribute("status") == "ok")
   {
       status = true;
       lastfm::ws::Username = lfm["session"]["name"].text();
       lastfm::ws::SessionKey = lfm["session"]["key"].text();
   }

   emit authorized(status);
}

void LastFM::logout()
{
    qDebug() << "LastFM::logout()";
    lastfm::ws::SessionKey = QString::null;
    emit authorized(false);
}

void LastFM::nowPlaying(QString title, QString artist, QString album, int duration)
{
    if (isAuthorized() && scrobbled)
    {
        qDebug() << "LastFM::nowPlaying(" << title << "," << artist << "," << album << "," << duration << ")";
        lastfm::MutableTrack t;

        t.setArtist(artist);
        t.setTitle(title);
        t.setAlbum(album);
        t.setDuration(duration);
        t.stamp();

        audioscrobbler.nowPlaying(t);
        audioscrobbler.cache(t);
        audioscrobbler.submit();
    }
}

void LastFM::love(QString title, QString artist, QString album)
{
    if (isAuthorized() && auto_liked)
    {
        qDebug() << "LastFM::love(" << title << "," << artist << "," << album <<")";
        lastfm::MutableTrack t;

        t.setArtist(artist);
        t.setTitle(title);
        t.setAlbum(album);

        t.love();
    }
}

void LastFM::unlove(QString title, QString artist, QString album)
{
    if (isAuthorized() && auto_liked)
    {
        qDebug() << "LastFM::unlove(" << title << "," << artist << "," << album <<")";

        lastfm::MutableTrack t;

        t.setArtist(artist);
        t.setTitle(title);
        t.setAlbum(album);

        t.unlove();
    }
}
