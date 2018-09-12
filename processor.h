#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QVector>

#include "rod.h"

class Processor
{
    using Matrix = QVector<QVector<qreal>>;

public:
    Processor(const QVector<Rod> &rods, const QVector<qreal>& nodes, bool leftFix, bool rightFix);

    QVector<qreal>  deltas();


private:
    qreal           detA( const Matrix& m );
    Matrix          getMinor(const Matrix& m, int index);
    Matrix          newMatrix(const Matrix& m, const QVector<qreal>& v, int index);

private:

    Matrix         matrixA;
    QVector<qreal>  vectorB;


};

#endif // PROCESSOR_H
