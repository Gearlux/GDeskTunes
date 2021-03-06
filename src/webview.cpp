#include "webview.h"
#include <QWebFrame>
#include <QMouseEvent>
#include <QWebElement>

WebView::WebView(QWidget *parent) :
#ifdef USE_WEBKIT
    QWebView(parent),
#else
    QWebEngineView(parent),
#endif
    draggable(false),
    move(false)
{
}

void WebView::mousePressEvent(QMouseEvent *event)
{
#ifdef USE_WEBKIT
    if (draggable)
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

        QWidget *w = dynamic_cast<QWidget*>(parent());
        if (w == 0)
        {
            offset.setX(0);
            offset.setY(0);
        }
        else
        {
            offset = w->mapToParent(pos());
        }
        qDebug() << offset;
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
#else
    QWebEngineView::mousePressEvent(event);
#endif
}

void WebView::mouseMoveEvent(QMouseEvent *event)
{
#ifdef USE_WEBKIT
    if (draggable)
    {
        if (move)
        {
            QWidget *w = dynamic_cast<QWidget*>(parent()->parent());
            if (w != 0)
                w->move(event->globalX() - mouse_click_x_coordinate - offset.x(), event->globalY() - mouse_click_y_coordinate - offset.y());
            event->accept();
            return;
        }
        else
        {
            event->ignore();
            return;
        }
    }
    QWebView::mouseMoveEvent(event);
#else
    QWebEngineView::mouseMoveEvent(event);
#endif
}

void WebView::mouseReleaseEvent(QMouseEvent *event)
{
#ifdef USE_WEBKIT
    move = false;
    QWebView::mouseReleaseEvent(event);
#else
    QWebEngineView::mouseReleaseEvent(event);
#endif
}

void WebView::setDraggable(bool drag)
{
    this->draggable = drag;
}

