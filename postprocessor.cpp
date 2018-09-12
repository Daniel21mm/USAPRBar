#include "postprocessor.h"
#include "ui_postprocessor.h"

#include <QTableWidget>

PostProcessor::PostProcessor(QWidget *parent)
    :QWidget(parent),
      ui(new Ui::PostProcessor)
{
    ui->setupUi(this);
    connect(ui->point,  SIGNAL(valueChanged(double)), this,  SLOT(calcValueByPoint(double)));

    connect(ui->backButton, SIGNAL(clicked(bool)), this, SIGNAL(sgClickBack()));

}

void PostProcessor::setDataRods(const QVector<qreal> &deltas, const QVector<Rod> &rods)
{
    ui->listWidget->clear();

    deltas_ = deltas;
    rods_ = rods;

    double maxLen = 0;
    for( Rod rod : rods)
        maxLen += rod.L;
    ui->point->setMaximum(maxLen);

    calcValueByPoint(0);


    for(int i(0); i < rods.size(); i++)
        addTableRod(deltas.at(i), deltas.at(i+1), rods.at(i));

}

PostProcessor::~PostProcessor()
{
    delete ui;
}

void PostProcessor::addTableRod(const qreal &deltaBegin,const qreal &deltaEnd, const Rod &rod)
{

     QTableWidget* table = new QTableWidget{};
     table->setRowCount(8);
     table->setColumnCount(5);
     table->setHorizontalHeaderItem(0, new QTableWidgetItem("x"));
     table->setHorizontalHeaderItem(1, new QTableWidgetItem("Ux"));
     table->setHorizontalHeaderItem(2, new QTableWidgetItem("Nx"));
     table->setHorizontalHeaderItem(3, new QTableWidgetItem("Gx"));
     table->setHorizontalHeaderItem(4, new QTableWidgetItem("G"));

     qreal step = rod.L/7;
     qreal x = 0;

     for(int row = 0; row < 8; row++)
     {
        table->setItem(row, 0, new QTableWidgetItem(QString::number(x)));

        qreal U = deltaBegin + (x/rod.L)*(deltaEnd - deltaBegin) + ((rod.Qx*rod.L*x)/(2*rod.E*rod.A))*(1 - (x/rod.L));
        table->setItem(row, 1, new QTableWidgetItem(QString::number(U)));

        qreal N = (rod.E*rod.A/rod.L)*(deltaEnd - deltaBegin) + (rod.Qx*rod.L/2)*(1 - 2*x/rod.L);
        table->setItem(row, 2, new QTableWidgetItem(QString::number(N)));

        qreal Gx = N/rod.A;
        table->setItem(row, 3, new QTableWidgetItem(QString::number(Gx)));

        table->setItem(row, 4, new QTableWidgetItem(QString::number(rod.G)));

        x += step;
     }

     QListWidgetItem* item = new QListWidgetItem{};


     item->setSizeHint(QSize(700, 300));

     ui->listWidget->addItem(item);
     ui->listWidget->setItemWidget(item, table);
}

void PostProcessor::calcValueByPoint(double x)
{
    double left = 0;
    int index = 0;

    for(int i = 0; i < rods_.size(); i++)
    {
        double right = left + rods_.at(i).L;


        if( x >= left && x < right) {
            index = i;
            break;
        }

        left = right;
    }


    qreal deltaBegin = deltas_[index];
    qreal deltaEnd = deltas_[index+1];
    Rod rod = rods_[index];

    qreal U = deltaBegin + (x/rod.L)*(deltaEnd - deltaBegin) + ((rod.Qx*rod.L*x)/(2*rod.E*rod.A))*(1 - (x/rod.L));
    ui->Ux->setText(QString::number(U));

    qreal N = (rod.E*rod.A/rod.L)*(deltaEnd - deltaBegin) + (rod.Qx*rod.L/2)*(1 - 2*x/rod.L);
    ui->Nx->setText(QString::number(N));

    qreal Gx = N/rod.A;
    ui->Gx->setText(QString::number(Gx));

    ui->globalG->setText(QString::number(rod.G));

}

