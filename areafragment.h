#ifndef AREAFRAGMENT_H
#define AREAFRAGMENT_H

#include <ui/fragmentui.h>

class AreaFragment: public FragmentUi
{
public:
    AreaFragment(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName = "unname");

    void startToCalc()
    {
        calcing = true;
    }
    void endToCalc()
    {
        calcing = false;
    }
    bool getCacl()
    {
        return calcing;
    }
    const cv::Mat getOffsetMat() const
    {
        return offset.clone();
    }
    void rotate(double ang);
    void update(const QRectF &rect = QRectF());
    void scaledToWidth(const double scale);
    double rotateAng = 0;

public slots:
    void updateFragment()
    {
        update();
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
private:

    QPoint undoPos;
    QPointF pressPos;
    void initSuggestAng();
    std::vector<SuggAng> suggAngs;
    bool calcing = false;
    double scale = 1.0;
    cv::Mat offset;
    double suggAng = -1.0;
    double suggX;
    double suggY;
};

#endif // AREAFRAGMENT_H
