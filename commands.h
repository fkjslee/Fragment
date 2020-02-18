#pragma once

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QPoint>

class MoveUndo : public QUndoCommand
{
public:
    MoveUndo(QGraphicsItem *item, QPoint undoPos, QPoint redoPos, QUndoCommand *parent=nullptr);
    ~MoveUndo();

    void undo();
    void redo();

private:
    QGraphicsItem *item;
    QPoint undoPos;
    QPoint redoPos;
};

//class JointUndo : public QUndoCommand
//{
//public:
//    JointUndo
//};
