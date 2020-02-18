#include <commands.h>

MoveUndo::MoveUndo(QGraphicsItem *item, QPoint undoPos, QPoint redoPos, QUndoCommand *parent) :
    QUndoCommand(parent), item(item), undoPos(undoPos), redoPos(redoPos)
{
}

MoveUndo::~MoveUndo()
{

}

void MoveUndo::undo()
{
    item->setPos(undoPos);
}

void MoveUndo::redo()
{
    item->setPos(redoPos);
}
