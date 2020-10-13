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

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

signals:
    void mousePressFragment(const HintFragment *fragment);
};

#endif // HINTFRAGMENT_H
