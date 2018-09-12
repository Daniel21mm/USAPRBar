#ifndef ROD_H
#define ROD_H

#include <QtCore>


struct Rod
{

    qreal A;
    qreal L;
    qreal E;
    qreal G;
    qreal Qx;

    Rod();
    Rod(qreal a, qreal l, qreal e, qreal g, qreal qx) : A(a), L(l), E(e), G(g), Qx(qx) {}
    Rod(const Rod& r): A(r.A), L(r.L), E(r.E), G(r.G), Qx(r.Qx) {}

};

#endif // ROD_H
