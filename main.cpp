#include "BNBLEditor.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	BNBLEditor w;
	w.show();
	return a.exec();
}
