#include "remoteclient.h"
#include "engine.h"
#include "imageprovider.h"
#include <qqml.h>

#include <QUrl>

RemoteClient::RemoteClient() :
    Client(0),
    mode(-1),
    title("Title")
{
    setObjectName("RemoteClient");
    connect(this, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)),
            this, SLOT(bonjourConnect(const QList<BonjourRecord> &)));
}

RemoteClient::~RemoteClient()
{

}

void RemoteClient::bonjourConnect(const QList<BonjourRecord> &records)
{
    if (records.size() == 0)
        return;

    qDebug() << "bonjourConnect";
    BonjourRecord first = records.at(0);
    connectToServer(first);
}

void RemoteClient::onPlay()
{
    invokeMethod("onClientPlay");
}

void RemoteClient::onPrevious()
{
    invokeMethod("onClientPrevious");
}

void RemoteClient::onNext()
{
    invokeMethod("onClientNext");
}

void RemoteClient::onServerPlaying(int mode)
{
    this->mode = mode;
    emit modeChanged(mode);
}

void RemoteClient::onServerNowPlaying(QString title, QString artist, QString album, QString, int)
{
    this->title = title;
    emit titleChanged(title);

    this->artist = artist;
    emit artistChanged(artist);

    this->album = album;
    emit albumChanged(album);

    // this->albumArt = art;
    // emit albumArtChanged(art);
}

void RemoteClient::onServerAlbumArt(QString url, QPixmap pixmap)
{
    // Transform the url
    QString transformed = url.replace("https", "image");
    QUrl real_url(url);
    transformed = transformed.replace(real_url.host(), "gdeskremote");
    qDebug() << transformed;

    Engine *engine = dynamic_cast<Engine*>(qmlEngine(this));
    qDebug() << "engine: " << engine;
    if (engine != 0)
    {
        ImageProvider *provider = dynamic_cast<ImageProvider*>(engine->getImageProvider("gdeskremote"));
        qDebug() << "provider:" << provider;
        if (provider == 0)
        {
            provider->addPixmap(transformed, pixmap);

            this->albumArt = transformed;
            emit albumArtChanged(transformed);
        }
    }

}

int RemoteClient::getMode()
{
    return this->mode;
}

QString RemoteClient::getTitle()
{
    return this->title;
}

QString RemoteClient::getArtist()
{
    return this->artist;
}

QString RemoteClient::getAlbum()
{
    return this->album;
}

QString RemoteClient::getAlbumArt()
{
    return this->albumArt;
}
