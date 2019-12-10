#include "fragment.h"
#include <iostream>

std::set<Fragment*> Fragment::unsortedFragments = std::set<Fragment*>();
std::set<Fragment*> Fragment::sortedFragments = std::set<Fragment*>();
Fragment* Fragment::draggingItem = nullptr;
Fragment::Fragment(QString fragmentName, int xpos, int ypos) : QObject()
{
    property = QColor(QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256), QRandomGenerator::global()->bounded(256));
    this->shape = new QLabel(fragmentName);
    this->position = QPoint(xpos, ypos);
    //connect(itemShape, &ColorItem::sendColorItemDragging, this, &Fragment::receiveColorItemDragging);
}

Fragment::Fragment(const Fragment &rhs)
{
    this->shape = new QLabel(rhs.shape->text());
}

Fragment::~Fragment()
{
    delete shape;
}

void Fragment::createFragments()
{
    qDebug() << "here lihai";
    for(int i = 0; i < 10; ++i) {
        sortedFragments.insert(new Fragment(QString("2")));
    }
    for(int i = 0; i < 10; ++i) {
        unsortedFragments.insert(new Fragment(QString("2")));
    }
}

bool Fragment::sortFragment(Fragment *frag)
{
    if(unsortedFragments.find(frag) == unsortedFragments.end())
        return false;
    unsortedFragments.erase(frag);
    sortedFragments.insert(frag);
    return true;
}

bool Fragment::unsortFragment(Fragment *frag)
{
    if(sortedFragments.find(frag) == sortedFragments.end())
        return false;
    sortedFragments.erase(frag);
    unsortedFragments.insert(frag);
    return true;
}

std::set<Fragment *> Fragment::getSortedFragments()
{
    return sortedFragments;
}

std::set<Fragment *> Fragment::getUnsortedFragments()
{
    return unsortedFragments;
}

std::vector<Fragment *> Fragment::getMostPossibleFragments(Fragment *f)
{
    std::vector<Fragment*> res;
    std::set<Fragment*> unsorted_fragments = getUnsortedFragments();
    for (Fragment* unsorted_fragment : unsorted_fragments) {
        res.emplace_back(unsorted_fragment);
        if (res.size() >= 5)
            break;
    }
    return res;
}

void Fragment::receiveColorItemDragging(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "receiveColorItemDragging";
    draggingItem = this;
}

