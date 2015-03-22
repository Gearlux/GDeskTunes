#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QScreen>

#include "gdeskstyler.h"
#include "ui_gdeskstyler.h"

#include "donatedialog.h"

#include "QtStringPropertyManager"
#include "QtColorPropertyManager"
#include "QtGroupPropertyManager"

#include "QtLineEditFactory"
#include "QtColorEditorFactory"
#include "QtSpinBoxFactory"

Element::~Element()
{
   delete ui;
}

GDeskStyler::GDeskStyler(QWidget *parent) :
    QMainWindow(parent),
    gdesktunes(0),
    ui(new Ui::GDeskStyler),
    css(QString::null),
    filename(QString::null),
    modified(false),
    block(false)
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());
    qDebug() << QApplication::organizationName() << QApplication::applicationName();
    qDebug() << settings.fileName();

    int next_count = settings.value("gdeskstyler.next", 0).toInt();
    int this_count = settings.value("gdeskstyler.current", 0).toInt();

    qDebug() << "GDeskStyler::GDeskStyler()" << next_count << this_count;
    if (next_count <= this_count)
    {
        DonateDialog *dlg = new DonateDialog(this);
        settings.setValue("gdeskstyler.next", qMax(1, next_count * 2));
        dlg->show();
    }

    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    stringManager = new QtStringPropertyManager(this);
    colorManager = new QtColorPropertyManager(this);
    intManager = new QtIntPropertyManager(this);
    boolManager = new QtBoolPropertyManager(this);
    groupManager = new QtGroupPropertyManager(this);

    QObject::connect(stringManager, SIGNAL(valueChanged(QtProperty*,QString)), this, SLOT(valueChanged(QtProperty *,QString)));
    QObject::connect(colorManager, SIGNAL(valueChanged(QtProperty*,QColor)), this, SLOT(valueChanged(QtProperty *,QColor)));
    QObject::connect(intManager, SIGNAL(valueChanged(QtProperty*,int)), this, SLOT(valueChanged(QtProperty *,int)));
    QObject::connect(boolManager, SIGNAL(valueChanged(QtProperty*,bool)), this, SLOT(valueChanged(QtProperty *,bool)));

    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(this);
    QtColorEditorFactory *colorEditorFactory = new QtColorEditorFactory(this);
    QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(this);
    QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(this);

    ui->property_browser->setFactoryForManager(stringManager, lineEditFactory);
    ui->property_browser->setFactoryForManager(colorManager, colorEditorFactory);
    ui->property_browser->setFactoryForManager(intManager, spinBoxFactory);
    ui->property_browser->setFactoryForManager(boolManager, checkBoxFactory);
}

GDeskStyler::~GDeskStyler()
{
    qDebug() << "Deleting the styler";

    if (this->gdesktunes && this->css != QString::null)
    {
        QObject *settings = this->gdesktunes->findChild<QObject*>("Settings");
        // QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, ""));
        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, this->css));
        if (settings != 0)
        {
            qDebug() << "Update styles";
            QMetaObject::invokeMethod(settings, "updateStyles");
        }

    }

    delete ui;
}

void GDeskStyler::show()
{
    QCoreApplication *app = QCoreApplication::instance();

    QApplication *gui = dynamic_cast<QApplication*>(app);
    if ( gui != 0)
    {
        QList<QScreen *> screens = gui->screens();
        QPoint pt = pos();
        for(int s=0; s<screens.size(); ++s)
        {
            QRect screen_rect = screens.at(s)->geometry();
            if (screen_rect.contains(pt))
            {
                setMaximumSize(screen_rect.width(), screen_rect.height()-200);
                break;
            }
        }
    }
    QMainWindow::show();
}

void GDeskStyler::populate()
{
    qDebug() << "GDeskStyler::populate()";

    QRegExp comment_re("/\\*(.*)\\*/");

    block = true;

    qDeleteAll(elements);
    style.clear();
    elements.clear();
    groups.clear();
    holders.clear();

    QFile qfile(this->filename);
    qfile.open(QFile::ReadOnly | QFile::Text);

    QTextStream os(&qfile);

    int line_nr = 0;
    while(!os.atEnd())
    {
        QString line = os.readLine();
        style.append(line);
        if (line.startsWith("$"))
        {
            int c = line.indexOf(':', 1);
            if (c != -1)
            {
                QString full_name = line.mid(1, c-1).trimmed();

                QStringList name_parts = full_name.split("-");

                QString group_name = name_parts.at(0);
                QtProperty *parent = 0;
                for(int i=1; i<name_parts.size(); ++i)
                {
                    QString n = name_parts.at(i-1);
                    qDebug() << "Part" << n << "from" << name_parts << group_name;
                    if (groups.contains(group_name))
                    {
                        parent = groups[group_name]->ui;
                    }
                    else
                    {
                        QtProperty* child = groupManager->addProperty(n.replace("_", " "));
                        if (parent)
                        {
                            parent->addSubProperty(child);
                        }
                        else
                        {
                            ui->property_browser->addProperty(child);
                        }
                        Element *elt = new Element(full_name, -1);
                        elt->ui = child;
                        elements.append(elt);
                        parent = child;
                        groups[group_name] = elt;
                    }
                    group_name = group_name + "-";
                    group_name += name_parts.at(i);
                }


                qDebug() << full_name;
                Element* elt = new Element(full_name, line_nr);
                elements.append(elt);
                QtProperty *property = 0;

                int comma = line.indexOf(';', c);
                QString value = line.mid(c+1, comma-c-1).trimmed();

                int comment_index = line.indexOf(comment_re, comma);

                if (comment_index != -1)
                {
                    elt->comment = comment_re.cap(0);
                    QString comment = comment_re.cap(1);
                    QStringList parts = comment.split(',');

                    qDebug() << parts.at(0);
                    if (parts.at(0).trimmed() == "color")
                    {
                        property = colorManager->addProperty(name_parts.last().replace("_", " "));
                        QColor color(value);
                        colorManager->setValue(property, color);
                        elt->value = color;
                    }
                    if (parts.at(0).trimmed() == "int")
                    {
                        property = intManager->addProperty(name_parts.last().replace("_", " "));
                        int v = value.toInt();
                        intManager->setValue(property, v);
                        elt->value = v;

                    }

                }
                else
                {
                    // We assume a string
                    property = stringManager->addProperty(name_parts.last().replace("_", " "));
                    stringManager->setValue(property, value);
                    elt->value = value;
                }


                elt->ui = property;
                holders[property] = elt;

                if (parent == 0)
                {
                    QtBrowserItem *it = ui->property_browser->addProperty(property);
                    ui->property_browser->setExpanded(it, false);
                }
                else
                {
                    parent->addSubProperty(property);

                }
            }
        }
        line_nr += 1;
    }


    qfile.close();

    block = false;

}

template <typename T>
void value_changed(Element *holder, QtProperty *property, T& value)
{
    if (holder == 0)
    {
        qDebug() << "No target object";
        return;
    }
    holder->value = value;
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

void GDeskStyler::valueChanged(QtProperty *property, bool value)
{
    // value_changed(holders[property], property, value);
    test();
}

void GDeskStyler::test()
{
    if (block) return;

    qDebug() << "GDeskStyler::test()";

    modified = true;

    if (this->gdesktunes)
    {
        qDebug() << "Apply test css";
#ifdef Q_OS_MAC
        QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../Resources/userstyles");
#else
        QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "userstyles");
#endif
        QString css_file = dir.absolutePath() + QDir::separator() + "__gdesktunes_test.scss";
        save(css_file);
        // QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, ""));
        QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, "__gdesktunes_test"));
        // QFile::remove(css_file);
        qDebug() << "Test css applied";
    }
}

void GDeskStyler::on_actionExit_triggered()
{
    close();
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

        modified = false;
    }
}

void GDeskStyler::on_actionSave_As_triggered()
{
#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../Resources/userstyles");
#else
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "userstyles");
#endif

    QString name = QInputDialog::getText(this, "Style Name", "Type the name of your new style sheet");

    QString new_filename = dir.absolutePath() + QDir::separator() + name + ".scss";

    QFile file(new_filename);
    if (file.exists())
    {
        QMessageBox::StandardButton btn = QMessageBox::warning(this, "Override Style",
                                                               QString("The style sheet '%1' already exists!\nDo you want to continue ?").arg(name),
                                                               QMessageBox::Yes|QMessageBox::No);

        if (btn != QMessageBox::Yes)
            return;
    }

    this->css = name;
    this->filename = new_filename;

    setWindowTitle(QString("GDeskStyler - ") + name);

    save(filename);

    modified = false;

    test();
}

void GDeskStyler::save(QString filename)
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    int this_count = settings.value("gdeskstyler.current", 0).toInt();
    settings.setValue("gdeskstyler.current", this_count + 1);

    QFileInfo info(filename);
    qDebug() << filename << info.isWritable() << info.exists();
    if (info.isWritable() || !info.exists())
    {
        QFile gss_file(filename);
        gss_file.open(QIODevice::WriteOnly);
        QTextStream os(&gss_file);

        for(int e=0; e<elements.size(); ++e)
        {
            Element *elt = elements.at(e);
            if (elt->line != -1)
            {
                style.replace(elt->line, QString("$") + elt->name + ":" + elt->value.toString() + ";" + elt->comment);
            }
        }

        for(int l=0; l<style.size(); ++l)
        {
            QString line = style.at(l);
            os << line << "\n";
        }
        gss_file.close();
    }
    else
    {
        QMessageBox::information(this, "Could not save file.", QString("Can not save %1 style sheet.\n Use Save As to save your changes").arg(this->css), QMessageBox::Ok);
    }
}

void GDeskStyler::load(QString filename)
{
    qDebug() << "GDeskStyler::load(" << filename << ")";
    this->filename = filename;

    ui->property_browser->clear();

    populate();

    modified = false;

    QFileInfo info(filename);

    setWindowTitle(QString("GDeskStyler - ") + info.baseName());
}

void GDeskStyler::setGDeskTunes(QObject *gdesktunes)
{
#ifdef Q_OS_MAC
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "../Resources/userstyles");
#else
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "userstyles");
#endif

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

        QString filename = dir.absolutePath() + QDir::separator() + css + ".scss";

        QFile gss_file(filename);

        if (gss_file.exists())
        {
            load(filename);
        }
    }
}

void GDeskStyler::closeEvent(QCloseEvent *evt)
{
    qDebug() << "GDeskStyler::closeEvent()";
    if (modified)
    {
        QMessageBox::Button btn = QMessageBox::warning(this, "Confirm exit",
                                                       "You have modified the style sheet.\n Do you want to exit without saving?",
                                                       QMessageBox::Yes | QMessageBox::No);
        if (btn == QMessageBox::Yes)
        {
            if (this->gdesktunes != 0)
            {
                // QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, ""));
                QMetaObject::invokeMethod(this->gdesktunes, "setCSS", Q_ARG(QString, this->css));
            }
            evt->accept();
        }
        else
            evt->ignore();
    }
    else
        evt->accept();
}

void GDeskStyler::on_actionReload_triggered()
{
    load(this->filename);
    test();
}
