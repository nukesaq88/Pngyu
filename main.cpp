#include <QApplication>
#include "pngyumainwindow.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  PngyuMainWindow w;
  w.show();
  
  return a.exec();
}
