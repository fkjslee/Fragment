#ifndef EVENTGRAPHICSSCENE_H
#define EVENTGRAPHICSSCENE_H

#include <QGraphicsScene>
#include <iostream>

class EventGraphicsScene: public QGraphicsScene {
public:
    enum SceneType {desktop, fragmentArea, hintArea};
    EventGraphicsScene(QObject* parent = nullptr): QGraphicsScene(parent) {}
    EventGraphicsScene(SceneType sceneType, QObject* parent = nullptr): QGraphicsScene(parent), sceneType(sceneType) {}

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    SceneType sceneType;
};

#endif // EVENTGRAPHICSSCENE_H
