#ifndef FRAGMENTAREA_H
#define FRAGMENTAREA_H

#include <QWidget>

namespace Ui {
class FragmentArea;
}

class FragmentArea : public QWidget
{
    Q_OBJECT

public:
    explicit FragmentArea(QWidget *parent = nullptr);
    ~FragmentArea();

private:
    Ui::FragmentArea *ui;
};

#endif // FRAGMENTAREA_H
