#ifndef QUTILS_H
#define QUTILS_H

#include <QObject>

void connect(const QObject *sender, const char* signal,
                                const QObject *receiver, const char* method,
                                Qt::ConnectionType type = Qt::AutoConnection, bool verbose = true);
void connect(const QObject *sender, const QMetaMethod &signal,
                                const QObject *receiver, const QMetaMethod &method,
                                Qt::ConnectionType type = Qt::AutoConnection, bool verbose = true);
void connectUI(QObject *ui, QObject *object);
void connectSlotsByName(QObject *sender, QObject *receiver);

#endif // QUTILS_H
