#ifndef FRAGMENTUI_H
#define FRAGMENTUI_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <opencv2/opencv.hpp>
#include <fragmentsscene.h>
#include <QMutex>

class Piece
{
public:
    Piece(const QString &piecePath, const QString &pieceName, const cv::Mat &transMat = cv::Mat::eye(3, 3, CV_32FC1))
        : piecePath(piecePath), pieceID(pieceName) {
        this->transMat = transMat.clone();
    }
public:
    QString piecePath;
    QString pieceID;
    cv::Mat transMat;
};

enum Platfrom {WorkArea, HintArea};

class FragmentUi : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

signals:
    void refreshHintWindow();

public:
    FragmentUi(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName = "unname", Platfrom platform=Platfrom::WorkArea);
    const QImage &getOriginalImage() const
    {
        return originalImage;
    }
    const QString &getFragmentName() const
    {
        return fragmentName;
    }
    const cv::Mat getOffsetMat() const {
        return offset.clone();
    }
    void scaledToWidth(const double scale);
    const std::vector<Piece> &getPieces() const
    {
        return pieces;
    }
    void setPiece(const std::vector<Piece>& pieces) {
        this->pieces = pieces;
    }
    void rotate(int ang);
    void update(const QRectF &rect = QRectF());
    void startToCalc() {
        calcing = true;
        locker.lock();
        calcCnt++;
        locker.unlock();
    }
    void endToCalc() {
        calcing = false;
        locker.lock();
        calcCnt--;
        locker.unlock();
    }
    bool getCacl() {
        return calcing;
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

public:
    std::vector<const FragmentUi*> undoFragments;
    int rotateAng = 0;
    Platfrom platform;

private:
    static FragmentUi *draggingItem;

    std::vector<Piece> pieces;
    const QImage originalImage;
    QImage showImage;
    QString fragmentName;
    QPoint undoPos;
    QPointF pressPos;
    cv::Mat offset;
    double scale = 1.0;
    bool calcing = false;
    static int calcCnt;
    QMutex locker;
};

#endif // FRAGMENTUI_H
