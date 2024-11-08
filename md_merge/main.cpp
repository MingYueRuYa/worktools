#include "md_merge.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  MDMerge w;
  w.show();
  return a.exec();
}
