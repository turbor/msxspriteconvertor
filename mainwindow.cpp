#include <QApplication>
#include <QtGui>
#include <QMainWindow>
#include <QScrollArea>

#include "mainwindow.hh"

MainWindow::MainWindow()
{

  //QMainWindow *mw = new QMainWindow();

  createActions();
  createMenus();

  QScrollArea *sa = new QScrollArea( this );
  
  ge5=new GE5Widget();
  connect(ge5,SIGNAL(imageClicked(int,int)),this,SLOT(testclicked(int,int)));

  ge5->load("sprites.ge5");

  spritegrd=new spriteGrid();
  spritegrd->setDataSource(ge5);
  spritegrd->updateImage();
  connect(ge5,SIGNAL(imageChanged()),spritegrd,SLOT(updateImage()));
  connect(ge5,SIGNAL(imageConverted()),spritegrd,SLOT(updateImage()));
  connect(ge5,SIGNAL(imageClicked(int,int)),spritegrd,SLOT(spriteClicked(int,int)));

  spritepatvw=new spritePaternView();
  spritepatvw->setDataSource(spritegrd);
  spritepatvw->setGE5DataSource(ge5);
  spritepatvw->updateImage();
  connect(spritegrd,SIGNAL(spriteChanged(int,int,int)),spritepatvw,SLOT(updateSprite(int,int,int)));
  connect(ge5,SIGNAL(imageChanged()),spritepatvw,SLOT(updateImage()));


  	QWidget *vwindow = new QWidget;
        QVBoxLayout *VLayout = new QVBoxLayout;
        VLayout->addStretch(1);
        VLayout->addWidget(ge5);
        VLayout->addWidget(spritepatvw);
	vwindow->setLayout(VLayout);
	vwindow->adjustSize();

  	QWidget *window = new QWidget;
        QHBoxLayout *Layout = new QHBoxLayout;
        Layout->addStretch(1);
        Layout->addWidget(vwindow);
        Layout->addWidget(spritegrd);
	window->setLayout(Layout);
  
  sa->setWidgetResizable( true );
  sa->setWidget( window );
  sa->adjustSize();
  
  showGridAct->setChecked(true);
  ge5->setShowGrid(true);

  setCentralWidget( sa );
  statusBar()->showMessage("A simple MSX GE5 to sprite convertor");  
  setWindowTitle("sprite convertor/viewer");  
  statusBar()->adjustSize();
  menuBar()->adjustSize();
  adjustSize();
  //resize(645,410); 
  //adjustSize();
  update();
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
	    ge5->load(fileName);
        }
}
void MainWindow::loadGE5toGrid()
{
        QString fileName = QFileDialog::getOpenFileName(this,
                                        tr("Open File"), QDir::currentPath());
        if (!fileName.isEmpty()) {
	    spritegrd->loadFromGE5(&fileName);
        }
}

void MainWindow::showGrid()
{
    ge5->setShowGrid(showGridAct->isChecked());
}

void MainWindow::saveGen()
{
	QString fileName = QFileDialog::getSaveFileName(this,
			tr("Save as"), QDir::currentPath());
	if (!fileName.isEmpty()) {
		spritegrd->save2file(&fileName);
	}
}
void MainWindow::saveGridtoGE5()
{
	QString fileName = QFileDialog::getSaveFileName(this,
			tr("Save as"), QDir::currentPath());
	if (!fileName.isEmpty()) {
		spritegrd->save2GE5(&fileName);
	}
}
void MainWindow::zoom1x()
{
    ge5->setZoom(1.0);
    adjustSize();
}

void MainWindow::zoom2x()
{
    ge5->setZoom(2.0);
    adjustSize();
}

void MainWindow::zoom3x()
{
    ge5->setZoom(3.0);
    adjustSize();
}


void MainWindow::about()
{
        QMessageBox::about(this, "About ",
                "This simple GE5 sprite convertor is written by David Heremans"
                 "<br>Released under GPL2.0");
}

void MainWindow::createActions()
{
        openAct = new QAction(tr("&Open..."), this);
        openAct->setShortcut(tr("Ctrl+O"));
        openAct->setStatusTip(tr("Open an existing GE5 file"));
        connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

        saveGenAct = new QAction(tr("&Save grid as gen"), this);
        saveGenAct->setShortcut(tr("Ctrl+S"));
        saveGenAct->setStatusTip(tr("Save sprites in grid as gen file"));
        connect(saveGenAct, SIGNAL(triggered()), this, SLOT(saveGen()));
	
        saveGridGE5Act = new QAction(tr("Save &grid as ge5"), this);
        saveGridGE5Act->setShortcut(tr("Ctrl+g"));
        saveGridGE5Act->setStatusTip(tr("Save sprites in grid as ge5 file"));
        connect(saveGridGE5Act, SIGNAL(triggered()), this, SLOT(saveGridtoGE5()));
	
        loadGE5GridAct = new QAction(tr("Load ge&5 to grid"), this);
        loadGE5GridAct->setShortcut(tr("Ctrl+5"));
        loadGE5GridAct->setStatusTip(tr("Load a GE5 directly to the grid"));
        connect(loadGE5GridAct, SIGNAL(triggered()), this, SLOT(loadGE5toGrid()));
	
	exitAct = new QAction(tr("E&xit"), this);
        exitAct->setShortcut(tr("Ctrl+Q"));
        exitAct->setStatusTip(tr("Exit the application"));
        connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

	zoomActGrp = new QActionGroup(this);
	
	zoom1xAct =  new QAction(tr("zoom &1x"), zoomActGrp);
	zoom1xAct->setCheckable(true);
        connect(zoom1xAct, SIGNAL(triggered()), this, SLOT(zoom1x()));

	zoom2xAct =  new QAction(tr("zoom &2x"), zoomActGrp);
	zoom2xAct->setCheckable(true);
	zoom2xAct->setChecked(true);
        connect(zoom2xAct, SIGNAL(triggered()), this, SLOT(zoom2x()));

	zoom3xAct =  new QAction(tr("zoom &3x"), zoomActGrp);
	zoom3xAct->setCheckable(true);
        connect(zoom3xAct, SIGNAL(triggered()), this, SLOT(zoom3x()));

	showGridAct  =  new QAction(tr("Show &grid"), this);
	showGridAct->setCheckable(true);
        connect(showGridAct, SIGNAL(triggered()), this, SLOT(showGrid()));

        aboutAct = new QAction(tr("&About"), this);
        aboutAct->setStatusTip(tr("Show the application's About box"));
        connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

        aboutQtAct = new QAction(tr("About &Qt"), this);
        aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
        connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
        fileMenu->addAction(openAct);
        fileMenu->addAction(saveGenAct);
        fileMenu->addSeparator();
        fileMenu->addAction(loadGE5GridAct);
        fileMenu->addAction(saveGridGE5Act);
        fileMenu->addSeparator();
        fileMenu->addAction(exitAct);

	viewMenu = menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(zoom1xAct);
	viewMenu->addAction(zoom2xAct);
	viewMenu->addAction(zoom3xAct);
	viewMenu->addSeparator();
	viewMenu->addAction(showGridAct);

	helpMenu = menuBar()->addMenu(tr("&Help"));
        helpMenu->addAction(aboutAct);
        helpMenu->addAction(aboutQtAct);
}


