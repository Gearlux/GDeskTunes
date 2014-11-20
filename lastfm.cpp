#include "lastfm.h"

#include "thirdparty/liblastfm/src/ws.h"
#include "thirdparty/liblastfm/src/misc.h"
#include "thirdparty/liblastfm/src/XmlQuery.h"
#include "thirdparty/liblastfm/src/Track.h"

LastFM::LastFM(MainWindow *parent) :
    QObject(parent),
    main_window(parent),
    audioscrobbler("GDeskTunes")
{
    lastfm::ws::SessionKey = QString::null;
}

bool LastFM::isAuthorized()
{
    return lastfm::ws::SessionKey != QString::null;
}

void LastFM::login()
{
    qDebug() << "LastFM::login()";
    QString user = main_window->options.getLastFMUserName();
    QString password = main_window->options.getLastFMPassword();

    lastfm::ws::Username = user;
    lastfm::ws::ApiKey = "6d6878dfb071f682d6cbe4f76cd7955a";
    lastfm::ws::SharedSecret = "97e8de952255bab40ea4443e84eda3d6";

    QMap<QString, QString> params;
    params["method"] = "auth.getMobileSession";
    params["username"] = user;
    params["authToken"] = lastfm::md5( (lastfm::ws::Username + lastfm::md5( password.toUtf8() )).toUtf8() );

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
   if (lfm.attribute("status") == "ok")
   {
       lastfm::ws::Username = lfm["session"]["name"].text();
       lastfm::ws::SessionKey = lfm["session"]["key"].text();
   }

   emit changed();
}

void LastFM::logout()
{
    lastfm::ws::SessionKey = QString::null;
}

void LastFM::scrobble(QString title, QString artist, QString album, int duration)
{
    if (isAuthorized())
    {
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
