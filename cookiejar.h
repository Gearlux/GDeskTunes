#ifndef COOKIEJAR_H
#define COOKIEJAR_H

#include <QtNetwork/QNetworkCookieJar>

class CookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    explicit CookieJar(QObject *parent = 0);

signals:
    void saveCookies(bool save);
    void dontSaveCookies(bool dont_save);

public slots:
    void setSaveCookies(bool saveCookie) { this->save_cookies = saveCookie; emit saveCookies(saveCookie); emit dontSaveCookies(!saveCookie); }
    void setDontSaveCookies(bool dontSaveCookie) { setSaveCookies(!dontSaveCookie); }
    void removeCookieFile();
    void deleteAllCookies();

    void save();

public:
    void load();

    bool isSaveCookies() { return this->save_cookies; }

private:
    void purgeOldCookies();

private:
    bool save_cookies;

};

#endif // COOKIEJAR_H
