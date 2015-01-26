#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>

class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider();
    ~ImageProvider();

public:
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    void addPixmap(const QString &id, QPixmap pixmap);

private:
    QString latest_id;
    QPixmap latest_pixmap;
};

#endif // IMAGEPROVIDER_H
