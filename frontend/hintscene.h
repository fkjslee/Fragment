#ifndef HINTSCENE_H
#define HINTSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>

class HintScene: public QGraphicsScene
{
public:
    HintScene();

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private slots:
    void btnAutoJointClicked();
};

#endif // HINTSCENE_H
