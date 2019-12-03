#ifndef FRAGMENT_H
#define FRAGMENT_H

#include "QtWidgets"

class Fragment
{
public:
    Fragment(QString fragmentName = "", int xpos = 0, int ypos = 0);
    ~Fragment();
    static std::vector<Fragment>* fragments_sorted;
    static std::vector<Fragment>* fragments_unsorted;

private:
    QLabel* shape;
    QPoint position;
};

#endif // FRAGMENT_H
