#ifndef ENGINE_H
#define ENGINE_H

#include <QQmlApplicationEngine>
#include <QMap>

class Engine : public QQmlApplicationEngine
{
public:
    Engine(const QUrl &url, QObject *parent=0);
    ~Engine();

public:
    void addImageProvider(const QString &id, QQmlImageProviderBase *);
    QQmlImageProviderBase* getImageProvider(const QString &id);

private:
    QMap<QString, QQmlImageProviderBase*> image_providers;
};

#endif // ENGINE_H
