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
    moveRelatedPieceToPos(pressedFragment.p1, pressedFragment.p2, pressedFragment.transMat);
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

void HintFragment::moveRelatedPieceToPos(const Piece *p1, const Piece *p2, cv::Mat transMat)
{
    if (p1 == nullptr) return;
    cv::Mat trans = p2->transMat.inv(); // jointed fragent back to start position

    trans = transMat * trans; // jointed fragment fusion with jointing fragment
    trans = p1->transMat * trans; // joing fragment back to start position

    AreaFragment *fragCorrToArea = FragmentsController::getController()->getFragmentByPiece(p1);
    AreaFragment *fragCorrToHint = FragmentsController::getController()->getFragmentByPiece(p2);

    cv::Mat areaImg = Tool::QImageToMat(fragCorrToArea->getOriginalImage());
    cv::Mat hadRotated = Tool::getRotationMatrix(areaImg.cols / 2.0, areaImg.rows / 2.0, Tool::angToRad(fragCorrToArea->rotateAng));

    trans = Tool::getFirst3RowsMat(hadRotated) * trans; // jointed fragment move with jointing fragment
    trans = fragCorrToArea->getOffsetMat() * trans; // add jointing fragment offset
    cv::Mat img = Tool::QImageToMat(fragCorrToHint->getOriginalImage());
    double ang = std::acos(trans.at<float>(0, 0)) * 180.0 / CV_PI;
    if (trans.at<float>(0, 1) < 0) ang = 360.0 - ang;

    fragCorrToHint->rotate(ang);
    trans = fragCorrToHint->getOffsetMat().inv() * trans;


    fragCorrToHint->setX(fragCorrToArea->x() + trans.at<float>(0, 2) * (MainWindow::mainWindow->getZoomSize() / 100.0));
    fragCorrToHint->setY(fragCorrToArea->y() + trans.at<float>(1, 2) * (MainWindow::mainWindow->getZoomSize() / 100.0));
}
