#ifndef GDESKSTYLER_H
#define GDESKSTYLER_H

#include <QMainWindow>
#include <QMetaProperty>

#include "QtProperty"
#include "QtStringPropertyManager"
#include "QtColorPropertyManager"
#include "QtGroupPropertyManager"
#include "gstyle.h"

namespace Ui {
class GDeskStyler;
}

class GDeskStyler : public QMainWindow
{
    Q_OBJECT

public:
    explicit GDeskStyler(QWidget *parent = 0);
    ~GDeskStyler();

private slots:
    void valueChanged(QtProperty *property, QString value);
    void valueChanged(QtProperty *property, QColor value);
    void valueChanged(QtProperty *property, int value);

    void on_actionNew_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionExit_triggered();

    void on_actionGenerate_triggered();

private:
    void save(QString filename);
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
                stream(os, variant.value<QObject*>());
            }
        }

    }

private:
    Ui::GDeskStyler *ui;

    GStyle *style;

    QMap<QtProperty*,QObject*> holders;

    QString filename;

    QtStringPropertyManager *stringManager;
    QtColorPropertyManager* colorManager;
    QtIntPropertyManager* intManager;
    QtGroupPropertyManager* groupManager;

};

#endif // GDESKSTYLER_H
