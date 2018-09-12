#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <QWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "rod.h"

namespace Ui {
class PreProcessor;
}

class PreProcessor : public QWidget
{
    Q_OBJECT

    enum class HEADER {FX, L, A, E, G, QX};
    enum class FIXING {LEFT, RIGHT, FULL};

public:
    explicit PreProcessor(QWidget *parent = 0);
    ~PreProcessor();

    void              setDataToFile(const QByteArray &data);
    QByteArray        dataToJson();

    void              initUi();

    QVector<qreal>    nodes();
    QVector<Rod>      rods();
    bool              leftFix();
    bool              rightFix();

private:
    int              columnByHeader(HEADER header) const;

    FIXING           statusFixing();
    QString          fixingToStr();
    void             setFixingByString(const QString& fixing);

signals:

    void             sgClicedBack();

private slots:

    void             clickedAddRods();
    void             clickedDelRods();
    void             draw();

    void            addRod(qreal L = 1, qreal A = 3, qreal E = 1, qreal G = 16, qreal QX = 0);
    void            deleteRod();
    void            addNode(qreal FX = 0);
    void            deleteNode();

    void            draw_construction(const QList<qreal> &lengths,
                                      const QList<qreal> &areas,
                                      const QList<qreal> &listFx,
                                      const QList<qreal> &listQx);

private:
    Ui::PreProcessor *ui;
};

#endif // PREPROCESSOR_H
