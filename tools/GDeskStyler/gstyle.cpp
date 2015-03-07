#include "gstyle.h"

#include <QDebug>
#include <QDir>
#include <QApplication>
#include <QTextStream>

GStyle::GStyle() :
    fontSize(0),
    banner(new Style())
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

QString GStyle::generate(QString name)
{
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

    // Copied from the python code to set the background color
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
    add_property(".song-row.hover .song-indicator", "background-color", "transparent");
    add_property(".song-row td", "background-color", "transparent");
    add_property(".song-row .fade-out:after", "background", "transparent");
    add_property("#player", "background-color", "inherit");
    add_property(".now-playing-menu-wrapper", "background", "inherit");
    add_property("#player:not(.material) [data-id=\"now-playing-menu\"]", "border", "inherit");
    add_property(".now-playing-menu-wrapper.fade-out:after", "background", "inherit");
    add_property(".player-middle", "background-color", "inherit");
    add_property(".card", "background", "rgba(128,128,128,0.5)");
    add_property(".button.small", "background", "rgba(128,128,128,0.5)");

    // Copied from the python code to set the foreground color
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

