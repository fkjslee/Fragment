#include "fragment.h"
#include <iostream>

Fragment::Fragment(QString fragmentName, int xpos, int ypos)
{
    this->shape = new QLabel(fragmentName);
    this->position = QPoint(xpos, ypos);
}

Fragment::Fragment(const Fragment &rhs)
{
    this->shape = new QLabel(rhs.shape->text());
}

Fragment::~Fragment()
{
    delete shape;
}

std::set<Fragment*> Fragment::fragments_unsorted = std::set<Fragment*>();
std::set<Fragment*> Fragment::fragments_sorted = std::set<Fragment*>();
void Fragment::createFragments()
{
    for(int i = 0; i < 9; ++i) {
        fragments_unsorted.insert(new Fragment(QString("2")));
    }
}

bool Fragment::sortFragment(Fragment *frag)
{
    if(fragments_unsorted.find(frag) == fragments_unsorted.end())
        return false;
    fragments_unsorted.erase(frag);
    fragments_sorted.insert(frag);
    return true;
}

bool Fragment::unsortFragment(Fragment *frag)
{
    if(fragments_sorted.find(frag) == fragments_sorted.end())
        return false;
    fragments_sorted.erase(frag);
    fragments_unsorted.insert(frag);
    return true;
}

