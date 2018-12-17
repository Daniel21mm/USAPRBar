#include "processor.h"

#include <math.h>

Processor::Processor(const QVector<Rod> &rods, const QVector<qreal> &nodes,
                     bool leftFix, bool rightFix)
{
    size_t sizeMatrixA = rods.size() + 1;

    matrixA.resize(sizeMatrixA);
    for(int i = 0; i < matrixA.size(); i++)
    {
        matrixA[i].resize(sizeMatrixA);
        for(int j = 0; j < matrixA[i].size(); j++)
             matrixA[i][j] = 0;
    }

    int x(0), y(0);
    for(int i = 0; i < sizeMatrixA - 1; i++)
    {
        qreal K = (rods[i].E * rods[i].A) / rods[i].L;

        matrixA[x][y]     += K;
        matrixA[x+1][y]   += -K;
        matrixA[x][y+1]   += -K;
        matrixA[x+1][y+1] += K;

        x++;
        y++;
    }

    vectorB.resize(sizeMatrixA);
    for(int i = 0; i < sizeMatrixA; i++)
    {
      qreal Qs(0), Qk(0);

      if( i < sizeMatrixA - 1)
      {
          qreal Qx = rods[i].Qx;
          qreal L  = rods[i].L;

          Qs = Qx * L / 2;
      }

      if( i > 0 )
      {
          qreal Qx = rods[i - 1].Qx;
          qreal L  = rods[i - 1].L;

          Qk =  Qx * L / 2;
      }

      qreal B = Qs + Qk + nodes[i];
      vectorB[i] = B;

    }


    if(leftFix)
    {
        matrixA[0][0] = 1;
        matrixA[0][1] = 0;

        vectorB[0] = 0;
    }

    if(rightFix)
    {
        int n = sizeMatrixA - 1;

        matrixA[n][n] = 1;
        matrixA[n][n-1] = 0;

        vectorB[n] = 0;
    }
}

QVector<qreal> Processor::deltas()
{
    qreal mainDelta = detA(matrixA);

    QVector<qreal> listDelta{};
    for(int i = 0; i < vectorB.size(); i++)
    {
        listDelta.push_back(  detA(newMatrix(matrixA, vectorB, i)) / mainDelta );
    }

    return listDelta;
}

qreal Processor::detA(const Processor::Matrix &m)
{
    if(m.size() == 2)
    {
        return m[0][0]*m[1][1] - m[0][1]*m[1][0];
    }
    else
    {
        qreal rez = 0.0;
        for(int i(0); i < m.size(); i++)
        {
            rez += m[0][i] * pow(-1, i) * detA( getMinor(m, i) );
        }
        return rez;
    }
}

Processor::Matrix Processor::getMinor(const Processor::Matrix &m, int index)
{
    Matrix newMinor(m.size() -1);

    for( int i = 0; i < newMinor.size(); i++)
        newMinor[i].resize(m.size() - 1);

    for(int i = 1; i < m.size(); i++)
    {
        for(int j = 0, in = 0; j < m.size(); j++)
        {
            if( index != j)
            {
                newMinor[i-1][in] = m[i][j];
                in++;
            }
        }
    }

    return newMinor;

}

Processor::Matrix Processor::newMatrix(const Processor::Matrix &m, const QVector<qreal> &v, int index)
{
    Matrix matrix(m.size());
    for(int i = 0; i < matrix.size(); i++)
        matrix[i].resize(m.size());


    for(int i = 0; i < m.size(); i++)
    {
        for(int j = 0; j < m.size(); j++)
        {
            if(j != index)
                matrix[i][j] = m[i][j];
            else
                matrix[i][j] = v[i];
        }
    }

    return matrix;
}
