#include "preprocessor.h"
#include "ui_preprocessor.h"

#include <QDebug>
#include <QPainter>
#include <QMessageBox>
#include <QDoubleSpinBox>

PreProcessor::PreProcessor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreProcessor)
{
    ui->setupUi(this);
    initUi();

    connect(ui->addRods,    &QPushButton::clicked,
            this,           &PreProcessor::clickedAddRods);
    connect(ui->delRods,    &QPushButton::clicked,
            this,           &PreProcessor::clickedDelRods);

    connect(ui->fullFixing, &QRadioButton::clicked,
            this,           &PreProcessor::draw);
    connect(ui->leftFixing, &QRadioButton::clicked,
            this,           &PreProcessor::draw);
    connect(ui->rightFixing,&QRadioButton::clicked,
            this,           &PreProcessor::draw);

    connect(ui->backButton, &QPushButton::clicked,
            this,           &PreProcessor::sgClicedBack);
}

PreProcessor::~PreProcessor()
{
    delete ui;
}

void PreProcessor::setDataToFile(const QByteArray& data)
{

    QJsonObject json = QJsonDocument::fromJson(data).object();

    ui->countRods->setText(QString::number(json.value("count").toInt()));
    setFixingByString(json.value("fixing").toString());


    ui->tableNodes->setRowCount(0);
    QJsonArray Fx = json.value("Fx").toArray();
    for(int i = 0; i < Fx.size(); i++)
    {
        addNode(Fx.at(i).toDouble());
    }

    ui->tableRods->setRowCount(0);
    QJsonArray Rods = json.value("Rods").toArray();
    for(int i = 0; i < Rods.size(); i++)
    {
        QJsonObject row = Rods.at(i).toObject();

        addRod(row.value("L").toDouble(),
               row.value("A").toDouble(),
               row.value("E").toDouble(),
               row.value("G").toDouble(),
               row.value("QX").toDouble());
    }

    draw();

}

QByteArray PreProcessor::dataToJson()
{
    QJsonObject json{};

    json.insert("count", ui->countRods->text().toInt());
    json.insert("fixing", fixingToStr());

    QJsonArray Fx{};

    for(int i = 0; i < ui->tableNodes->rowCount(); i++ )
    {
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableNodes->cellWidget(i, columnByHeader(HEADER::FX))))
            Fx.append(QJsonValue(sp->value()));
    }

    json.insert("Fx", QJsonValue(Fx));

    QJsonArray Rods{};

    for(int i = 0; i < ui->tableRods->rowCount(); i++)
    {
        QJsonObject rod{};

        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::A))))
            rod.insert("A", QJsonValue(sp->value()));
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::L))))
            rod.insert("L", QJsonValue(sp->value()));
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::QX))))
            rod.insert("QX", QJsonValue(sp->value()));
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::E))))
            rod.insert("E", QJsonValue(sp->value()));
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::G))))
            rod.insert("G", QJsonValue(sp->value()));

        Rods.append(QJsonValue(rod));
    }

    json.insert("Rods", QJsonValue(Rods));

    return QJsonDocument(json).toJson();
}

void PreProcessor::initUi()
{
    addRod();

    addNode();
    addNode();

    draw();

}

QVector<qreal> PreProcessor::nodes()
{
    QVector<qreal> listFx{};
    for(int i = 0; i < ui->tableNodes->rowCount(); i++ )
    {
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableNodes->cellWidget(i, columnByHeader(HEADER::FX))))
            listFx.push_back(sp->value());
    }

    return listFx;
}

QVector<Rod> PreProcessor::rods()
{
    QVector<Rod> rods;

    for(int i = 0; i < ui->tableRods->rowCount(); i++)
    {
        Rod rod;

        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::A))))
            rod.A = sp->value();
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::L))))
            rod.L =sp->value();
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::QX))))
            rod.Qx = sp->value();
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::E))))
            rod.E = sp->value();
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::G))))
            rod.G = sp->value();

        rods.push_back(rod);
    }

    return rods;
}

bool PreProcessor::leftFix()
{
    if( statusFixing() == FIXING::FULL || statusFixing() == FIXING::LEFT)
        return true;
    else
        return false;
}

bool PreProcessor::rightFix()
{
    if( statusFixing() == FIXING::FULL || statusFixing() == FIXING::RIGHT)
        return true;
    else
        return false;
}

int PreProcessor::columnByHeader(HEADER header) const
{
    switch (header) {
    case HEADER::FX:
    case HEADER::L:
        return 0;
    case HEADER::A:
        return 1;
    case HEADER::E:
        return 2;
    case HEADER::G:
        return 3;
    case HEADER::QX:
        return 4;
    default:
        break;
    }
}

PreProcessor::FIXING PreProcessor::statusFixing()
{
    if(ui->leftFixing->isChecked())
        return FIXING::LEFT;
    else if(ui->rightFixing->isChecked())
        return FIXING::RIGHT;
    else if(ui->fullFixing->isChecked())
        return FIXING::FULL;
}

QString PreProcessor::fixingToStr()
{
    if(ui->leftFixing->isChecked())
        return "LEFT";
    else if(ui->rightFixing->isChecked())
        return "RIGHT";
    else if(ui->fullFixing->isChecked())
        return "FULL";
}

void PreProcessor::setFixingByString(const QString &fixing)
{
    if(fixing == "LEFT")
        ui->leftFixing->setChecked(true);
    else if(fixing == "RIGHT")
        ui->rightFixing->setChecked(true);
    else if(fixing == "FULL")
        ui->fullFixing->setChecked(true);
}

void PreProcessor::clickedAddRods()
{
    int count = ui->countRods->text().toInt() + 1;

    if(count <= 25)
    {
        addRod();
        addNode();
        draw();
        ui->countRods->setText(QString::number(count));
    }
    else
    {
       QMessageBox::information(this, "Максимальное кол-во сержней",
                                "Вы создали максимальное кол-во сержней");
    }
}

void PreProcessor::clickedDelRods()
{
    int count = ui->countRods->text().toInt() - 1;

    if(count > 0)
    {
        deleteRod();
        deleteNode();
        draw();
        ui->countRods->setText(QString::number(count));
    }
    else
    {
       QMessageBox::information(this, "Удаление последнего стержня",
                                "У вас остался только один стержень");
    }
}

void PreProcessor::draw()
{
    QList<qreal> lengths{};
    QList<qreal> areas{};
    QList<qreal> listQx{};

    for(int i = 0; i < ui->tableRods->rowCount(); i++ )
    {
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::A))))
            areas.push_back(sp->value());
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::L))))
            lengths.push_back(sp->value());
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableRods->cellWidget(i, columnByHeader(HEADER::QX))))
            listQx.push_back(sp->value());
    }

    QList<qreal> listFx{};
    for(int i = 0; i < ui->tableNodes->rowCount(); i++ )
    {
        if( QDoubleSpinBox* sp = qobject_cast<QDoubleSpinBox*>(ui->tableNodes->cellWidget(i, columnByHeader(HEADER::FX))))
            listFx.push_back(sp->value());
    }

    draw_construction(lengths, areas, listFx, listQx);

}
void PreProcessor::addRod(qreal L, qreal A, qreal E, qreal G, qreal QX)
{
    int row = ui->tableRods->rowCount();
    ui->tableRods->setRowCount(row + 1);

    QDoubleSpinBox* sbL = new QDoubleSpinBox;
    sbL->setRange(0.000001, 10000);
    sbL->setValue(L);
    sbL->setDecimals(6);
    //sbL->setSuffix("см");
    ui->tableRods->setCellWidget(row, columnByHeader(HEADER::L), sbL);

    QDoubleSpinBox* sbA = new QDoubleSpinBox;
    sbA->setRange(0.0000001, 10000);
    sbA->setValue(A);
    sbA->setDecimals(6);
    //sbA->setSuffix("см^2");
    ui->tableRods->setCellWidget(row, columnByHeader(HEADER::A), sbA);

    QDoubleSpinBox* sbE = new QDoubleSpinBox;
    sbE->setRange(0.0000001, 1000000000000);
    sbE->setValue(E);
    sbE->setDecimals(6);
    //sbE->setSuffix("МПа");
    ui->tableRods->setCellWidget(row, columnByHeader(HEADER::E), sbE);

    QDoubleSpinBox* sbG = new QDoubleSpinBox;
    sbG->setRange(0.0000001, 1000000000000);
    sbG->setValue(G);
    sbG->setDecimals(6);
    //sbG->setSuffix("МПа");
    ui->tableRods->setCellWidget(row, columnByHeader(HEADER::G), sbG);


    QDoubleSpinBox* sbQX = new QDoubleSpinBox;
    sbQX->setRange(-10000, 10000);
    sbQX->setValue(QX);
    sbQX->setDecimals(6);
    //sbQX->setSuffix("кН/м^2");
    ui->tableRods->setCellWidget(row, columnByHeader(HEADER::QX), sbQX);

    connect(sbL, SIGNAL(valueChanged(double)), this, SLOT(draw()));
    connect(sbA, SIGNAL(valueChanged(double)), this, SLOT(draw()));
    connect(sbE, SIGNAL(valueChanged(double)), this, SLOT(draw()));
    connect(sbG, SIGNAL(valueChanged(double)), this, SLOT(draw()));
    connect(sbQX, SIGNAL(valueChanged(double)), this, SLOT(draw()));
}

void PreProcessor::deleteRod()
{
    int row = ui->tableRods->rowCount();
    QList<HEADER> headers = {HEADER::L, HEADER::A, HEADER::E, HEADER::G, HEADER::QX};

    for(HEADER header : headers)
    {
        ui->tableRods->setCellWidget(row, columnByHeader(header), Q_NULLPTR);
    }

    ui->tableRods->setRowCount(row - 1);
}

void PreProcessor::addNode(qreal FX)
{
    int row = ui->tableNodes->rowCount();
    ui->tableNodes->setRowCount(row + 1);

    QDoubleSpinBox* sp = new QDoubleSpinBox;
    sp->setRange(-10000, 10000);
    sp->setValue(FX);
    sp->setDecimals(6);
    //sp->setSuffix("кН/м");
    ui->tableNodes->setCellWidget(row, columnByHeader(HEADER::FX), sp);

    connect(sp, SIGNAL(valueChanged(double)), this, SLOT(draw()));
}

void PreProcessor::deleteNode()
{
    int row = ui->tableNodes->rowCount();

    ui->tableNodes->setCellWidget(row, columnByHeader(HEADER::FX), Q_NULLPTR);
    ui->tableNodes->setRowCount(row - 1);
}

void PreProcessor::draw_construction(const QList<qreal> &lengths,
                                     const QList<qreal> &areas,
                                     const QList<qreal> &listFx,
                                     const QList<qreal> &listQx)
{
    QPixmap canvas(":/pics/canvas.png");

    qreal constrHeight = 0, constrWidth =0;
    QList<int> nodes{0};

    for(int i = 0; i < lengths.size(); i++)
    {
        constrHeight += lengths[i];
        constrWidth += areas[i];
        nodes.push_back(nodes.back() + lengths[i]);
    }

    qreal cH = canvas.height() * 0.8, cW = canvas.width() * 0.8;
    qreal KW = cW/constrHeight;
    qreal KH = cH/constrWidth;


    qreal centrLine = canvas.height()/2;


    QPainter pen;

    pen.begin(&canvas);

    pen.setPen(QPen(QColor(0, 0, 0), 2));


    //рисуем стержни
    for(int i = 0; i < lengths.size(); i++)
    {
        qreal left = nodes[i]*KW + canvas.width() * 0.1;
        qreal top = centrLine - ((areas[i]*KH > 6 ) ? (areas[i]*KH) : 6)/2;
        qreal width = lengths[i]*KW;
        qreal height = (areas[i]*KH > 6 ) ? (areas[i]*KH) : 6 ; //2

        pen.drawRect(QRectF(left, top, width, height));
    }

    //рисуем заделки
    pen.setPen(QPen(QColor(0, 0, 255), 2));

    if(FIXING::LEFT == statusFixing() || FIXING::FULL == statusFixing())
    {
        qreal x1 = nodes.front() * KW + canvas.width() * 0.1;
        qreal y1 = centrLine - 4 * 5;
        qreal x2 = x1;
        qreal y2 = centrLine + 4 * 5;
        pen.drawLine(QPointF(x1, y1), QPointF(x2, y2));

        for(int i = -4; i <= 4; i++)
        {
            qreal x1 = nodes.front() * KW + canvas.width() * 0.1;
            qreal y1 = centrLine + i * 5;
            qreal x2 = x1 - 5;
            qreal y2 = y1 + 5;
            pen.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
    }

    if(FIXING::RIGHT == statusFixing() || FIXING::FULL == statusFixing())
    {
        qreal x1 = nodes.back() * KW + canvas.width() * 0.1;
        qreal y1 = centrLine - 4 * 5;
        qreal x2 = x1;
        qreal y2 = centrLine + 4 * 5;
        pen.drawLine(QPointF(x1, y1), QPointF(x2, y2));

        for(int i = -4; i <= 4; i++)
        {
            qreal x1 = nodes.back() * KW + canvas.width() * 0.1;
            qreal y1 = centrLine + i * 5;
            qreal x2 = x1 + 5;
            qreal y2 = y1 + 5;
            pen.drawLine(QPointF(x1, y1), QPointF(x2, y2));
        }
    }

    //рисуем Qx
    pen.setPen(QPen(QColor(0, 255, 0), 3));
    for(int i = 0; i < listQx.size(); i++ )
    {
        int step = 10;

        if(listQx[i] > 0)
        {
            qreal begin = nodes[i] * KW + canvas.width() * 0.1;
            qreal end = nodes[i + 1] * KW + canvas.width() * 0.1;

            for(int j = begin + step; j < end; j+=step)
            {
                qreal x1 = j;
                qreal y1 = centrLine;
                qreal x2 = (x1 - step > begin) ? x1 - step : begin;
                qreal y2 = y1 - step/2, y3 = y1 + step/2;

                pen.drawLine(QPointF(x1, y1), QPointF(x2, y2));
                pen.drawLine(QPointF(x1, y1), QPointF(x2, y3));

            }

        }
        else if(listQx[i] < 0)
        {

            qreal begin = nodes[i] * KW + canvas.width() * 0.1;
            qreal end = nodes[i + 1] * KW + canvas.width() * 0.1;

            for(int j = begin; j < end; j+=step)
            {
                qreal x1 = j;
                qreal y1 = centrLine;
                qreal x2 = (x1 + step < end) ? x1 + step : end;
                qreal y2 = y1 - step/2, y3 = y1 + step/2;

                pen.drawLine(QPointF(x1, y1), QPointF(x2, y2));
                pen.drawLine(QPointF(x1, y1), QPointF(x2, y3));

            }
        }
    }

    //рисуем Fx
    pen.setPen(QPen(QColor(255, 0, 0), 4));

    for(int i = 0; i < listFx.size(); i++)
    {
        if(listFx[i] > 0)
        {
            qreal x1 = nodes[i] * KW + canvas.width() * 0.1;
            qreal y1 = centrLine;
            qreal x2 = x1 - 5;
            qreal y2 = y1 - 5, y3 = y1 + 5;

            pen.drawLine(QPointF(x1, y1), QPointF(x2, y2));
            pen.drawLine(QPointF(x1, y1), QPointF(x2, y3));
            pen.drawLine(QPointF(x1, y1), QPointF(x2 - 5, y1));

        }
        else if(listFx[i] < 0)
        {
            qreal x1 = nodes[i] * KW + canvas.width() * 0.1;
            qreal y1 = centrLine;
            qreal x2 = x1 + 5;
            qreal y2 = y1 - 5, y3 = y1 + 5;

            pen.drawLine(QPointF(x1, y1), QPointF(x2, y2));
            pen.drawLine(QPointF(x1, y1), QPointF(x2, y3));
            pen.drawLine(QPointF(x1, y1), QPointF(x2 + 5, y1));
        }
    }

    pen.end();

    ui->canvan->setPixmap(canvas);
}

