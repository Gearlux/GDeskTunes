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

    static void parseExtra(GStyle& style, QString elt, QString extra);
};

class Background
{
public:
    Background() : background(QString::null), backgroundColor(QColor::Invalid) {}

protected:
    void generate(GStyle &style, QString elt);

public:
    QString background;
    QColor backgroundColor;
};

class Border
{
public:
    Border() : borderRadius(0), borderWidth(0), borderColor(QColor::Invalid), borderSide("") {}
    Border(QString side) : borderRadius(0), borderWidth(0), borderColor(QColor::Invalid), borderSide(side) {}
    void generate(GStyle &style, QString elt);

public:
    int borderRadius;
    int borderWidth;
    QColor borderColor;
    QString borderSide;
};

class BorderStyle: public QObject, public Background, public Border
{
    Q_OBJECT
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)
    Q_PROPERTY(QColor BorderColor MEMBER borderColor)
    Q_PROPERTY(int BorderWidth MEMBER borderWidth)
    Q_PROPERTY(int BorderRadius MEMBER borderRadius)
    Q_PROPERTY(QString CSS MEMBER extra)

public:
    BorderStyle() {}
    void generate(GStyle &style, QString elt);
public:
    QString extra;
};

class MusicBanner : public Style
{
    Q_OBJECT
    Q_PROPERTY(bool Display MEMBER display)

public:
    MusicBanner(): display(true) {}
    void generate(GStyle &style);

public:
    bool display;
};

class Banner: public Style, public Background, public Border
{
    Q_OBJECT
    Q_PROPERTY(QString Background MEMBER background)
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)
    Q_PROPERTY(int BorderWidth MEMBER borderWidth)
    Q_PROPERTY(QColor BorderColor MEMBER borderColor)

public:
    Banner(): Border("-bottom") {}
    void generate(GStyle &style);
};

class Search: public Style
{
    Q_OBJECT
    Q_PROPERTY(bool MergeButton MEMBER merge)
    Q_PROPERTY(int BorderRadius MEMBER borderRadius)
    Q_PROPERTY(QColor BorderColor MEMBER borderColor)
    Q_PROPERTY(QString CSS MEMBER extra)

public:
    Search() : merge(false), borderRadius(0), borderColor(QColor::Invalid) {}
    void generate(GStyle &style);

public:
    bool merge;
    int borderRadius;
    QColor borderColor;
    QString extra;
};

class NavigationBar: public Style, public Background, public Border
{
    Q_OBJECT
    Q_PROPERTY(QString Background MEMBER background)
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)
    Q_PROPERTY(BorderStyle* Button MEMBER button)

public:
    NavigationBar(): button(new BorderStyle()) {}
    void generate(GStyle &style);
public:
    BorderStyle* button;
};

class SideBar: public Style, public Background
{
    Q_OBJECT
    Q_PROPERTY(QString Background MEMBER background)
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)

public:
    void generate(GStyle &style);
};

class Player: public Style, public Background
{
    Q_OBJECT
    Q_PROPERTY(QString Background MEMBER background)
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)

public:
    void generate(GStyle &style);
};

class LoadingPage: public Style, public Background
{
    Q_OBJECT
    Q_PROPERTY(QString Background MEMBER background)
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)
    Q_PROPERTY(QColor Color MEMBER color)
    Q_PROPERTY(int HeightPosition MEMBER position)
    Q_PROPERTY(int height MEMBER height)

public:
    LoadingPage(): color(QColor::Invalid), position(0), height(0) {}
    void generate(GStyle &style);

public:
    QColor color;
    int position;
    int height;
    // TODO #loading-progress-message { text-align: center; }
};

class ScrollBar: public Style
{
    Q_OBJECT
    Q_PROPERTY(int width MEMBER width)
    Q_PROPERTY(BorderStyle* Track MEMBER track)
    Q_PROPERTY(BorderStyle* Thumb MEMBER thumb)

public:
    ScrollBar() : width(0), track(new BorderStyle()), thumb(new BorderStyle()) {}
    void generate(GStyle &style);

public:
    int width;
    BorderStyle *track;
    BorderStyle *thumb;
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
    Q_PROPERTY(NavigationBar* NavigationBar MEMBER navigationBar)
    Q_PROPERTY(SideBar* SideBar MEMBER sideBar)
    Q_PROPERTY(Player* Player MEMBER player)
    Q_PROPERTY(LoadingPage* LoadingPage MEMBER loadingPage)
    Q_PROPERTY(ScrollBar* ScrollBar MEMBER scrollBar)

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
    NavigationBar* navigationBar;
    SideBar* sideBar;
    Player* player;
    LoadingPage* loadingPage;
    ScrollBar* scrollBar;

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
