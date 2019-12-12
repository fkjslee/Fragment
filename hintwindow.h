#ifndef HINTWINDOW_H
#define HINTWINDOW_H

#include <QWidget>
#include "eventgraphicsscene.h"

namespace Ui {
class HintWindow;
}

class HintWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HintWindow(QWidget *parent = nullptr);
    ~HintWindow();

private slots:
    void on_refreshBtn_clicked();

private:
    Ui::HintWindow *ui;
    EventGraphicsScene* scene;
    std::vector<Fragment*> fragments;
};

#endif // HINTWINDOW_H
