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
#include <ui/fragmentui.h>
#include <entity/fragmententity.h>

class Fragment : public FragmentUi, public FragmentEntity
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    Fragment(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName = "unname");

signals:
    void fragmentsMoveEvents(QGraphicsSceneMouseEvent *event, QPoint biasPos);

public slots:

private:
};

#endif // FRAGMENT_H
