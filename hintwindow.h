#ifndef HINTWINDOW_H
#define HINTWINDOW_H

#include <QWidget>

namespace Ui {
class HintWindow;
}

class HintWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HintWindow(QWidget *parent = nullptr);
    ~HintWindow();

private:
    Ui::HintWindow *ui;
};

#endif // HINTWINDOW_H
