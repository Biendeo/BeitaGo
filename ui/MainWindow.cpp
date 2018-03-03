#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	// New game action.
	connect(ui->actionNewGame, &QAction::triggered, this, &MainWindow::NewGame);
	ui->actionNewGame->setShortcut(Qt::Key_N | Qt::CTRL);

	// Quit action.
	connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::CloseWindow);
	ui->actionQuit->setShortcut(Qt::Key_Q | Qt::CTRL);

	// About action.
	connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::ShowAbout);
	ui->actionAbout->setShortcut(Qt::Key_F2);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::NewGame() {
	QMessageBox messageBox(QMessageBox::Icon::Critical, "Incomplete feature", "I haven't made this feature yet, so I won't do anything!");
	messageBox.exec();
}

void MainWindow::CloseWindow() {
	close();
}

void MainWindow::ShowAbout() {
	QMessageBox::about(ui->centralwidget, "About", "BeitaGo is a program.");
}