#include <QDebug>
#include <QSettings>
#include <QFileDialog>

#include "gdeskstyler.h"
#include "ui_gdeskstyler.h"

#include "QtProperty"
#include "QtStringPropertyManager"
#include "QtColorPropertyManager"
#include "QtGroupPropertyManager"

#include "QtLineEditFactory"
#include "QtColorEditorFactory"
#include "QtSpinBoxFactory"

GDeskStyler::GDeskStyler(QWidget *parent) :
    QMainWindow(parent),
    gdesktunes(0),
    ui(new Ui::GDeskStyler),
    style(0),
    filename(QString::null),
    modified(false),
    css(QString::null)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    stringManager = new QtStringPropertyManager(this);
    colorManager = new QtColorPropertyManager(this);
    intManager = new QtIntPropertyManager(this);
    groupManager = new QtGroupPropertyManager(this);

    QObject::connect(stringManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(valueChanged(QtProperty *,QString)));
    QObject::connect(colorManager, SIGNAL(valueChanged(QtProperty*,QColor)), this, SLOT(valueChanged(QtProperty *,QColor)));
    QObject::connect(intManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(valueChanged(QtProperty *,int)));

    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(this);
    QtColorEditorFactory *colorEditorFactory = new QtColorEditorFactory(this);
    QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(this);

    ui->property_browser->setFactoryForManager(stringManager, lineEditFactory);
    ui->property_browser->setFactoryForManager(colorManager, colorEditorFactory);
    ui->property_browser->setFactoryForManager(intManager, spinBoxFactory);

    style = new GStyle();
    populate(style);
}

GDeskStyler::~GDeskStyler()
{
    qDebug() << "Deleting the styler";
    ask();

    if (this->gdesktunes && this->css != QString::null)
    {
        QObject *settings = this->gdesktunes->findChild<QObject*>("Settings");
        if (settings != 0)
        {
            qDebug() << "Update styles";
            QMetaObject::invokeMethod(settings, "updateStyles");
        }

        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, ""));
        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, this->css));
    }

    delete ui;
}

void GDeskStyler::populate(QObject *object, QtProperty *parent)
{
    const QMetaObject* meta = object->metaObject();

    for(int i=1; i<meta->propertyCount(); ++i)
    {
        QtProperty *item = 0;
        QMetaProperty property = meta->property(i);
        QVariant variant = property.read(object);
        switch(variant.type())
        {
        case QVariant::String:
            item = stringManager->addProperty(property.name());
            stringManager->setValue(item, variant.toString());
            break;
        case QVariant::Color:
            item = colorManager->addProperty(property.name());
            colorManager->setValue(item, variant.value<QColor>());
            break;
        case QVariant::Int:
            item = intManager->addProperty(property.name());
            intManager->setValue(item, variant.toInt());
            break;
         default:
            qDebug() << "Unsupported property" << property.name() << property.type();
            item = groupManager->addProperty(property.name());
            populate(variant.value<QObject*>(),item);
        }
        if (item)
        {
            if (parent)
                parent->addSubProperty(item);
            else
            {
                QtBrowserItem *it = ui->property_browser->addProperty(item);
                ui->property_browser->setExpanded(it, false);
            }
            holders[item] = object;
        }
    }

}

template <typename T>
void value_changed(QObject *holder, QtProperty *property, T& value)
{
    if (holder == 0)
    {
        qDebug() << "No target object";
        return;
    }
    holder->setProperty(property->propertyName().toUtf8().constData(), value);
}

template<class T> T& as_lvalue(T&& v){ return v; }

void GDeskStyler::valueChanged(QtProperty *property, QString value)
{
    value_changed(holders[property], property, value);
    test();
}

void GDeskStyler::valueChanged(QtProperty *property, QColor value)
{
    value_changed(holders[property], property, value);
    test();
}

void GDeskStyler::valueChanged(QtProperty *property, int value)
{
    value_changed(holders[property], property, value);
    test();
}

void GDeskStyler::test()
{
    stream(as_lvalue(qDebug()), style);

    modified = true;

    if (this->gdesktunes)
    {
        QString css_file = style->generate("__gdesktunes_test");
        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, ""));
        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, "__gdesktunes_test"));
        QFile::remove(css_file);
    }
}

void GDeskStyler::on_actionExit_triggered()
{
    close();
}

void GDeskStyler::on_actionGenerate_triggered()
{
    style->generate();

    this->css = style->name;

    if (this->gdesktunes)
    {
        QObject *settings = this->gdesktunes->findChild<QObject*>("Settings");
        if (settings != 0)
        {
            qDebug() << "Update styles";
            QMetaObject::invokeMethod(settings, "updateStyles");
        }

        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, ""));
        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, style->name));
    }

}

void GDeskStyler::on_actionNew_triggered()
{
    filename = QString::null;

    ui->property_browser->clear();

    style = new GStyle();
    populate(style);
}

void GDeskStyler::on_actionSave_triggered()
{
    if (filename == QString::null)
    {
        on_actionSave_As_triggered();
    }
    else
    {
        save(filename);
    }
}

void GDeskStyler::on_actionSave_As_triggered()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    QString dir = settings.value("gss.dir", QCoreApplication::applicationDirPath()).toString();

    QString filename = QFileDialog::getSaveFileName(this, "Save style sheet", dir, "*.gss");

    if (filename != QString::null)
    {
        save(filename);

        this->filename = filename;

        QFileInfo info(filename);
        settings.setValue("gss.dir", info.absoluteDir().absolutePath());
    }

}

void GDeskStyler::save(QString filename)
{
    style->generate();

    QFile gss_file(filename);
    gss_file.open(QIODevice::WriteOnly);
    QDataStream os(&gss_file);
    stream(os, style);
    gss_file.close();
}

void GDeskStyler::load(QString filename)
{
    QFile gss_file(filename);
    gss_file.open(QIODevice::ReadOnly);
    QDataStream os(&gss_file);
    GStyle *new_style = new GStyle();
    destream(os, new_style);
    stream(as_lvalue(qDebug()), new_style);
    this->filename = filename;

    ui->property_browser->clear();

    style = new_style;
    populate(style);

    modified = false;
}

void GDeskStyler::on_actionOpen_triggered()
{
    ask();

    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    QString dir = settings.value("gss.dir", QCoreApplication::applicationDirPath()).toString();

    QString filename = QFileDialog::getOpenFileName(this, "Load style sheet", dir, "*.gss");

    if (filename != QString::null)
    {
        QFileInfo info(filename);
        settings.setValue("gss.dir", info.absoluteDir().absolutePath());

        load(filename);
    }
}

void GDeskStyler::setGDeskTunes(QObject *gdesktunes)
{
    if (this->gdesktunes)
    {
        qDebug() << "GDeskTunes allready set";
        return;
    }

    this->gdesktunes = gdesktunes;

    // Try to locate the corresponding gss file
    QString css;
    bool success = QMetaObject::invokeMethod(gdesktunes, "getCSS", Q_RETURN_ARG(QString, css));
    if (success)
    {
        qDebug() << "Return value" << css;

        this->css = css;

        QSettings settings(QApplication::organizationName(), QApplication::applicationName());
        QString dir = settings.value("gss.dir", QCoreApplication::applicationDirPath()).toString();
        QString filename = dir + QDir::separator() + css + ".gss";

        QFile gss_file(filename);

        if (gss_file.exists())
        {
            load(filename);
        }
    }
}

void GDeskStyler::ask()
{
    qDebug() << "Save ?";
}
