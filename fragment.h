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
    Fragment(const QImage& showImage, const QString& fragmentName = "unname");

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    Fragment* getFragment() const { return fragment; }
    const QImage& getOriginalImage() const { return originalImage; }
    const QImage& getShowImage() const { return showImage; }
    const QString& getFragmentName() const { return fragmentName; }
    const QPointF& getBiasPos() const { return biasPos; }
    void scaledToWidth(int width);

    static Fragment* getDraggingItem() { return draggingItem; }
    static std::vector<JointFragment> getMostPossibleFragments(Fragment* item = nullptr);
    static void createAllFragments(const QString& fragmentsPath);
    static std::vector<Fragment*> getSortedFragments();
    static std::vector<Fragment*> getUnsortedFragments();
    static bool sortFragment(Fragment* frag);
    static bool unsortFragment(Fragment* frag);
    static bool jointFragment(Fragment* f1, JointFragment jointFragment);

signals:
    void doubleClickItem(Fragment* item);

public slots:
    void imageSizeChanged(const int value);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;

public:
    static std::vector<Fragment*> sortedFragments;
    static std::vector<Fragment*> unsortedFragments;
    Fragment* fragment;
    static Fragment* draggingItem;
    const QImage originalImage;
    QImage showImage;
    QString fragmentName;
    QPointF biasPos;
};

#endif // FRAGMENT_H
