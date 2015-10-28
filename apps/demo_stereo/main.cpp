#include "demostereo.h"
#include <QApplication>

DemoStereo* demo;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	DemoStereo w;
	w.show();

	demo = &w;

	return a.exec();
}
