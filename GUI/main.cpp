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
	else if (argc == 3) {
		w.setVariant(argv[2]);
		qDebug() << w.loadA43File(argv[1]);
	}

    return a.exec();
}
