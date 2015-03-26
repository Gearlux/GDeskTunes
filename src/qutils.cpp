#define QT_NO_DEBUG_OUTPUT

#include "qutils.h"

#include <QSet>
#include <QTextStream>
#include <QDebug>
#include <QMetaMethod>
#include <QAction>
#include <QPushButton>
#include <QLabel>

QSet<QString> connected;

void connect(const QObject *sender, const char* signal,
                                const QObject *receiver, const char* method,
                                Qt::ConnectionType type, bool verbose)
{
    QString key;
    QTextStream stream(&key);

    // FIXME: Use QMetaObject::normalized
    stream << sender->objectName()  << "." << QString(signal).remove(QRegExp("(^[1,2]|\\s+)")) << " -> " << receiver->objectName() << "." << QString(method).remove(QRegExp("(^[1,2]|\\s+)"));
    if (connected.contains(key))
    {
        if (verbose)
            qDebug() << "Not connecting: " << key;
        return;
    }
    if (verbose)
        qDebug() << "Connecting: " << key;
    connected.insert(key);
    QObject::connect(sender, signal, receiver, method, type);
}

void connect(const QObject *sender, const QMetaMethod &signal,
                                const QObject *receiver, const QMetaMethod &method,
                                Qt::ConnectionType type, bool verbose)
{
    QString key;
    QTextStream stream(&key);

    // FIXME: Use QMetaObject::normalized
    stream << sender->objectName()  << "." << QString(signal.methodSignature()).remove(QRegExp("(^[1,2]|\\s+)")) << " -> " << receiver->objectName() << "." <<QString(method.methodSignature()).remove(QRegExp("(^[1,2]|\\s+)"));

    if (connected.contains(key))
    {
        if (verbose)
            qDebug() << "Not connecting: " << key;
        return;
    }
    if (verbose)
        qDebug() << "Connecting: " << key;
    connected.insert(key);
    QObject::connect(sender, signal, receiver, method, type);
}

void connectUI(QObject *ui, QObject *object)
{
    if (!ui) return;
    if (!object) return;

    qDebug() << "connectUI(" << ui->objectName() << ","<< object->objectName() <<")";

    const QMetaObject *mobject = object->metaObject();

    const QObjectList list = ui->findChildren<QObject *>(QString());

    for(int o=0; o<list.length(); ++o)
    {
        const QObject *ui_object = list.at(o);
        const QMetaObject *mui = ui_object->metaObject();

        QString objectName = ui_object->objectName();
        int underscore = objectName.indexOf('_');
        while(underscore != -1)
        {
            objectName.remove(underscore, 1);
            objectName[underscore] = objectName[underscore].toUpper();
            underscore = objectName.indexOf('_');
        }

        QString slot_method, argument;
        int signal_index = -1;

        // Not the best programming practice, but this code is more elegant than solving it with if/else
        if (qobject_cast<const QAction*>(ui_object) != 0) goto _connect_action;
        if (qobject_cast<const QPushButton*>(ui_object) != 0) goto _button;
        if (qobject_cast<const QLabel*>(ui_object) != 0) goto _button;

        // Connect GUI elements
        signal_index = mui->indexOfSignal("toggled(bool)");
        if (signal_index != -1) { slot_method = "setChecked(bool)"; argument = "(bool)"; goto _connect;}

        signal_index = mui->indexOfSignal("currentIndexChanged(QString)");
        if (signal_index != -1) { slot_method = "setCurrentText(QString)"; argument = "(QString)"; goto _connect;}

        signal_index = mui->indexOfSignal("valueChanged(int)");
        if (signal_index != -1) { slot_method = "setValue(int)"; argument = "(int)"; goto _connect; }

_button:
        signal_index = mui->indexOfSignal("clicked()");
        if (signal_index != -1) { slot_method = QString::null; argument = "()"; goto _connect;}

_connect:
        if (signal_index != -1)
        {
            QString setter;
            if (slot_method == QString::null)
            {
                setter = objectName + argument;
            }
            else
            {
                setter = "set" + objectName + argument;
                setter[3] = setter[3].toUpper();
            }

            int slot_index = mobject->indexOfMethod(setter.toUtf8().constData());
            if (slot_index != -1)
            {
                connect(ui_object, mui->method(signal_index), object, mobject->method(slot_index));
            }
        }

        if (slot_method == QString::null) continue;

        signal_index = mobject->indexOfSignal((objectName + argument).toUtf8().constData());
        if (signal_index != -1)
        {
            int slot_index = mui->indexOfSlot(slot_method.toUtf8().constData());
            if (slot_index != -1)
            {
                connect(object, mobject->method(signal_index), ui_object, mui->method(slot_index));
            }
        }
        // No need to check if this is an action
        continue;

        // Connect actions
_connect_action:
        signal_index = mui->indexOfSignal("triggered()");
        if (signal_index != -1)
        {
            QString slotMethod;
            if (objectName.startsWith("action"))
                slotMethod = objectName.mid(6);
            else
                slotMethod = objectName;
            slotMethod[0] = slotMethod[0].toLower();

            int slot_index = mobject->indexOfMethod((slotMethod + "()").toUtf8().constData());
            if (slot_index != -1)
            {
                connect(ui_object, mui->method(signal_index), object, mobject->method(slot_index));
            }
        }
    }
}

void connectSlotsByName(QObject *sender, QObject *receiver)
{
    if (!sender) return;
    if (!receiver) return;
    qDebug() << "connectSlotsByName("<< sender->objectName() <<","<< receiver->objectName() <<")";

    const QMetaObject *msender = sender->metaObject();
    const QMetaObject *mreceiver = receiver->metaObject();

    for(int s=0; s<msender->methodCount(); ++s)
    {
        QMetaMethod ms = msender->method(s);
        if (ms.methodType() != QMetaMethod::Signal) continue;
        const QByteArray signalSignature = ms.methodSignature();
        const char *signal = signalSignature.constData();

        if (strncmp(signal,"destroyed", 9) == 0) continue;
        if (strncmp(signal,"deleteLater", 11) == 0) continue;
        if (strncmp(signal,"objectNameChanged", 16) == 0) continue;
        if (strncmp(signal,"_q_reregisterTimers", 18) == 0) continue;

        for(int m=0; m<mreceiver->methodCount(); ++m)
        {
            QMetaMethod mr = mreceiver->method(m);
            if (mr.methodType() != QMetaMethod::Slot) continue;
            const QByteArray slotSignature = mr.methodSignature();
            const char *slot = slotSignature.constData();

            if (strcmp(signal, slot) == 0)
            {
                connect(sender, ms, receiver, mr, Qt::AutoConnection, true);
            }
        }
    }

}
