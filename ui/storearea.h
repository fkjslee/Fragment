#ifndef STOREAREA_H
#define STOREAREA_H

#include <QWidget>
#include <fragmentscontroller.h>
#include <storescene.h>

namespace Ui
{
    class StoreArea;
}

class StoreArea : public QWidget
{
    Q_OBJECT

public:
    explicit StoreArea(QWidget *parent = nullptr);
    void dropEvent(QDropEvent *event) override;
    QGraphicsScene *getScene() const
    {
        return scene;
    }
    void update();
    ~StoreArea() override;

private:
    Ui::StoreArea *ui;
    storeScene *scene;
    FragmentsController *fragCtrl;
    std::vector<FragmentUi *> fragmentItems;
};

#endif // STOREAREA_H
