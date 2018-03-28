#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <sstream>

#include <QMessageBox>
#include <QToolBar>

#include "DumbAIPlayer.h"
#include "HumanPlayer.h"

#include <iostream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), _ui(new Ui::MainWindow) {
	_e.NewGame(BeitaGo::Grid2(19, 19), new BeitaGo::HumanPlayer(_e, BeitaGo::Color::Black), new BeitaGo::DumbAIPlayer(_e, BeitaGo::Color::White));

	_ui->setupUi(this);
	setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
	setStatusBar(_ui->statusbar);

	// New game action.
	connect(_ui->actionNewGame, &QAction::triggered, this, &MainWindow::NewGame);
	_ui->actionNewGame->setShortcut(Qt::Key_N | Qt::CTRL);

	// Quit action.
	connect(_ui->actionQuit, &QAction::triggered, this, &MainWindow::CloseWindow);
	_ui->actionQuit->setShortcut(Qt::Key_Q | Qt::CTRL);

	// About action.
	connect(_ui->actionAbout, &QAction::triggered, this, &MainWindow::ShowAbout);
	_ui->actionAbout->setShortcut(Qt::Key_F2);

	// Board view.
	_boardView = new BoardView(&_e.GetBoard(), centralWidget());
	connect(_boardView, &BoardView::BoardClicked, this, &MainWindow::UserClicked);
	connect(_boardView, &BoardView::Resized, this, &MainWindow::BoardSizeUpdated);

	DispatchMessage("Started up!");

}

MainWindow::~MainWindow() {
	delete _ui;
}

void MainWindow::DispatchMessage(const QString& str) {
	statusBar()->showMessage(str);
}

void MainWindow::NewGame() {
	//TODO: Add a dialog to get settings. For this, we're just going to use a player and some AI.
	_e.NewGame(BeitaGo::Grid2(19, 19), new BeitaGo::HumanPlayer(_e, BeitaGo::Color::Black), new BeitaGo::DumbAIPlayer(_e, BeitaGo::Color::White));
	_boardView->SetBoard(&_e.GetBoard());
	_boardView->update();
	std::stringstream ss;
	ss << "Started a new game of size (" << 19 << ", " << 19 << ")!";
	DispatchMessage(QString::fromStdString(ss.str()));
	//BoardUpdated();
}

void MainWindow::CloseWindow() {
	close();
}

void MainWindow::ShowAbout() {
	QMessageBox::about(_ui->centralwidget, "About", "BeitaGo is a program.");
}

void MainWindow::UserClicked(BeitaGo::Grid2 position) {
	std::stringstream ss;
	ss << "User clicked at board (" << position.X() << ", " << position.Y() << ")";
	if (&_e.GetCurrentPlayer() == &_e.GetPlayer1()) {
		if (_e.GetBoard().IsMoveValid(position, BeitaGo::Color::Black)) {
			_e.GetPlayer1().ActDecision(position);
			// The AI will do something here.
			_e.GetPlayer2().ActDecision(dynamic_cast<BeitaGo::AIPlayer&>(_e.GetPlayer2()).MakeDecision());
			_boardView->update();
		}
	}
	DispatchMessage(QString::fromStdString(ss.str()));
}

void MainWindow::BoardUpdated() {
	//TODO: Call a specific function here.
	//_boardView->UpdateBoard();
}

void MainWindow::BoardSizeUpdated(QSize size) {
	//TODO: Can this be done in one nice step?
	centralWidget()->setMinimumSize(size);
	centralWidget()->setMaximumSize(size);

	//TODO: Could this be smoother?
	setMinimumSize(0, 0);
	setMaximumSize(1000000, 1000000);
	adjustSize();
	setMinimumSize(this->size());
	setMaximumSize(this->size());
}
