#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include "GE5Widget.hh"
#include "spriteGrid.hh"
#include "spritePaternView.hh"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void open();
    void saveGen();
    void loadGE5toGrid();
    void saveGridtoGE5();

    void showGrid();

    void about();

    void testclicked(int x,int y);

private:
    Ui::MainWindow *ui;

    GE5Widget *ge5;
    spriteGrid *spritegrd;
    spritePaternView *spritepatvw;

};

#endif // MAINWINDOW_H
