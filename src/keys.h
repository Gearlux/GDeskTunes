#ifndef KEYFORWARDER_H
#define KEYFORWARDER_H

#include <QKeyEvent>

template <class T>
class KeyReceiver: public T
{
public:
    KeyReceiver(QWidget *parent = 0) : T(parent)
    {
    }
    virtual ~KeyReceiver()
    {
    }

public:
    void keyPressEvent(QKeyEvent *event)
    {
        T::keyPressEvent(event);
    }

};

template <class T, class Parent>
class KeyForwarder: public T
{
public:
    KeyForwarder(QWidget *parent = 0) : T(parent)
    {
        // Nothing to do here
    }
    virtual ~KeyForwarder()
    {
        // Nothing to do here
    }

public:
    void keyPressEvent(QKeyEvent *event)
    {
        T::keyPressEvent(event);
        if (!event->isAccepted())
        {
            Parent *w = dynamic_cast<Parent*>(T::parentWidget());
            if (w != 0)
            {
                qDebug() << "Forwarding keyPress: " << T::objectName() << event->key();
                w->keyPressEvent(event);
            }
        }
    }

};

#endif // KEYFORWARDER_H
