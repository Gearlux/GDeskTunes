#ifndef GDESKSTYLER_H
#define GDESKSTYLER_H

#include <QMainWindow>
#include <QMetaProperty>
#include <QDebug>

#include "gstyle.h"

namespace Ui {
class GDeskStyler;
}

// Forward declarations
class QtProperty;
class QtStringPropertyManager;
class QtColorPropertyManager;
class QtIntPropertyManager;
class QtGroupPropertyManager;

class GDeskStyler : public QMainWindow
{
    Q_OBJECT

public:
    explicit GDeskStyler(QWidget *parent = 0);
    ~GDeskStyler();

    void setGDeskTunes(QObject *gdesktunes);


private slots:
    void valueChanged(QtProperty *property, QString value);
    void valueChanged(QtProperty *property, QColor value);
    void valueChanged(QtProperty *property, int value);

    void on_actionNew_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();

    void on_actionGenerate_triggered();

    void on_actionOpen_triggered();

private:
    void save(QString filename);
    void load(QString filename);
    void test();
    void ask();
    void populate(QObject *style, QtProperty *parent=0);

    template <class Stream>
    void stream(Stream &os, QObject *object)
    {
        const QMetaObject *metaObject = object->metaObject();

        for(int i=1; i<metaObject->propertyCount(); ++i)
        {
            QMetaProperty property = metaObject->property(i);
            QVariant variant = property.read(object);
            switch(variant.type())
            {
            case QVariant::Int:
                os << property.name() << variant.toInt();
                break;
            case QVariant::String:
                os << property.name() << variant.toString();
                break;
            case QVariant::Color:
                os << property.name() << variant.value<QColor>();
                break;
            default:
                qDebug() << "Unsupported" << property.name() << variant << variant.type();
                os << property.name();
                stream(os, variant.value<QObject*>());
                os << property.name();
            }
        }
    }
    template <class Stream>
    void destream(Stream &os, QObject *object)
    {
        const QMetaObject *metaObject = object->metaObject();

        for(int i=1; i<metaObject->propertyCount(); ++i)
        {
            QMetaProperty property = metaObject->property(i);
            char* name;
            os >> name;
            qDebug() << "Reading property" << name;
            if (QString(name) != property.name())
            {
                qDebug() << "Wrong serialization, moving on to the next" << name << property.name();
                delete name;
                return;
            }
            delete name;

            QVariant variant;
            switch(property.type())
            {
            case QVariant::Int:
            {
                int v;
                os >> v;
                variant.setValue(v);
            }
                break;
            case QVariant::String:
            {
                QString v;
                os >> v;
                variant.setValue(v);
            }
                break;
            case QVariant::Color:
            {
                QColor v;
                os >> v;
                variant.setValue(v);
            }
                break;
            default:
            {
                qDebug() << "Unsupported" << property.name() << variant << variant.type();
                QObject *obj = property.read(object).value<QObject*>();
                destream(os, obj);
                QString end_value;
                os >> end_value;
            }
            }
            if (variant.isValid())
            {
                qDebug() << property.name() << "=" << variant;
                property.write(object, variant);
            }
        }
    }

private:
    QObject *gdesktunes;
    Ui::GDeskStyler *ui;

    GStyle *style;

    QMap<QtProperty*,QObject*> holders;

    QString css;
    QString filename;
    bool modified;

    QtStringPropertyManager *stringManager;
    QtColorPropertyManager* colorManager;
    QtIntPropertyManager* intManager;
    QtGroupPropertyManager* groupManager;

};

#endif // GDESKSTYLER_H
