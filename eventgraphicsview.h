#ifndef EVENTGRAPHICSVIEW_H
#define EVENTGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWidget>
#include <QDropEvent>

class EventGraphicsView : public QGraphicsView
{
public:
    EventGraphicsView(QWidget* parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // EVENTGRAPHICSVIEW_H
