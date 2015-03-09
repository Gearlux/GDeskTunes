#ifndef GSTYLE_H
#define GSTYLE_H

#include <QObject>
#include <QColor>
#include <QString>
#include <QMap>

// Forward declarations
class GStyle;

class Style: public QObject
{
    Q_OBJECT
public:
    virtual void generate(GStyle& style) = 0;
};

class MusicBanner : public Style
{
    Q_OBJECT
    Q_PROPERTY(bool Display MEMBER display)

public:
    MusicBanner() {}
    void generate(GStyle &style);

public:
    bool display;
};

class Banner: public Style
{
    Q_OBJECT
    Q_PROPERTY(QString Background MEMBER background)
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)
    Q_PROPERTY(int BorderWidth MEMBER borderWidth)
    Q_PROPERTY(QColor BorderColor MEMBER borderColor)

public:
    Banner(): background(QString::null), backgroundColor(QColor::Invalid), borderWidth(0), borderColor(QColor::Invalid) {}
    void generate(GStyle &style);

public:
    QString background;
    QColor backgroundColor;
    int borderWidth;
    QColor borderColor;
};

class Search: public Style
{
    Q_OBJECT
    Q_PROPERTY(bool MergeButton MEMBER merge)
    Q_PROPERTY(int BorderRadius MEMBER borderRadius)
    Q_PROPERTY(QColor BorderColor MEMBER borderColor)

public:
    Search() : merge(false), borderRadius(0), borderColor(QColor::Invalid) {}
    void generate(GStyle &style);

public:
    bool merge;
    int borderRadius;
    QColor borderColor;
};

class SideBar: public Style
{
    Q_OBJECT
    Q_PROPERTY(QString Background MEMBER background)
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)

public:
    SideBar() : background(QString::null), backgroundColor(QColor::Invalid) {}
    void generate(GStyle &style);

public:
    QString background;
    QColor backgroundColor;
};

class Player: public Style
{
    Q_OBJECT
    Q_PROPERTY(QString Background MEMBER background)
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)

public:
    Player() : background(QString::null), backgroundColor(QColor::Invalid) {}
    void generate(GStyle &style);

public:
    QString background;
    QColor backgroundColor;
};

class GStyle: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Name MEMBER name)
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)
    Q_PROPERTY(QColor Color MEMBER color)
    Q_PROPERTY(int FontSize MEMBER fontSize)
    Q_PROPERTY(Banner* Banner MEMBER banner)
    Q_PROPERTY(Search* Search MEMBER search)
    Q_PROPERTY(MusicBanner* MusicBanner MEMBER musicBanner)
    Q_PROPERTY(SideBar* SideBar MEMBER sideBar)
    Q_PROPERTY(Player* Player MEMBER player)

public:
    GStyle();
    ~GStyle();

    void generate();
    QString generate(QString name);

public:
    void add_property(QString element, QString key, QString value);
    void add_property(QString element, QString key, const char* value);
    void add_property(QString element, QString key, QColor value);

public:
    QString name;
    QColor backgroundColor;
    QColor color;
    int fontSize;

    Banner* banner;
    MusicBanner* musicBanner;
    Search* search;
    SideBar* sideBar;
    Player* player;

private:
    QMap<QString, QMap<QString, QString> > css_styles;
    void styleBackground();
    void styleForeground();
    void setFontSize();
    void adjustRowHeight();

 private:
    int row_height;
};

#endif // GSTYLE_H
