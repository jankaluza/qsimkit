#include <QtGui/QApplication>
#include "ui/QSimKit.h"

#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QSimKit w;

	w.showMaximized();

	if (argc == 2) {
		w.loadProject(argv[1]);
	}

	return a.exec();
}
