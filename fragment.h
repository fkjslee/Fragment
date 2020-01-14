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

class Piece
{
public:
    Piece(const QString &piecePath, const QString &pieceName = "noname")
        : piecePath(piecePath), pieceName(pieceName) {}

public:
    QString piecePath;
    QString pieceName;
};

class Fragment : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    Fragment(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName = "unname");

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    Fragment *getFragment() const
    {
        return fragment;
    }
    const QImage &getOriginalImage() const
    {
        return originalImage;
    }
    const QImage &getShowImage() const
    {
        return showImage;
    }
    const QString &getFragmentName() const
    {
        return fragmentName;
    }
    const QPointF &getBiasPos() const
    {
        return biasPos;
    }
    bool getSelected() const
    {
        return selected;
    }
    void scaledToWidth(const double scale);
    void reverseSelectState();
    void update(const QRectF &rect = QRectF());
    const std::vector<Piece> &getPiece() const
    {
        return pieces;
    }

    static Fragment *getDraggingItem()
    {
        return draggingItem;
    }

signals:
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
    static Fragment *draggingItem;

    std::vector<Piece> pieces;
    Fragment *fragment;
    const QImage originalImage;
    QImage showImage;
    QString fragmentName;
    QPointF biasPos;
    bool selected = false;
    double scale = 1.0;
};

#endif // FRAGMENT_H
