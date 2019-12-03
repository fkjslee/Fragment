#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWidget>

namespace Ui {
class Desktop;
}

class Desktop : public QWidget
{
    Q_OBJECT

public:
    explicit Desktop(QWidget *parent = nullptr);
    ~Desktop();

private:
    Ui::Desktop *ui;
};

#endif // DESKTOP_H
