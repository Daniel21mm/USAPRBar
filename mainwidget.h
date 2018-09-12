#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = Q_NULLPTR);
    ~MainWidget();

private:
    void           openFileProject(const QString& fileName);

private slots:
    void           openProject();
    void           createProject();
    void           writeToFile();
    void           goCala();

    void           ininWnd();

private:
    Ui::MainWidget *ui;

    QString         projectName;
};

#endif // MAINWIDGET_H
