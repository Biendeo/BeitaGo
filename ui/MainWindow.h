#pragma once

#include <QMainWindow>

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
	Ui::MainWindow* ui;
	Engine e;

	private slots:
	void NewGame();
	void CloseWindow();
	void ShowAbout();
};
