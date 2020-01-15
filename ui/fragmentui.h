#ifndef FRAGMENTUI_H
#define FRAGMENTUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class Piece
{
public:
    Piece(const QString &piecePath, const QString &pieceName = "noname")
        : piecePath(piecePath), pieceName(pieceName) {}

public:
    QString piecePath;
    QString pieceName;
};

class FragmentUi : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    FragmentUi(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName = "unname");
    static FragmentUi *getDraggingItem()
    {
        return draggingItem;
    }
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void update(const QRectF &rect = QRectF());
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
    const std::vector<Piece> &getPiece() const
    {
        return pieces;
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;

public:
    static FragmentUi *draggingItem;

    std::vector<Piece> pieces;
    const QImage originalImage;
    QImage showImage;
    QString fragmentName;
    QPointF biasPos;
    bool selected = false;
    double scale = 1.0;
};

#endif // FRAGMENTUI_H
