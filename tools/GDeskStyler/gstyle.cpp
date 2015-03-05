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
        add_property(element, key, QString("rgb(%1,%2,%3,%4)").arg(value.red()).arg(value.green()).arg(value.blue()).arg(value.alpha()));
    }
}

void GStyle::generate()
{
    css_styles.clear();

    if (name == QString::null)
    {
        qDebug() << "No name specified";
        return;
    }

#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../Resources/userstyles" + QDir::separator());
#else
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "userstyles" + QDir::separator());
#endif
    if (!dir.exists())
        dir.mkpath(dir.absolutePath());

    // Copied from the python code
    add_property("body", "background-color", backgroundColor);
    add_property("#oneGoogleWrapper > div:first-child > div:first-child", "background", "none");
    add_property(".music-banner", "background-color", "inherit");
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

    QFile css_file(dir.absoluteFilePath(name + ".css"));
    qDebug() << dir.absoluteFilePath(name + ".css");
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
}

