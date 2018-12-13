#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->ge5,SIGNAL(imageClicked(int,int)),this,SLOT(testclicked(int,int)));

    ui->ge5->load("sprites.ge5");

    ui->spritegrd->setDataSource(ui->ge5);
    ui->spritegrd->updateImage();

    connect(ui->ge5,SIGNAL(imageChanged()),
            ui->spritegrd,SLOT(updateImage()));
    connect(ui->ge5,SIGNAL(imageConverted()),
            ui->spritegrd,SLOT(updateImage()));
    connect(ui->ge5,SIGNAL(imageClicked(int,int)),
            ui->spritegrd,SLOT(spriteClicked(int,int)));

    ui->spritepatvw->setDataSource(ui->spritegrd);
    ui->spritepatvw->setGE5DataSource(ui->ge5);
    ui->spritepatvw->updateImage();

    connect(ui->spritegrd,SIGNAL(spriteChanged(int,int,int)),
            ui->spritepatvw,SLOT(updateSprite(int,int,int)));
    connect(ui->ge5,SIGNAL(imageChanged()),
            ui->spritepatvw,SLOT(updateImage()));


    connect(ui->action_Open_ge5_file, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->action_Save_gen_file, SIGNAL(triggered()), this, SLOT(saveGen()));
    connect(ui->actionSave_grid_to_ge5, SIGNAL(triggered()), this, SLOT(saveGridtoGE5()));
    connect(ui->actionLoad_ge5_to_grid, SIGNAL(triggered()), this, SLOT(loadGE5toGrid()));
    connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionShow_grid, SIGNAL(triggered()), this, SLOT(showGrid()));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));


}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testclicked(int x,int y)
{
    printf("\nClicked signal x=%i y=%i \n",x,y);
}
void MainWindow::open()
{
        QString fileName = QFileDialog::getOpenFileName(this,
                                        tr("Open File"), QDir::currentPath());
        if (!fileName.isEmpty()) {
        ui->ge5->load(fileName);
        }
}
void MainWindow::loadGE5toGrid()
{
        QString fileName = QFileDialog::getOpenFileName(this,
                                        tr("Open File"), QDir::currentPath());
        if (!fileName.isEmpty()) {
        ui->spritegrd->loadFromGE5(&fileName);
        }
}

void MainWindow::showGrid()
{
    ui->ge5->setShowGrid(ui->actionShow_grid->isChecked());
}

void MainWindow::saveGen()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save as"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        ui->spritegrd->save2file(&fileName);
    }
}
void MainWindow::saveGridtoGE5()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save as"), QDir::currentPath());
    if (!fileName.isEmpty()) {
        ui->spritegrd->save2GE5(&fileName);
    }
}

void MainWindow::about()
{
        QMessageBox::about(this, "About ",
                "This simple GE5 sprite convertor is written by David Heremans"
                 "<br>Released under GPL2.0");
}
