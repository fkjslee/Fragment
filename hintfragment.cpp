#include "hintfragment.h"
#include <ui/hintwindow.h>

HintFragment::HintFragment(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName): FragmentUi(pieces, originalImage, fragmentName)
{
    setToolTip(fragmentName);
    connect(this, &HintFragment::mousePressFragment, HintWindow::getHintWindow(), &HintWindow::mousePressFragment);
    connect(this, &FragmentUi::refreshHintWindow, HintWindow::getHintWindow(), &HintWindow::actSuggestTrigged);
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
    emit mousePressFragment(this);
}
