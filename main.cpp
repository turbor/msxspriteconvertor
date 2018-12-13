#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  MainWindow* window=new MainWindow();
  window->show();
  window->adjustSize();
  app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );
  
  return app.exec();
}


