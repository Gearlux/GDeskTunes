#include "remoteserver.h"

RemoteServer::RemoteServer() :
    Server(QLatin1String("_gdeskremote._tcp")),
    state_playing(-1),
    state_repeat(-1),
    state_shuffle(-1),
    state_volume(-1)
{

}

RemoteServer::~RemoteServer()
{

}

void RemoteServer::onClientPlay()
{
    qDebug() << "RemoteServer::onClientPlay()";
    emit play();
}

void RemoteServer::onClientNext()
{
    qDebug() << "RemoteServer::onClientNext()";
    emit next();
}

void RemoteServer::onClientPrevious()
{
    qDebug() << "RemoteServer::onClientPrevious()";
    emit previous();
}

void RemoteServer::onClientInfo()
{
    emit info();

    qDebug() << "RemoteServer::onClientInfo()";
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerNowPlaying", state_title, state_artist, state_album);
        proto->invokeMethod("onServerPlaying", state_playing);
        proto->invokeMethod("onServerAlbumArt", state_url, state_pixmap);
        proto->invokeMethod("onServerRepeat", state_repeat);
        proto->invokeMethod("onServerShuffle", state_shuffle);
        proto->invokeMethod("onServerVolume", state_volume);
    }
    connections_mutex.unlock();
}

void RemoteServer::onClientVolume(int vol)
{
    qDebug() << "RemoteServer::onClientVolume(" << vol << ")";
    emit volumeChanged(vol);
}

void RemoteServer::volume(int vol)
{
    if (vol == state_volume) return;
    state_volume = vol;
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerVolume", vol);
    }
    connections_mutex.unlock();
}

void RemoteServer::isPlaying(int playing)
{
    if (playing == state_playing) return;
    state_playing = playing;
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerPlaying", playing);
    }
    connections_mutex.unlock();

}

void RemoteServer::nowPlaying(QString title, QString artist, QString album, QString art, int duration)
{
    qDebug() << "RemoteServer::nowPlaying(" << title << "," << artist << "," << album << "," << art << "," << duration << ")";
    state_title = title;
    state_artist = artist;
    state_album = album;
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerNowPlaying", title, artist, album);
    }
    connections_mutex.unlock();
}

void RemoteServer::albumArt(QString url, QPixmap pixmap)
{
    qDebug() << "RemoteServer::albumArt(" << url << "," << pixmap << ")";
    if (url == state_url) return;
    state_url = url;
    state_pixmap = pixmap;
    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerAlbumArt", url, pixmap);
    }
    connections_mutex.unlock();

}

int RemoteServer::convertRepeat(QString mode)
{
    int repeat = 0;
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

    return repeat;
}

void RemoteServer::repeat(QString mode)
{
    int repeat = convertRepeat(mode);

    if (state_repeat == repeat) return;
    state_repeat = repeat;

    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerRepeat", repeat);
    }
    connections_mutex.unlock();
}

int RemoteServer::convertShuffle(QString mode)
{
    int shuffle = 0;
    if (mode == "NO_SHUFFLE")
    {
        shuffle = 0;
    }
    else
        shuffle = 1;

    return shuffle;
}

void RemoteServer::shuffle(QString mode)
{
    // Convert to integer
    int shuffle = convertShuffle(mode);

    if (state_shuffle == shuffle) return;
    state_shuffle = shuffle;

    connections_mutex.lock();
    for(int i=0; i<connections.count();++i)
    {
        Protocol* proto = connections.at(i);
        proto->invokeMethod("onServerShuffle", shuffle);
    }
    connections_mutex.unlock();
}

void RemoteServer::onClientShuffle(int shuffle)
{
    if (shuffle == 0)
        emit shuffleOff();
    if (shuffle == 1)
        emit shuffleOn();
}

void RemoteServer::onClientRepeat(int repeat)
{
    if (repeat == 0)
        emit repeatOff();
    if (repeat == 1)
        emit repeatOne();
    if (repeat == 2)
        emit repeatAll();
}
