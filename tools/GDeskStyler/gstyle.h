#ifndef GSTYLE_H
#define GSTYLE_H

#include <QObject>
#include <QColor>
#include <QString>
#include <QMap>

class Border: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int Width MEMBER width)

public:
    int width;
};

class Style: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)
    Q_PROPERTY(QString Background MEMBER background)

public:
    QColor backgroundColor;
    QString background;
};

class GStyle: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Name MEMBER name)
    Q_PROPERTY(QColor BackgroundColor MEMBER backgroundColor)
    Q_PROPERTY(QColor Color MEMBER color)
    Q_PROPERTY(int FontSize MEMBER fontSize)
    Q_PROPERTY(Style* Banner MEMBER banner)

public:
    GStyle();
    ~GStyle();

    void generate();

private:
    void add_property(QString element, QString key, QString value);
    void add_property(QString element, QString key, const char* value);
    void add_property(QString element, QString key, QColor value);

public:
    QString name;
    QColor backgroundColor;
    QColor color;
    int fontSize;

    Style *banner;

    QMap<QString, QMap<QString, QString> > css_styles;
};

#endif // GSTYLE_H
