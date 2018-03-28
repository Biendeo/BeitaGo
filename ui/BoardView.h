#pragma once

#include <QWidget>

#include "Board.h"

class BoardView : public QWidget {
	Q_OBJECT

	public:
	BoardView(BeitaGo::Board* board, QWidget* parent = nullptr);

	void SetBoard(BeitaGo::Board* board);

	void mousePressEvent(QMouseEvent* event) override;

	signals:
	void BoardClicked(BeitaGo::Grid2 position);
	void Resized(QSize size);

	protected:
	void paintEvent(QPaintEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;

	private:
	static const size_t TILE_SIZE = 32;

	BeitaGo::Board* _board;
};


