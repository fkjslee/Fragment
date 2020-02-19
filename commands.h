#pragma once

#include <QUndoCommand>
#include <QGraphicsItem>
#include <QPoint>
#include <fragmentscontroller.h>
#include <ui/fragmentui.h>
#include <ui/fragmentarea.h>

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

class JointUndo : public QUndoCommand
{
public:
    JointUndo(std::vector<FragmentUi*> undoFragments, FragmentUi* redoFragments, FragmentArea* fragmentArea, QUndoCommand *parent = nullptr);

    void undo();
    void redo();

private:
    std::vector<FragmentUi*> undoFragments;
    FragmentUi* redoFragments;
    FragmentsController* fragCtller;
    FragmentArea* fragmentArea;
};

class SplitUndo : public QUndoCommand
{
public:
    SplitUndo(std::vector<FragmentUi*> undoFragments, std::vector<FragmentUi*> redoFragments, FragmentArea* fragmentArea, QUndoCommand *parent = nullptr);

    void undo();
    void redo();

private:
    std::vector<FragmentUi*> undoFragments;
    std::vector<FragmentUi*> redoFragments;
    FragmentArea* fragmentArea;
    FragmentsController* fragCtller;
};
