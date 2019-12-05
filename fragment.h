#ifndef FRAGMENT_H
#define FRAGMENT_H

#include "QtWidgets"
#include <set>
#include <QGraphicsSceneMouseEvent>
#include "coloritem.h"

class Fragment : public QObject
{
    Q_OBJECT
public:
    Fragment(QString fragmentName = "", int xpos = 0, int ypos = 0);
    Fragment(const Fragment& rhs);
    ~Fragment();
    ColorItem* getItemShape();
    static void createFragments();
    static bool sortFragment(Fragment* frag);
    static bool unsortFragment(Fragment* frag);
    static std::set<Fragment*> getSortedFragments();
    static std::set<Fragment*> getUnsortedFragments();
    static Fragment* getDraggingItem() { return draggingItem; }

public slots:
    void receiveColorItemDragging(QGraphicsSceneMouseEvent* event);

private:
    static std::set<Fragment*> sortedFragments;
    static std::set<Fragment*> unsortedFragments;
    static Fragment* draggingItem;
    QLabel* shape;
    QPoint position;
    ColorItem* itemShape;
};

#endif // FRAGMENT_H
