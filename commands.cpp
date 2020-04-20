#ifndef COMMANDS_H
#define COMMANDS_H

#include <commands.h>
#include <Tool.h>
#include <CommonHeader.h>

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

JointUndo::JointUndo(std::vector<FragmentUi *> undoFragments, FragmentUi *redoFragment, QUndoCommand *parent) :
    QUndoCommand(parent), undoFragments(undoFragments), redoFragments(redoFragment)
{
    fragCtller = FragmentsController::getController();
}

void JointUndo::undo()
{
    Tool::eraseInVector(fragCtller->getUnsortedFragments(), redoFragments);
    for (FragmentUi* fragment : undoFragments)
        fragCtller->getUnsortedFragments().push_back(fragment);
    QUndoCommand::undo();
    MainWindow::mainWindow->update();
}

void JointUndo::redo()
{
    fragCtller->getUnsortedFragments().push_back(redoFragments);
    for (FragmentUi* fragment : undoFragments)
        Tool::eraseInVector(fragCtller->getUnsortedFragments(), fragment);
    MainWindow::mainWindow->update();
    QUndoCommand::redo();
}


#endif

SplitUndo::SplitUndo(std::vector<FragmentUi *> undoFragments, std::vector<FragmentUi*> redoFragments, QUndoCommand *parent) :
    QUndoCommand(parent), undoFragments(undoFragments), redoFragments(redoFragments)
{
    fragCtller = FragmentsController::getController();
}

void SplitUndo::undo()
{
    for (FragmentUi* fragment : redoFragments)
        Tool::eraseInVector(fragCtller->getUnsortedFragments(), fragment);
    for (FragmentUi* fragment : undoFragments)
        fragCtller->getUnsortedFragments().push_back(fragment);
    MainWindow::mainWindow->update();
}

void SplitUndo::redo()
{
    for (FragmentUi* fragment : undoFragments)
        Tool::eraseInVector(fragCtller->getUnsortedFragments(), fragment);
    for (FragmentUi* fragment : redoFragments)
        fragCtller->getUnsortedFragments().push_back(fragment);
    MainWindow::mainWindow->update();
}
