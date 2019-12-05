#ifndef COLORITEM_H
#define COLORITEM_H

#include <QMainWindow>
#include <QGraphicsObject>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QPainter>
#include <robotpart.h>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QApplication>
#include <QDrag>
#include <QPixmap>
#include <QBitmap>
#include <QGraphicsObject>
#include <QDropEvent>

class ColorItem :  public QGraphicsObject
{
    Q_OBJECT
public:
    ColorItem();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QColor getColor() { return color; }

signals:
    void sendColorItemDragging(QGraphicsSceneMouseEvent* event);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void dropEvent(QDropEvent  * e);
    void dragEnterEvent(QDragEnterEvent  * e);

private:
    QColor color;
};

#endif // COLORITEM_H
