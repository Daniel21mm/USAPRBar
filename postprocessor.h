#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <QWidget>

#include "rod.h"

namespace Ui {
class PostProcessor;
}

class PostProcessor : public QWidget
{
    Q_OBJECT

public:
    explicit PostProcessor(QWidget *parent = Q_NULLPTR);

    void              setDataRods(const QVector<qreal>& deltas, const QVector<Rod> &rods);

    ~PostProcessor();

signals:
    void               sgClickBack();


private:
    void               addTableRod(const qreal& deltaBegin, const qreal &deltaEnd, const Rod& rod);

private slots:

    void               calcValueByPoint(double x);

private:
    Ui::PostProcessor *ui;

    QVector<qreal>      deltas_;
    QVector<Rod>        rods_;
};

#endif // POSTPROCESSOR_H
