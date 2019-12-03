#include "fragment.h"

Fragment::Fragment(QString fragmentName, int xpos, int ypos)
{
    this->shape = new QLabel(fragmentName);
    this->position = QPoint(xpos, ypos);
}

Fragment::~Fragment()
{
    delete shape;
}
