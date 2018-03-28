#pragma once

#include <QImage>
#include <QMainWindow>
#include <QString>

#include "BoardView.h"

#include "Engine.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	private:
	Ui::MainWindow* _ui;
	BeitaGo::Engine _e;
	BoardView* _boardView;

	void DispatchMessage(const QString& str);

	private slots:
	void NewGame();
	void CloseWindow();
	void ShowAbout();

	void UserClicked(BeitaGo::Grid2 position);
	void BoardUpdated();
	void BoardSizeUpdated(QSize size);

};
