#ifndef COMMONHEADER_H
#define COMMONHEADER_H

#include <QUndoStack>
#include <ui/mainwindow.h>
#include <fragmentscontroller.h>

class CommonHeader
{
public:
    static QUndoStack *undoStack;
    static FragmentsController* controller;
    static QMainWindow* mainWindow;
};

#endif // COMMONHEADER_H
