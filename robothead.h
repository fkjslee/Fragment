#ifndef ROBOTHEAD_H
#define ROBOTHEAD_H

#include <QMainWindow>
#include <QGraphicsObject>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QPainter>
#include <robotpart.h>

class RobotHead : public RobotPart
{
public:
    RobotHead(QGraphicsItem *parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

private:
    QPixmap pixmap;
};

#endif // ROBOTHEAD_H
