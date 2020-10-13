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
    Piece(const QString &piecePath, const int &pieceId, const cv::Mat &transMat = cv::Mat::eye(3, 3, CV_32FC1))
        : piecePath(piecePath), pieceID(pieceId)
    {
        this->transMat = transMat.clone();
    }
public:
    QString piecePath;
    int pieceID;
    cv::Mat transMat;
};

struct SuggAng
{
    int ang;
    int cnt;
    bool operator < (const SuggAng &rhs) const
    {
        return cnt > rhs.cnt;
    }
};

class FragmentUi : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

signals:
    void refreshHintWindow();

public:
    FragmentUi(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName = "unname");
    const QImage &getOriginalImage() const
    {
        return originalImage;
    }
    const QString &getFragmentName() const
    {
        return fragmentName;
    }
    const std::vector<Piece> &getPieces() const
    {
        return pieces;
    }
    void setPiece(const std::vector<Piece> &pieces)
    {
        this->pieces = pieces;
    }


public:
    std::vector<const FragmentUi *> undoFragments;

protected:
    const QImage originalImage;

private:
    std::vector<Piece> pieces;
    QImage showImage;
    QString fragmentName;
};

#endif // FRAGMENTUI_H
