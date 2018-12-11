#include <QApplication>
#include "mainwindow.hh"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  MainWindow window;
  window.show();
  window.adjustSize();
  app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );
  
  return app.exec();
}


