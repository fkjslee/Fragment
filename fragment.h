#ifndef FRAGMENT_H
#define FRAGMENT_H

#include "QtWidgets"
#include <set>
#include <QGraphicsSceneMouseEvent>

class Fragment;

enum JointMethod {upDown, downUp, leftRight, rightLeft};

struct JointFragment {
    Fragment* fragment;
    JointMethod method;
    int absGrayscale;
    JointFragment(Fragment* fragment, JointMethod method, int absGrayscale)
        : fragment(fragment), method(method), absGrayscale(absGrayscale) {}
};


class Fragment : public QObject
{
    Q_OBJECT
public:
    Fragment( const QImage& image, const QString& fragmentName = "unname");
    Fragment(const Fragment& rhs);
    ~Fragment();
    QColor getProperty() const { return property; }
    const QImage& getImage() const { return image; }
    const QString& getFragmentName() const { return fragmentName; }

    static void createFragments();
    static bool sortFragment(Fragment* frag);
    static bool unsortFragment(Fragment* frag);
    static std::set<Fragment*> getSortedFragments();
    static std::set<Fragment*> getUnsortedFragments();
    static std::vector<JointFragment> getMostPossibleFragments(Fragment* f = nullptr);

public slots:
    void receiveColorItemDragging(QGraphicsSceneMouseEvent* event);

private:
    static std::set<Fragment*> sortedFragments;
    static std::set<Fragment*> unsortedFragments;
    QColor property;
    QImage image;
    QString fragmentName;
};

#endif // FRAGMENT_H
