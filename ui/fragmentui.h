#ifndef FRAGMENTUI_H
#define FRAGMENTUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <opencv2/opencv.hpp>

class Piece
{
public:
    Piece(const QString &piecePath, const QString &pieceName, const cv::Mat &transMat = cv::Mat::eye(3, 3, CV_32FC1))
        : piecePath(piecePath), pieceName(pieceName) {
        this->transMat = transMat.clone();
    }
public:
    QString piecePath;
    QString pieceName;
    cv::Mat transMat;
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
    void scaledToWidth(const double scale);
    const std::vector<Piece> &getPieces() const
    {
        return pieces;
    }
    void rotate(double ang);
    void update(const QRectF &rect = QRectF());

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

public:
    std::vector<FragmentUi *> undoFragments;

private:
    static FragmentUi *draggingItem;

    std::vector<Piece> pieces;
    const QImage originalImage;
    QImage showImage;
    QString fragmentName;
    QPoint undoPos;
    QPointF pressPos;
    double scale = 1.0;
    double rotateAng = 0;
};

#endif // FRAGMENTUI_H
