#ifndef LOOPTYPE_H
#define LOOPTYPE_H

#include <QString>

class LoopType
{
public:
    LoopType();

    int numTurns;
    double wireRadius;
    double relPerm;
    double height;
    double width;
    double scale;

    QString relPermLastString;
};

#endif // LOOPTYPE_H
