#include "hintfragment.h"
#include <ui/hintwindow.h>

HintFragment::HintFragment(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName): FragmentUi(pieces, originalImage, fragmentName)
{
    setToolTip(fragmentName);
    connect(this, &FragmentUi::refreshHintWindow, HintWindow::getHintWindow(), &HintWindow::actSuggestTrigged);
    setAcceptHoverEvents(true);
    hoverPrePos = QPointF(-1, -1);
    update();
}

void HintFragment::update(const QRectF &rect)
{
    auto removeBgColorImg = originalImage.copy();
    auto mask = removeBgColorImg.createMaskFromColor(qRgb(0, 0, 0), Qt::MaskMode::MaskOutColor);
    removeBgColorImg.setAlphaChannel(mask);
    setPixmap(QPixmap::fromImage(removeBgColorImg));
    QGraphicsPixmapItem::update(rect);
}

void HintFragment::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    FragmentUi::mousePressEvent(event);
}

void HintFragment::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)
    SuggestedFragment pressedFragment = HintWindow::getHintWindow()->getSuggestedFragmentByHintFragment(this);
    hoverPrePos = pressedFragment.fragCorrToHint->pos();
    hoverPreAng = pressedFragment.fragCorrToHint->rotateAng;
    if (pressedFragment.p1 == nullptr) return;
    cv::Mat trans = pressedFragment.p2->transMat.inv(); // jointed fragent back to start position

    trans = pressedFragment.transMat * trans; // jointed fragment fusion with jointing fragment
    trans = pressedFragment.p1->transMat * trans; // joing fragment back to start position

    cv::Mat areaImg = Tool::QImageToMat(pressedFragment.fragCorrToArea->getOriginalImage());
    cv::Mat hadRotated = Tool::getRotationMatrix(areaImg.cols / 2.0, areaImg.rows / 2.0, Tool::angToRad(pressedFragment.fragCorrToArea->rotateAng));

    trans = Tool::getFirst3RowsMat(hadRotated) * trans; // jointed fragment move with jointing fragment
    trans = pressedFragment.fragCorrToArea->getOffsetMat() * trans; // add jointing fragment offset
    cv::Mat img = Tool::QImageToMat(pressedFragment.fragCorrToHint->getOriginalImage());
    double ang = std::acos(trans.at<float>(0, 0)) * 180.0 / CV_PI;
    if (trans.at<float>(0, 1) < 0) ang = 360.0 - ang;

    pressedFragment.fragCorrToHint->rotate(ang);
    trans = pressedFragment.fragCorrToHint->getOffsetMat().inv() * trans;


    pressedFragment.fragCorrToHint->setX(pressedFragment.fragCorrToArea->x() + trans.at<float>(0, 2));
    pressedFragment.fragCorrToHint->setY(pressedFragment.fragCorrToArea->y() + trans.at<float>(1, 2));
    FragmentUi::hoverEnterEvent(event);
}

void HintFragment::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    SuggestedFragment pressedFragment = HintWindow::getHintWindow()->getSuggestedFragmentByHintFragment(this);
    if (pressedFragment.p1 == nullptr) return;
    if (hoverPrePos != QPointF(-1, -1))
    {
        pressedFragment.fragCorrToHint->setPos(hoverPrePos);
        pressedFragment.fragCorrToHint->rotate(hoverPreAng);
    }
    hoverPrePos = QPointF(-1, -1);
    FragmentUi::hoverLeaveEvent(event);
}
