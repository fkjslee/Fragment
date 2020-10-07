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
        locker.lock();
        calcCnt++;
        locker.unlock();
    }
    void endToCalc()
    {
        calcing = false;
        locker.lock();
        calcCnt--;
        locker.unlock();
    }
    bool getCacl()
    {
        return calcing;
    }
    const cv::Mat getOffsetMat() const
    {
        return offset.clone();
    }
    void rotate(int ang);
    void update(const QRectF &rect = QRectF());
    void scaledToWidth(const double scale);
    int rotateAng = 0;
private:

    void initSuggestAng();
    std::vector<SuggAng> suggAngs;
    bool calcing = false;
    static int calcCnt;
    QMutex locker;
    double scale = 1.0;
    cv::Mat offset;
};

#endif // AREAFRAGMENT_H
