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
#include "fragment.h"
#include "QImage"

class ColorItem :  public QGraphicsObject
{
    Q_OBJECT
public:
    ColorItem(Fragment* fragment, const QString& picPath = "");

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    static ColorItem* getDraggingItem() { return draggingItem; }
    QColor getColor() { return color; }
    Fragment* getFragment() const { return fragment; }

signals:
    void doubleClickItem(ColorItem* item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void dropEvent(QDropEvent  * e);
    void dragEnterEvent(QDragEnterEvent  * e);

private:
    QColor color;
    Fragment* fragment;
    static ColorItem* draggingItem;
    QString picPath;
};

#endif // COLORITEM_H
