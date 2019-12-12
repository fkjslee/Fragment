#ifndef FRAGMENTAREA_H
#define FRAGMENTAREA_H

#include <QWidget>
#include "coloritem.h"
#include "eventgraphicsscene.h"

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

private:
    Ui::FragmentArea *ui;
    EventGraphicsScene* scene;
    std::vector<Fragment*> fragmentItems;
};

#endif // FRAGMENTAREA_H
