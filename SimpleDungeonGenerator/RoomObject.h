#pragma once

class RoomObject {
public:
	RoomObject(int row, int col);
	char getTile();
	int getRow();
	int getCol();

private:
	char tile;
	int _row;
	int _col;
};