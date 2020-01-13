#ifndef FRAGMENTAREA_H
#define FRAGMENTAREA_H

#include <QWidget>
#include "fragment.h"
#include "eventgraphicsscene.h"
#include <QHBoxLayout>
#include <QGraphicsView>

namespace Ui {
class FragmentArea;
}

class FragmentArea : public QWidget
{
    Q_OBJECT

public:
    explicit FragmentArea(QWidget *parent = nullptr);
    ~FragmentArea();
    void update();

private slots:
    void on_autoStitch_clicked();
    void sortItem(Fragment* item);
    void fragmentsMoveEvents(QGraphicsSceneMouseEvent *event, QPoint biasPos);

    void on_btnJoint_clicked();

    void on_btnSplit_clicked();

private:
    Ui::FragmentArea *ui;
    EventGraphicsScene* scene;
    std::vector<Fragment*> fragmentItems;
};

#endif // FRAGMENTAREA_H
