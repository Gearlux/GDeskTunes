#ifndef NOTIFICATIONCENTER_H
#define NOTIFICATIONCENTER_H

#include <QObject>
#include <QString>

class MacNotificationCenter: public QObject {
    Q_OBJECT
public:
    explicit MacNotificationCenter();

signals:
    void next();

public slots:

public:
    void showMessage(QString title, QString message, QString art, bool itunes, bool show_art);

private:
    class Private;
    Private* d;
};

#endif // NOTIFICATIONCENTER_H

