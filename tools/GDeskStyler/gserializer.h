#ifndef GPARSER_H
#define GPARSER_H

#include <QThread>

class GSerializer
{
public:
    GSerializer(): name(0) {}
    ~GSerializer() { if (name != 0) delete name; name = 0; }

    template <class Stream>
    void serialize(Stream &os, const QObject* object)
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
            case QVariant::Bool:
                os << property.name() << variant.toBool();
                break;
            default:
                qDebug() << "Unsupported" << property.name() << variant << variant.type();
                os << property.name();
                serialize(os, variant.value<QObject*>());
                os << property.name();
            }
        }
    }
    template <class Stream>
    void deserialize(Stream &os, QObject* object) { if (name != 0) delete name; name = 0; destream(os, object); }

private:
    template <class Stream>
    void destream(Stream &os, QObject *object)
    {
        const QMetaObject *metaObject = object->metaObject();

        if (name == 0)
            os >> name;

        // Try to find the property
        while(true)
        {
            qDebug() << "Searching property" << name;
            for(int i=1; i<metaObject->propertyCount(); ++i)
            {
                QMetaProperty property = metaObject->property(i);
                if (QString(name) != property.name())
                {
                    continue;
                }
                // Found it, so delete it
                delete name; name = 0;

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
                case QVariant::Bool:
                {
                    bool v;
                    os >> v;
                    variant.setValue(v);
                }
                    break;
                default:
                {
                    qDebug() << "Sub property" << property.name() << variant << variant.type();
                    QObject *obj = property.read(object).value<QObject*>();
                    // We will deserialize it
                    destream(os, obj);
                    // After the deserialization, the name of the property must be present in the name
                    if (QString(name) != property.name())
                    {
                        qDebug() << "Problem deserialization";
                    }
                    else
                    {
                        delete name; name = 0;
                    }
                }
                }
                if (variant.isValid())
                {
                    qDebug() << property.name() << "=" << variant;
                    property.write(object, variant);
                }
                break;
            }
            if (name != 0)
            {
                // We did not find the property
                qDebug() << "Property can not be found" << name;
                break;
            }
            // Read the other properties
            os >> name;
            if (name == 0) break;
        }
        qDebug() << "Leaving sub property";
    }

private:
    char* name;
};

#endif // GPARSER_H
