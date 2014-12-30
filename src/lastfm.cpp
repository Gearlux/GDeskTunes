#define QT_NO_DEBUG_OUTPUT

#include "lastfm.h"

#include <QSettings>
#include <QApplication>
#include <QDebug>

#include "thirdparty/liblastfm/src/ws.h"
#include "thirdparty/liblastfm/src/misc.h"
#include "thirdparty/liblastfm/src/XmlQuery.h"
#include "thirdparty/liblastfm/src/Track.h"

LastFM::LastFM(QObject *parent) :
    QObject(parent),
    audioscrobbler(0)
{
    lastfm::ws::SessionKey = QString::null;

    current_track.setDuration(-1);
}

QString LastFM::getLastFMUserName()
{
    return lastfm::ws::Username;
}

QString LastFM::getLastFMPassword()
{
    return this->password;
}

bool LastFM::isAuthorized()
{
    return lastfm::ws::SessionKey != QString::null;
}

void LastFM::login(QString username, QString password)
{
    qDebug() << "LastFM::login()";

    this->password = password;
    this->md5_password = lastfm::md5( password.toUtf8() );

    lastfm::ws::Username = username;
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

       audioscrobbler = new lastfm::Audioscrobbler("GDeskTunes");

       emit lastFMUserName(lastfm::ws::Username);
       emit lastFMPassword(this->password);
   }

   emit authorized(status);
}

void LastFM::logout()
{
    qDebug() << "LastFM::logout()";
    lastfm::ws::SessionKey = QString::null;
    emit authorized(false);
}

void LastFM::nowPlaying(QString title, QString artist, QString album, QString art, int duration)
{
    if (isAuthorized() && scrobble)
    {
        qDebug() << "LastFM::nowPlaying(" << title << "," << artist << "," << album << "," << art << "," << duration << ")";
        if (current_track.duration() != -1)
        {
            unsigned int playtime = current_track.timestamp().secsTo(QDateTime::currentDateTime());
            qDebug() << "Song " << current_track << " has played " << playtime << " seconds";
            if (playtime > current_track.duration() / 2)
            {
                if (audioscrobbler != 0)
                {
                    audioscrobbler->cache(current_track);
                    audioscrobbler->submit();
                }
            }
            current_track.setDuration(-1);
        }

        lastfm::MutableTrack t;

        t.setArtist(artist);
        t.setTitle(title);
        t.setAlbum(album);
        t.setDuration(duration);
        t.stamp();

        if (audioscrobbler != 0)
            audioscrobbler->nowPlaying(t);
        current_track = t;
    }
}

void LastFM::love(QString title, QString artist, QString album)
{
    if (isAuthorized() && auto_like)
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
    if (isAuthorized() && auto_like)
    {
        qDebug() << "LastFM::unlove(" << title << "," << artist << "," << album <<")";

        lastfm::MutableTrack t;

        t.setArtist(artist);
        t.setTitle(title);
        t.setAlbum(album);

        t.unlove();
    }
}

void LastFM::save()
{
    qDebug() << "LastFM::save()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    settings.setValue("last.fm.username", this->getLastFMUserName());
    settings.setValue("last.fm.password", this->getLastFMPassword());
    settings.setValue("last.fm.authorized", this->isAuthorized());
    settings.setValue("last.fm.scrobble", this->isScrobbled());
    settings.setValue("last.fm.autolike", this->isAutoLiked());
}

void LastFM::load()
{
    qDebug() << "LastFM::load()";
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    this->setScrobbled(settings.value("last.fm.scrobble", false).toBool());
    this->setAutoLiked(settings.value("last.fm.autolike", false).toBool());

    if (settings.value("last.fm.authorized", false).toBool())
    {
        this->login(settings.value("last.fm.username", "").toString(), settings.value("last.fm.password", "").toString());
    }
}
