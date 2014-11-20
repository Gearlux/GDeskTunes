#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QtNetwork/QNetworkCookieJar>

class CookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    explicit CookieJar(QObject *parent = 0);

public:
    void load();
    void save();

    void deleteAllCookies();

    void removeCookieFile();
private:
    void purgeOldCookies();

signals:

public slots:

};

#endif // COOKIEJAR_H
