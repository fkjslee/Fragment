#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWidget>
#include "eventgraphicsscene.h"

namespace Ui {
class Desktop;
}

class Desktop : public QWidget
{
    Q_OBJECT

public:
    explicit Desktop(QWidget *parent = nullptr);
    void dropEvent(QDropEvent *event) override;
    EventGraphicsScene* getScene() const { return scene; }
    ~Desktop() override;

private:
    Ui::Desktop *ui;
    EventGraphicsScene* scene;
};

#endif // DESKTOP_H
