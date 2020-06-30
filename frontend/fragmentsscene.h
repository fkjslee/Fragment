#ifndef FRAGMENTSSCENE_H
#define FRAGMENTSSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

class FragmentsScene: public QGraphicsScene
{
public:
    FragmentsScene();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

};

#endif // FRAGMENTSSCENE_H
