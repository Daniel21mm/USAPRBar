#include "mainwidget.h"
#include "ui_mainwidget.h"

#include "processor.h"

#include <QFileDialog>
#include <QDebug>
#include <QFile>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    projectName("")
{
    //setWindowState( Qt::WindowMaximized );
    ui->setupUi(this);

    ui->save->hide();
    ui->calc->hide();

    connect(ui->createrProject, &QPushButton::clicked,
            this,               &MainWidget::createProject);
    connect(ui->openProject,    &QPushButton::clicked,
            this,               &MainWidget::openProject);
    connect(ui->save,           &QPushButton::clicked,
            this,               &MainWidget::writeToFile);
    connect(ui->calc,           &QPushButton::clicked,
            this,               &MainWidget::goCala);

    connect(ui->preProcessor,   &PreProcessor::sgClicedBack,
            this,               &MainWidget::ininWnd);

    connect(ui->postProcessor,  &PostProcessor::sgClickBack,[=](){
        ui->stackedWidget->setCurrentWidget(ui->preProcessor);
        ui->save->show();
        ui->calc->show();
    });
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::openFileProject(const QString &fileName)
{
    QByteArray data{""};
    QFile fileProject(fileName);
    if(!fileName.isEmpty() && fileProject.open(QIODevice::ReadWrite))
    {
        data = fileProject.readAll();
    }
    fileProject.close();

    projectName = fileName;
    ui->projectName->setText(fileName);


    if(!data.isEmpty())
        ui->preProcessor->setDataToFile(data);

    ui->stackedWidget->setCurrentWidget(ui->preProcessor);
    ui->save->show();
    ui->calc->show();
}

void MainWidget::openProject()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть файл проекта"),
                                                    "",tr("*.usapr"));
    if(!fileName.isEmpty())
        openFileProject(fileName);

}

void MainWidget::createProject()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Создать файл проекта"),
                                                    "untitled.usapr",
                                                    tr("*.usapr"));
    if(!fileName.isEmpty())
        openFileProject(fileName);
}

void MainWidget::writeToFile()
{
    QByteArray data = ui->preProcessor->dataToJson();
    QFile fileProject(projectName);
    if(fileProject.open(QIODevice::WriteOnly))
    {
        fileProject.write(data);
        //qDebug() << fileProject.readAll() << endl;
    }
    fileProject.close();

}

void MainWidget::goCala()
{
    QVector<Rod> rods       = ui->preProcessor->rods();
    QVector<qreal> nodes    = ui->preProcessor->nodes();
    bool leftFix            = ui->preProcessor->leftFix();
    bool rightFix           = ui->preProcessor->rightFix();

    Processor pro(rods, nodes, leftFix, rightFix);

    QVector<qreal> delta = pro.deltas();

    for(qreal& d : delta)
        qDebug() << d << " ";

    ui->postProcessor->setDataRods(delta, rods);

    ui->save->hide();
    ui->calc->hide();

    ui->stackedWidget->setCurrentWidget(ui->postProcessor);


}

void MainWidget::ininWnd()
{
    ui->save->hide();
    ui->calc->hide();

    projectName = "ProjectName";
    ui->projectName->setText(projectName);

    ui->stackedWidget->setCurrentWidget(ui->StartWnd);
}
