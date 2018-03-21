//
// Created by Thomas on 21/03/2018.
//

#include "MoveHistoryEntry.h"

namespace BeitaGo {
	MoveHistoryEntry::MoveHistoryEntry(const BeitaGo::Grid2& position, BeitaGo::Color color) : _position(position), _color(color) {}

	const Grid2& BeitaGo::MoveHistoryEntry::GetPosition() const {
		return _position;
	}

	const Color BeitaGo::MoveHistoryEntry::GetColor() const {
		return _color;
	}
}