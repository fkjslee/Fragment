#include "hintfragment.h"

HintFragment::HintFragment(const std::vector<Piece> &pieces, const QImage &originalImage, const QString &fragmentName): FragmentUi(pieces, originalImage, fragmentName)
{
    setToolTip(fragmentName);
}
