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

void RemoteClient::onVolume(float volume)
{
    int server_volume = qMax(0, qMin(100, (int)(volume * 100)));
    invokeMethod("onClientVolume", server_volume);
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

void RemoteClient::onServerShuffle(QString mode)
{
    if (mode == "NO_SHUFFLE")
    {
        shuffle = 0;
    }
    else
        shuffle = 1;
    emit shuffleChanged(shuffle);
}

void RemoteClient::onServerRepeat(QString mode)
{
    if (mode == "NO_REPEAT")
    {
        repeat = 0;
    }
    else if (mode == "SINGLE_REPEAT")
    {
        repeat = 1;
    }
    else
        repeat = 2;
    qDebug() << "onServerRepeat(" << mode << ")" << repeat;
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

int RemoteClient::getRepeat()
{
    return this->repeat;
}
