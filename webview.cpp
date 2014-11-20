#include "webview.h"
#include <QMouseEvent>
#include <QWebFrame>
#include <QWebElement>
#include "mainwindow.h"

WebView::WebView(QWidget *parent) :
    QWebView(parent),
    move(false),
    main_window( (MainWindow*)parent->parent())
{
    qDebug() << parent->parent();
}

void WebView::mousePressEvent(QMouseEvent *event)
{
    if (main_window->isMini())
    {
        bool process = true;

        QWebPage *page = this->page();
        QWebFrame *frame = page->mainFrame();
        QWebHitTestResult result = frame->hitTestContent(event->pos());


        QString name = result.element().localName();
        QString id = result.element().attribute("id", "");
        QString clazz = result.element().attribute("class", "");

        // FIXME: faster access
        if (name == "button") process = false;
        if (name == "li") process = false;

        if (process)
        {
            if (id.contains("volume")) process = false;
            if (id.contains("slider")) process = false;
            if (id.contains("hover")) process = false;
        }

        if (process)
        {
            if (clazz.contains("volume")) process = false;
            if (clazz.contains("slider")) process = false;
        }

        qDebug() << id << " " << name << " " << clazz << "  " << process << " " << result.linkUrl();

        if (process)
        {
            qDebug() << "MousePress" << event;
            mouse_click_x_coordinate = event->x();
            mouse_click_y_coordinate = event->y();
            move = true;
            event->accept();
            return;
        }
    }
    QWebView::mousePressEvent(event);
}

void WebView::mouseMoveEvent(QMouseEvent *event)
{
    if (main_window->isMini())
    {
        if (move)
        {
            main_window->move(event->globalX() - mouse_click_x_coordinate, event->globalY() - mouse_click_y_coordinate);
            event->accept();
            return;
        }
    }
    QWebView::mouseMoveEvent(event);
}

void WebView::mouseReleaseEvent(QMouseEvent *event)
{
    move = false;
    QWebView::mouseReleaseEvent(event);
}
