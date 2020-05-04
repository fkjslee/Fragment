#ifndef STORESCENE_H
#define STORESCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>


class storeScene: public QGraphicsScene
{
public:
    storeScene();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
};

#endif // STORESCENE_H
