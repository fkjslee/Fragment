#ifndef COLORITEM_H
#define COLORITEM_H

#include <QMainWindow>
#include <QGraphicsObject>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QApplication>
#include <QDrag>
#include <QPixmap>
#include <QBitmap>
#include <QGraphicsObject>
#include <QDropEvent>
#include "QImage"
#include <set>

class Fragment;

enum JointMethod {leftRight, rightLeft, upDown, downUp};

struct JointFragment {
    Fragment* item;
    JointMethod method;
    double absGrayscale;
    JointFragment(Fragment* item, JointMethod method, double absGrayscale)
        : item(item), method(method), absGrayscale(absGrayscale) {}
};

class Fragment :  public QGraphicsObject
{
    Q_OBJECT
public:
    Fragment(const QImage& image, const QString& fragmentName = "unname");

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    static Fragment* getDraggingItem() { return draggingItem; }
    QColor getColor() { return color; }
    Fragment* getFragment() const { return fragment; }
    const QImage& getImage() const { return image; }
    const QString& getFragmentName() const { return fragmentName; }

    static std::vector<JointFragment> getMostPossibleColorItems(Fragment* item = nullptr);
    static void createFragments();
    static std::set<Fragment*> getSortedFragments();
    static std::set<Fragment*> getUnsortedFragments();
    static bool sortFragment(Fragment* frag);
    static bool unsortFragment(Fragment* frag);
    static bool jointFragment(Fragment* f1, JointFragment jointFragment);

signals:
    void doubleClickItem(Fragment* item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent  * e);

private:
    static std::set<Fragment*> sortedFragments;
    static std::set<Fragment*> unsortedFragments;
    QColor color;
    Fragment* fragment;
    static Fragment* draggingItem;
    QImage image;
    QString picPath;
    QString fragmentName;
};

#endif // COLORITEM_H
