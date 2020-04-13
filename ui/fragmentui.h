#ifndef FRAGMENTUI_H
#define FRAGMENTUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class Piece
{
public:
    Piece(const QString &piecePath, const QPoint &piecePos = QPoint(0, 0))
        : piecePath(piecePath), piecePos(piecePos)
    {
        pieceName = QString("f%1").arg(fragmentCnt++);
    }

    Piece(const QString &piecePath, const QString &pieceName, const QPoint &piecePos = QPoint(0, 0))
        : piecePath(piecePath), pieceName(pieceName), piecePos(piecePos) {}
public:
    QString piecePath;
    QString pieceName;
    QPoint piecePos;

private:
    static int fragmentCnt;
};

class FragmentUi : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    FragmentUi(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName = "unname");
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
        return isSelected();
    }
    void scaledToWidth(const double scale);
    const std::vector<Piece> &getPiece() const
    {
        return pieces;
    }
    void update(const QRectF &rect = QRectF());

signals:
    void fragmentsMoveEvents(QGraphicsSceneMouseEvent *event, QPoint biasPos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

public:
    std::vector<FragmentUi *> undoFragments;

private:
    static FragmentUi *draggingItem;

    std::vector<Piece> pieces;
    const QImage originalImage;
    QImage showImage;
    QString fragmentName;
    QPointF biasPos;
    QPoint undoPos;
    QPointF pressPos;
    double scale = 1.0;
};

#endif // FRAGMENTUI_H
