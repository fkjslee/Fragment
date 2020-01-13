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

class Piece {
public:
    Piece(const QString& piecePath, const QString& pieceName = "noname")
        : piecePath(piecePath), pieceName(pieceName) {}

public:
    QString piecePath;
    QString pieceName;
};

struct JointFragment {
    Fragment* item;
    JointMethod method;
    double absGrayscale;
    JointFragment(Fragment* item, JointMethod method, double absGrayscale)
        : item(item), method(method), absGrayscale(absGrayscale) {}
};

class Fragment : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    Fragment(const std::vector<Piece>& pieces, const QImage& originalImage, const QString& fragmentName = "unname");

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    Fragment* getFragment() const { return fragment; }
    const QImage& getOriginalImage() const { return originalImage; }
    const QImage& getShowImage() const { return showImage; }
    const QString& getFragmentName() const { return fragmentName; }
    const QPointF& getBiasPos() const { return biasPos; }
    void scaledToWidth(const double scale);
    void reverseSelectState();
    void update(const QRectF &rect = QRectF());
    const std::vector<Piece>& getPiece() const { return pieces; }

    // static methods
    static Fragment* getDraggingItem() { return draggingItem; }
    static std::vector<JointFragment> getMostPossibleFragments(Fragment* item = nullptr);
    static void createAllFragments(const QString& fragmentsPath);
    static std::vector<Fragment*> getSortedFragments();
    static std::vector<Fragment*> getUnsortedFragments();
    static bool sortFragment(Fragment* frag);
    static bool unsortFragment(Fragment* frag);
    static bool jointFragment(Fragment* f1, JointFragment jointFragment);
    static bool splitSelectedFragments();
    static void reverseChosenFragment(Fragment* f);
    static const std::vector<Fragment*> getSelectedFragments();
    static JointFragment mostPossibleJointMethod(Fragment* f1, Fragment* f2);

signals:
    void doubleClickItem(Fragment* item);
    void fragmentsMoveEvents(QGraphicsSceneMouseEvent *event, QPoint biasPos);

public slots:
    void imageSizeChanged(const int value);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    static std::vector<Fragment*> sortedFragments;
    static std::vector<Fragment*> unsortedFragments;
    static std::vector<Fragment*> chosenFragments;
    static Fragment* draggingItem;

    std::vector<Piece> pieces;
    Fragment* fragment;
    const QImage originalImage;
    QImage showImage;
    QString fragmentName;
    QPointF biasPos;
    bool selected = false;
    double scale = 1.0;
};

#endif // FRAGMENT_H
