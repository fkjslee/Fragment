#ifndef HINTFRAGMENT_H
#define HINTFRAGMENT_H

#include <QGraphicsPixmapItem>
#include <ui/fragmentui.h>

class HintFragment: public FragmentUi
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    HintFragment(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName = "unname");

    void update(const QRectF &rect = QRectF());
    static void moveRelatedPieceToPos(const Piece *p1, const Piece *p2, cv::Mat transMat);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    QPointF hoverPrePos;
    double hoverPreAng;
};

#endif // HINTFRAGMENT_H
