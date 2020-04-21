#ifndef FRAGMENTAREA_H
#define FRAGMENTAREA_H

#include <QWidget>
#include <ui/fragmentui.h>
#include "eventgraphicsscene.h"
#include <QHBoxLayout>
#include <QGraphicsView>
#include <fragmentscontroller.h>
#include <QUndoStack>

namespace Ui
{
    class FragmentArea;
}

class FragmentsController;

class FragmentArea : public QWidget
{
    Q_OBJECT

public:
    explicit FragmentArea(QWidget *parent = nullptr);
    ~FragmentArea();
    virtual void update();


private slots:
    void fragmentsMoveEvents(QGraphicsSceneMouseEvent *event, QPoint biasPos);

    void on_btnJoint_clicked();

    void on_btnSplit_clicked();

    void on_autoStitch_clicked();

    void on_unSelect_clicked();

    void on_btnAutoJoint_clicked();

private:
    Ui::FragmentArea *ui;
    EventGraphicsScene *scene;
    std::vector<FragmentUi *> fragmentItems;
    FragmentsController *fragCtrl;
    QUndoStack *undoStack;
};

#endif // FRAGMENTAREA_H
