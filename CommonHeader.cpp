#include <CommonHeader.h>

QUndoStack* CommonHeader::undoStack = new QUndoStack();
FragmentsController* CommonHeader::controller = FragmentsController::getController();
