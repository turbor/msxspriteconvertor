#ifndef MAINWINDOW
#define MAINWINDOW

#include <QtWidgets>
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include "GE5Widget.hh"
#include "spriteGrid.hh"
#include "spritePaternView.hh"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
    MainWindow();
    
private slots:
    void open();
    void saveGen();
    void loadGE5toGrid();
    void saveGridtoGE5();

    void zoom1x();
    void zoom2x();
    void zoom3x();
    void showGrid();

    void about();

    void testclicked(int x,int y);

private:
    void createActions();
    void createMenus();

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    QAction *openAct;
    QAction *saveGenAct;
    QAction *saveGridGE5Act;
    QAction *loadGE5GridAct;
    QAction *exitAct;

    QActionGroup *zoomActGrp;
    QAction *zoom1xAct;
    QAction *zoom2xAct;
    QAction *zoom3xAct;
    QAction *showGridAct;

    QAction *aboutAct;
    QAction *aboutQtAct;

    GE5Widget *ge5;
    spriteGrid *spritegrd;
    spritePaternView *spritepatvw;
};

#endif // MAINWINDOW

