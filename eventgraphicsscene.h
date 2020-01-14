#ifndef EVENTGRAPHICSSCENE_H
#define EVENTGRAPHICSSCENE_H

#include <QGraphicsScene>
#include "fragment.h"

class EventGraphicsScene: public QGraphicsScene
{
    Q_OBJECT
public:
    enum SceneType {none = 0x00, desktop = 0x01, fragmentArea = 0x02, hintArea = 0x04};
    EventGraphicsScene(SceneType sceneType, QObject *parent = nullptr): QGraphicsScene(parent), sceneType(sceneType) {}

signals:
    void removeFragment(Fragment *fragment);

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    void invalidOperation(QGraphicsSceneDragDropEvent *event);
    void moveFragmentFromHintToDesktop(QGraphicsSceneDragDropEvent *event);
    void moveBetweenTwoNormalSceen(QGraphicsSceneDragDropEvent *event);

private:
    SceneType sceneType;
};

#endif // EVENTGRAPHICSSCENE_H
