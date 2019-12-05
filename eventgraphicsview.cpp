#include <iostream>
#include "eventgraphicsview.h"
#include <QtDebug>

EventGraphicsView::EventGraphicsView(QWidget* parent) : QGraphicsView(parent)
{
    setMouseTracking(true);
    setAcceptDrops(true);
}

void EventGraphicsView::dragEnterEvent(QDragEnterEvent *event)
{
    std::cout << "desktop dragEnterEvent" << std::endl;
    event->acceptProposedAction();
    event->setAccepted(true);
    update();
}

void EventGraphicsView::dropEvent(QDropEvent *event)
{
    std::cout << "desktop dropEvent" << std::endl;
}

