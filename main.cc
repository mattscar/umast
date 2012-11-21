#include <QApplication>
#include <QDebug>
#include "main_window.h"

#include <Windows.h>

int main(int argc, char *argv[]) {

  // Configure application
  QApplication app(argc, argv);
  app.setOrganizationName("Eclipse Engineering LLC");
  app.setApplicationName("UMAST - Universal Modeling and Simulation Tool");

  // Create window
  MainWindow main_window;
  main_window.showMaximized();
  main_window.show();
    
  return app.exec();
}
