#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QNetworkReply>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QWidget *parent = 0);
    ~Downloader();

signals:

public slots:
    void onUnsupportedContent(QNetworkReply * reply);
};

#endif // DOWNLOADER_H
