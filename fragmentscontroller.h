#ifndef FRAGMENTSCONTROLLER_H
#define FRAGMENTSCONTROLLER_H

#include <fragment.h>
#include <vector>

enum JointMethod {leftRight, rightLeft, upDown, downUp};

struct JointFragment
{
    Fragment *item;
    JointMethod method;
    double absGrayscale;
    JointFragment(Fragment *item, JointMethod method, double absGrayscale)
        : item(item), method(method), absGrayscale(absGrayscale) {}
};

class FragmentsController
{
public:
    void createAllFragments(const QString &fragmentsPath);
    static FragmentsController *getController();
    /* todo
     * Choose most possible fragment which could be jointed by giving fragment(item)
     */
    std::vector<JointFragment> getMostPossibleFragments(Fragment *item = nullptr);
    /* todo
     * get most possible jointsing method through giving fragment(f1 and f2)
     */
    JointFragment mostPossibleJointMethod(Fragment *f1, Fragment *f2);
    bool splitSelectedFragments();
    const std::vector<Fragment *> getSelectedFragments();
    std::vector<Fragment *> getUnsortedFragments();
    std::vector<Fragment *> getSortedFragments();
    bool jointFragment(Fragment *f1, JointFragment jointFragment);
    void reverseChosenFragment(Fragment *f);

private:
    FragmentsController();

private:
    static FragmentsController *controller;

    std::vector<Fragment *> sortedFragments;
    std::vector<Fragment *> unsortedFragments;
    std::vector<Fragment *> chosenFragments;
};

#endif // FRAGMENTSCONTROLLER_H
