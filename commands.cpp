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
    QString log = QString("undo: ") + undoFragments[0]->getFragmentName() + " and " + undoFragments[1]->getFragmentName() + "->" + redoFragments->getFragmentName();
    Tool::eraseInVector(fragCtller->getUnsortedFragments(), redoFragments);
    for (FragmentUi* fragment : undoFragments)
        fragCtller->getUnsortedFragments().push_back(fragment);
    log += QString(" with score: ") + QString::number(FragmentsController::getController()->calcScore());
    qInfo() << log;
    QUndoCommand::undo();
    MainWindow::mainWindow->update();
}

void JointUndo::redo()
{
    QString log = QString("redo: ") + undoFragments[0]->getFragmentName() + " and " + undoFragments[1]->getFragmentName() + " -> " + redoFragments->getFragmentName();
    fragCtller->getUnsortedFragments().push_back(redoFragments);
    for (FragmentUi* fragment : undoFragments)
        Tool::eraseInVector(fragCtller->getUnsortedFragments(), fragment);
    log += QString(" with score: ") + QString::number(FragmentsController::getController()->calcScore());
    qInfo() << log;
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
    QString log = QString("undo: ");
    for (FragmentUi* fragment : redoFragments) {
        log += fragment->getFragmentName() + " and ";
        Tool::eraseInVector(fragCtller->getUnsortedFragments(), fragment);
    }
    log += " -> ";
    for (FragmentUi* fragment : undoFragments) {
        log += fragment->getFragmentName() + " and ";
        fragCtller->getUnsortedFragments().push_back(fragment);
    }
    log += QString(" with score: ") + QString::number(FragmentsController::getController()->calcScore());
    qInfo() << log;
    MainWindow::mainWindow->update();
}

void SplitUndo::redo()
{
    QString log = QString("undo: ");
    for (FragmentUi* fragment : undoFragments) {
        log += fragment->getFragmentName() + " and ";
        Tool::eraseInVector(fragCtller->getUnsortedFragments(), fragment);
    }
    log += " -> ";
    for (FragmentUi* fragment : redoFragments) {
        log += fragment->getFragmentName() + " and ";
        fragCtller->getUnsortedFragments().push_back(fragment);
    }
    log += QString(" with score: ") + QString::number(FragmentsController::getController()->calcScore());
    qInfo() << log;
    MainWindow::mainWindow->update();
}
