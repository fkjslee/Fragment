#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWidget>
#include "eventgraphicsscene.h"
#include <fragmentscontroller.h>

namespace Ui
{
    class Desktop;
}

class Desktop : public QWidget
{
    Q_OBJECT

public:
    explicit Desktop(QWidget *parent = nullptr);
    void dropEvent(QDropEvent *event) override;
    EventGraphicsScene *getScene() const
    {
        return scene;
    }
    void update();
    ~Desktop() override;

private slots:
    void on_btnSelect_clicked();

private:
    Ui::Desktop *ui;
    EventGraphicsScene *scene;
    FragmentsController *fragCtrl;
    std::vector<FragmentUi *> fragmentItems;
};

#endif // DESKTOP_H
