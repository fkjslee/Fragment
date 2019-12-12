#ifndef FRAGMENT_H
#define FRAGMENT_H

#include <QMainWindow>
#include <QGraphicsObject>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QApplication>
#include <QDrag>
#include <QPixmap>
#include <QBitmap>
#include <QGraphicsObject>
#include <QDropEvent>
#include <QImage>
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

    Fragment* getFragment() const { return fragment; }
    const QImage& getImage() const { return image; }
    const QString& getFragmentName() const { return fragmentName; }
    const QPointF& getBiasPos() const { return biasPos; }

    static Fragment* getDraggingItem() { return draggingItem; }
    static std::vector<JointFragment> getMostPossibleFragments(Fragment* item = nullptr);
    static void createFragments(const QString& fragmentsPath);
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
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    static std::set<Fragment*> sortedFragments;
    static std::set<Fragment*> unsortedFragments;
    Fragment* fragment;
    static Fragment* draggingItem;
    QImage image;
    QString fragmentName;
    QPointF biasPos;
};

#endif // FRAGMENT_H
