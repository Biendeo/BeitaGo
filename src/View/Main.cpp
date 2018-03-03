#include <iostream>

#include <QApplication>

#include "MainWindow.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	std::cout << "This program has many features.\n";

	MainWindow mw(nullptr);

	mw.show();

	return app.exec();
}
