#include "WindbgIFEO.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	WindbgIFEO w;
	w.show();
	return a.exec();
}
