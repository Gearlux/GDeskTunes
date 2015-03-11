#include "gstyle.h"

#include <QDebug>
#include <QDir>
#include <QApplication>
#include <QTextStream>

void Style::parseExtra(GStyle &style, QString elt, QString extra)
{
    QStringList elements = extra.split(";", QString::SkipEmptyParts);
    for(int i=0; i<elements.size(); ++i)
    {
        QStringList element = elements.at(i).split(":", QString::SkipEmptyParts);
        if (element.size() == 2)
        {
            style.add_property(elt, element.at(0).trimmed(), element.at(1).trimmed());
        }
    }
}

void Background::generate(GStyle &style, QString elt)
{
    if (this->backgroundColor.isValid())
        style.add_property(elt, "background-color", this->backgroundColor);
    if (!this->background.isNull())
        style.add_property(elt, "background", this->background);
}

void Border::generate(GStyle &style, QString elt)
{
    if (this->borderColor.isValid())
        style.add_property(elt, QString("border%1-color").arg(borderSide), borderColor);
    if (this->borderRadius != 0)
        style.add_property(elt, QString("border%1-radius").arg(borderSide), QString("%1px").arg(this->borderRadius));
    if (this->borderWidth != 0)
    {
        style.add_property(elt, QString("border%1-width").arg(borderSide), QString("%1px").arg(this->borderWidth));
        style.add_property(elt, QString("border%1-style").arg(borderSide), "solid");
    }
}

void BorderStyle::generate(GStyle &style, QString elt)
{
    Background::generate(style, elt);
    Border::generate(style, elt);
    Style::parseExtra(style, elt, extra);
}

void MusicBanner::generate(GStyle& style)
{
    if (!this->display)
    {
        style.add_property(".music-banner", "display", "none");
        style.add_property("#nav", "top", "0");
    }
}

void Banner::generate(GStyle &style)
{
    QString elt("#oneGoogleWrapper > div:first-child > div:first-child");
    Background::generate(style, elt);
    Border::generate(style, elt);
    if (this->borderWidth != 0)
    {
        style.add_property(elt, "height", QString("%1px").arg(60 - this->borderWidth));
    }
}

void Search::generate(GStyle &style)
{
    if (this->merge)
    {
        style.add_property("#gbqfb", "border", "none");
        style.add_property("#gbqfb", "width", "24px");
        style.add_property("#gbqfb", "height", "24px");
        style.add_property("#gbqfb", "min-width", "24px");
        style.add_property("#gbqfb", "margin-left", "-35px");
        style.add_property("#gbqfb", "position", "absolute");
        style.add_property("#gbqfb", "background", "transparent");
        style.add_property("#gbqfb", "-webkit-filter", "invert(50%)");
    }
    if (this->borderRadius != 0)
    {
        style.add_property(".gbqfqw", "border-radius", QString("%1px").arg(borderRadius));
    }
    if (this->borderColor.isValid())
    {
        style.add_property(".gbqfqw", "border-color", this->borderColor);
    }
    parseExtra(style, ".gbqfqw", extra);
}

void NavigationBar::generate(GStyle &style)
{
    Background::generate(style, ".nav-bar");
    style.add_property(".nav-bar #breadcrumbs", "background", "none");
    button->generate(style, ".header-tab-title.selected");
}

void SideBar::generate(GStyle &style)
{
    Background::generate(style, "#nav");
}

void Player::generate(GStyle &style)
{
    Background::generate(style, "#player");
}

void ScrollBar::generate(GStyle &style)
{
    if (this->width != 0)
    {
        style.add_property("::-webkit-scrollbar", "width", QString("%1px").arg(this->width));
    }
    track->generate(style, "::-webkit-scrollbar-track");
    thumb->generate(style, "::-webkit-scrollbar-thumb");
}

void LoadingPage::generate(GStyle &style)
{
    if (!this->background.isNull())
        style.add_property("#loading-progress", "background", this->background);
    if (this->backgroundColor.isValid())
        style.add_property("#loading-progress", "background-color", this->backgroundColor);
    if (this->color.isValid())
        style.add_property("#loading-progress-message", "color", this->color);
    if (this->position != 0)
        style.add_property("#loading-progress-content", "top", QString("%1%%").arg(this->position));
    if (this->height != 0)
    {
        style.add_property("#loading-progress-bar", "height", QString("%1px").arg(this->height));
        style.add_property("#current-loading-progress", "height", QString("%1px").arg(this->height));
    }
}

GStyle::GStyle() :
    name(QString::null),
    backgroundColor(QColor::Invalid),
    color(QColor::Invalid),
    fontSize(0),
    banner(new Banner()),
    musicBanner(new MusicBanner()),
    search(new Search()),
    navigationBar(new NavigationBar()),
    sideBar(new SideBar()),
    player(new Player()),
    loadingPage(new LoadingPage()),
    scrollBar(new ScrollBar())
{

}

GStyle::~GStyle()
{

}

void GStyle::add_property(QString element, QString key, QString value)
{
    if (!css_styles.contains(element))
    {
        css_styles[element] = QMap<QString, QString>();
    }
    QMap<QString,QString> &the_map = css_styles[element];
    the_map[key] = value;
}

void GStyle::add_property(QString element, QString key, const char* value)
{
    add_property(element, key, QString(value));
}

void GStyle::add_property(QString element, QString key, QColor value)
{
    if (value.isValid())
    {
        add_property(element, key, QString("rgba(%1,%2,%3,%4)").arg(value.red()).arg(value.green()).arg(value.blue()).arg(value.alpha()));
    }
}

void GStyle::styleBackground()
{
    add_property("body", "background-color", backgroundColor);
    add_property("#oneGoogleWrapper > div:first-child > div:first-child", "background", "none");
    // add_property(".music-banner", "background-color", "inherit");
    add_property("#headerBar", "background-color", "inherit");
    add_property(".nav-bar", "background-color", "inherit");
    add_property("#breadcrumbs", "background-color", "inherit");
    add_property("#nav", "background-color", "inherit");
    add_property(".fade-out:after", "background", "inherit");
    add_property("#music-content", "background-color", "inherit");
    add_property(".fade-out.gray:after", "background", "inherit");
    add_property(".read-more-button-wrapper.fade-out", "background", "inherit");
    add_property(".song-row .song-indicator", "background-color", backgroundColor);
    add_property(".song-row.hover .song-indicator", "background-color", "inherit");
    add_property(".song-row td", "background-color", "inherit");
    add_property(".song-row .fade-out:after", "background", "inherit");
    add_property("#player", "background-color", "inherit");
    add_property(".now-playing-menu-wrapper", "background", "inherit");
    add_property("#player:not(.material) [data-id=\"now-playing-menu\"]", "border", "inherit");
    add_property(".now-playing-menu-wrapper.fade-out:after", "background", "inherit");
    add_property(".player-middle", "background-color", "inherit");
    add_property("#loading-progress", "background-color", "inherit");
}

void GStyle::styleForeground()
{
    add_property("body", "color", color);
    add_property(".music-banner-title", "color", "inherit");
    add_property("#nav:not(.material) #nav_collections.nav-section .nav-item-container", "color", "inherit");
    add_property(".nav-item-container", "color", "inherit");
    add_property(".nav-section-header", "color", "inherit");
    add_property("#headerBar .tab-text.spacer", "color", "inherit");
    add_property("#headerBar .tab-text", "color", "inherit");
    add_property("#headerBar .tab-text.has-parent", "color", "inherit");
    add_property("#headerBar .tab-text.parent", "color", "inherit");
    add_property(".header-tab-title", "color", "inherit");
    add_property(".header-tab-title.selected", "color", "inherit");
    add_property("#countSummary", "color", "inherit");
    add_property(".artist-view .bio", "color", "inherit");
    add_property(".section-header", "color", "inherit");
    add_property(".cluster .header .title", "color", "inherit");
    add_property(".cluster .header", "color", "inherit");
    add_property(".recommended-header", "color", "inherit");
    add_property(".playlist-view .description", "color", "inherit");
    add_property(".playlist-view .song-count", "color", "inherit");
    add_property(".playlist-view .song-duration", "color", "inherit");
    add_property(".playlist-view .owner-name", "color", "inherit");
    add_property(".album-view .song-count", "color", "inherit");
    add_property(".album-view .album-length", "color", "inherit");
    add_property(".album-view .year", "color", "inherit");
    add_property(".genres .genre", "color", "inherit");
    add_property(".song-table th", "color", "inherit");
    add_property(".song-row td", "color", "inherit");
    add_property(".song-table [data-col=\"track\"] .content", "color", "inherit");
    add_property(".song-table [data-col=\"track\"]", "color", "inherit");
    add_property("#player", "color", "inherit");
    add_property("#playerSongTitle", "color", "inherit");
    add_property("#player-artist", "color", "inherit");
    add_property(".player-album", "color", "inherit");
    add_property(".player-dash", "color", "inherit");
    add_property("#loading-progress-message", "color", "inherit");
}

void GStyle::setFontSize()
{
    if (fontSize == 0) { row_height =0; return; }
    float fraction = 36.f / 18.f;
    add_property("#nav:not(.material) #nav_collections.nav-section .nav-item-container", "font-size", QString("%1px").arg(fontSize));
    add_property("#nav:not(.material) #nav_collections.nav-section .nav-item-container", "height", QString("%1px").arg((int)(fontSize*fraction)));
    add_property("#nav:not(.material) #nav_collections.nav-section .nav-item-container", "line-height", QString("%1px").arg((int)(fontSize*fraction)));

    // Playlist items
    fraction = 30.f / 14.f;
    add_property(".nav-item-container", "font-size", QString("%1px").arg(fontSize));
    add_property(".nav-item-container", "height", QString("%1px").arg((int)(fontSize*fraction)));
    add_property(".nav-item-container", "line-height", QString("%1px").arg((int)(fontSize*fraction)));

    // Header of the table
    fraction = 36.f / 12.f;
    add_property(".song-table th", "font-size", QString("%1px").arg(fontSize));
    add_property(".song-table th", "height", QString("%1px").arg((int)(fontSize * fraction)));

    fraction = 40.f / 14.f;
    row_height = (int)(fontSize * fraction);
    add_property(".song-row td", "font-size", QString("%1px").arg(fontSize));
}

void GStyle::adjustRowHeight()
{
    if (row_height == 0) return;

    int new_height = row_height;
    add_property(".song-row td", "height", QString("%1px").arg(new_height));
    add_property(".song-row td", "max-height", QString("%1px").arg(new_height));
    add_property(".song-row td", "min-height", QString("%1px").arg(new_height));
    add_property(".song-row td", "line-height", QString("%1px").arg(new_height));

    add_property(".song-table [data-col=\"title\"] img", "height", QString("%1px").arg(new_height));
    add_property(".song-table [data-col=\"title\"] img", "width", QString("%1px").arg(new_height));

    add_property(".song-row .hover-button[data-id=\"menu\"]", "margin", QString("%1dpx 2px %2px 2px").arg((row_height-24)/2).arg((row_height-24)/2));
    add_property(".song-row .rating-container.stars", "margin", QString("10px 0 %1px 7px").arg(-row_height));

    add_property(".song-row .song-indicator", "width", QString("%1px !important").arg(new_height));
    add_property(".song-row .song-indicator", "height", QString("%1px !important").arg(new_height));
    add_property(".song-row .hover-button[data-id=\"play\"]","height", QString("%1px").arg(new_height));
    add_property(".song-row .hover-button[data-id=\"play\"]","width", QString("%1px").arg(new_height));

}

QString GStyle::generate(QString name)
{
    qDebug() << "GStyle::generate(" << name << ")";
    if (name == QString::null)
    {
        qDebug() << "No name specified";
        return QString::null;
    }
    css_styles.clear();


#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../Resources/userstyles" + QDir::separator());
#else
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "userstyles" + QDir::separator());
#endif
    if (!dir.exists())
        dir.mkpath(dir.absolutePath());

    // Style the background
    styleBackground();
    // Style the foreground
    styleForeground();
    // Set the font size
    setFontSize();

    banner->generate(*this);
    search->generate(*this);
    musicBanner->generate(*this);
    navigationBar->generate(*this);
    sideBar->generate(*this);
    player->generate(*this);
    loadingPage->generate(*this);
    scrollBar->generate(*this);

    // Adjust the row height
    adjustRowHeight();

    // Create the css file
    QString filename = dir.absoluteFilePath(name + ".css");
    QFile css_file(filename);
    qDebug() << filename;
    css_file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream css(&css_file);

    QMap<QString, QMap<QString,QString> >::iterator it;
    for(it = css_styles.begin(); it != css_styles.end(); ++it)
    {
        css << it.key() << "{";
        QMap<QString, QString>::iterator j;
        for(j = it.value().begin(); j != it.value().end(); ++j)
        {
            css << j.key() << ":" << j.value() << ";";
        }
        css << "}" << endl;
    }
    css_file.close();
    return filename;

}

void GStyle::generate()
{
    generate(this->name);
}

