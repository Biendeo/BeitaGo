//
// Created by Thomas on 27/03/2018.
//

#include "BoardView.h"

#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>

BoardView::BoardView(BeitaGo::Board* board, QWidget* parent) : QWidget(parent), _board(board) {
	setLayout(new QVBoxLayout(this));
}

void BoardView::SetBoard(BeitaGo::Board* board) {
	_board = board;
}


void BoardView::mousePressEvent(QMouseEvent* event) {
	QWidget::mousePressEvent(event);
	BeitaGo::Grid2 tile((event->pos().x()) / TILE_SIZE, (event->pos().y()) / TILE_SIZE);
	emit BoardClicked(tile);
}

void BoardView::paintEvent(QPaintEvent* event) {
	// The view should be resized based on the board size.
	BeitaGo::Grid2 size = _board->GetDimensions() * TILE_SIZE;
	this->setMinimumSize(size.X(), size.Y());
	this->setMaximumSize(size.X(), size.Y());

	// Some initial brushes and painter.
	QRect boardRect(0, 0, size.X(), size.Y());
	QBrush backgroundBrush(QColor::fromRgb(128, 64, 0));
	QBrush boardLineBrush(QColor::fromRgb(0, 0, 0));
	QBrush blackTileBrush(QColor::fromRgb(32, 32, 32));
	QBrush whiteTileBrush(QColor::fromRgb(223, 223, 223));
	QPainter painter(this);

	// Start with a background color.
	//TODO: Leela uses a wooden texture, perhaps that might be more appealing.
	painter.setBrush(backgroundBrush);
	painter.drawRect(boardRect);

	// Now draw the horizontal board lines.
	painter.setBrush(boardLineBrush);
	for (int i = 0; i < _board->GetDimensions().Y(); ++i) {
		QRect lineRect(TILE_SIZE / 2 - 1, TILE_SIZE / 2 - 1 + (TILE_SIZE * i), size.X() - TILE_SIZE + 2, 2);
		painter.drawRect(lineRect);
	}

	// And the vertical lines.
	for (int i = 0; i < _board->GetDimensions().X(); ++i) {
		QRect lineRect(TILE_SIZE / 2 - 1 + (TILE_SIZE * i), TILE_SIZE / 2 - 1, 2, size.Y() - TILE_SIZE + 2);
		painter.drawRect(lineRect);
	}

	// Finally, draw the tiles.
	for (int x = 0; x < _board->GetDimensions().X(); ++x) {
		for (int y = 0; y < _board->GetDimensions().Y(); ++y) {
			QRect tileRect(TILE_SIZE * x, TILE_SIZE * y, TILE_SIZE, TILE_SIZE);
			switch (_board->GetTile(BeitaGo::Grid2(x, y))) {
				case BeitaGo::Color::None:
					break;
				case BeitaGo::Color::Black:
					painter.setBrush(blackTileBrush);
					painter.drawEllipse(tileRect);
					break;
				case BeitaGo::Color::White:
					painter.setBrush(whiteTileBrush);
					painter.drawEllipse(tileRect);
					break;
			}
		}
	}
}

void BoardView::resizeEvent(QResizeEvent* event) {
	QWidget::resizeEvent(event);
	emit Resized(event->size());
}