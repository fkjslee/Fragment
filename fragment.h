#ifndef FRAGMENT_H
#define FRAGMENT_H

#include "QtWidgets"
#include <set>

class Fragment
{
public:
    Fragment(QString fragmentName = "", int xpos = 0, int ypos = 0);
    Fragment(const Fragment& rhs);
    ~Fragment();
    static void createFragments();
    static bool sortFragment(Fragment* frag);
    static bool unsortFragment(Fragment* frag);

private:
    static std::set<Fragment*> fragments_sorted;
    static std::set<Fragment*> fragments_unsorted;
    QLabel* shape;
    QPoint position;
};

#endif // FRAGMENT_H
