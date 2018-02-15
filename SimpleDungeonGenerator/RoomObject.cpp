#include "RoomObject.h"

RoomObject::RoomObject(int row, int col) {
	_row = row;
	_col = col;
}

char RoomObject::getTile() {
	return tile;
}

int RoomObject::getRow() {
	return _row;
}

int RoomObject::getCol() {
	return _col;
}