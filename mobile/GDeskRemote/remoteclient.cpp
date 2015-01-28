#include "remoteclient.h"
#include "engine.h"
#include "imageprovider.h"
#include <qqml.h>

#include <QUrl>

RemoteClient::RemoteClient() :
    Client(QLatin1String("_gdeskremote._tcp"), 0),
    mode(-1),
    title("Title")
{
    setObjectName("RemoteClient");
    connect(this, SIGNAL(currentBonjourRecordsChanged(const QList<BonjourRecord> &)),
            this, SLOT(bonjourConnect(const QList<BonjourRecord> &)));
    connect(this, SIGNAL(connected()), this, SLOT(onInfo()));
    connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
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

void RemoteClient::setVolume(int volume)
{
    qDebug() << "RemoteClient::onVolume(" << volume << ")";
    if (vol == volume) return;
    vol = volume;
    invokeMethod("onClientVolume", vol);
    emit volumeChanged(vol);
}

int RemoteClient::getVolume()
{
    return vol;
}

void RemoteClient::onInfo()
{
    invokeMethod("onClientInfo");
}

void RemoteClient::onDisconnected()
{
    this->mode = -1;
    emit modeChanged(mode);
}

void RemoteClient::onServerPlaying(int mode)
{
    this->mode = mode;
    emit modeChanged(mode);
}

void RemoteClient::onServerNowPlaying(QString title, QString artist, QString album)
{
    qDebug() << "onServerNowPlaying(" << title << "," << artist << "," << album << ")";
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
    qDebug() << "onServerAlbumArt(" << url << "," << pixmap << ")";
    // Transform the url
    QString transformed = url.replace("https", "image");
    QUrl real_url(url);
    transformed = transformed.replace(real_url.host(), "gdeskremote") + ".png";
    qDebug() << transformed;

    Engine *engine = dynamic_cast<Engine*>(qmlEngine(this));
    qDebug() << "engine: " << engine;
    if (engine != 0)
    {
        ImageProvider *provider = dynamic_cast<ImageProvider*>(engine->getImageProvider("gdeskremote"));
        qDebug() << "provider:" << provider;
        if (provider)
        {
            provider->addPixmap(transformed, pixmap);

            this->albumArt = transformed;
            emit albumArtChanged(transformed);
        }
    }
}

void RemoteClient::onServerVolume(int vol)
{
    this->vol = vol;
    emit volumeChanged(vol);
}

void RemoteClient::onServerShuffle(int mode)
{
    shuffle = mode;
    emit shuffleChanged(shuffle);
}

void RemoteClient::onServerRepeat(int mode)
{
    qDebug() << "onServerRepeat(" << mode << ")" << repeat;
    repeat = mode;
    emit repeatChanged(repeat);
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

int RemoteClient::getShuffle()
{
    return this->shuffle;
}

void RemoteClient::setShuffle(int shuffle)
{
    this->shuffle = shuffle;
    invokeMethod("onClientShuffle", shuffle);
}

int RemoteClient::getRepeat()
{
    return this->repeat;
}

void RemoteClient::setRepeat(int repeat)
{
    this->repeat = repeat;
    invokeMethod("onClientRepeat", repeat);
}
