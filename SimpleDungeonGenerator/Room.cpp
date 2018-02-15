#include "Room.h"

Room::Room(int row, int col, int height, int width) {
	_row = row;
	_col = col;
	_height = height;
	_width = width;
}

Room::Room() {  //Just for preventing error. Not actually used.
	_row = 0;
	_col = 0;
	_height = 0;
	_width = 0;
}

int Room::getRow() {
	return _row;
}

int Room::getCol() {
	return _col;
}

int Room::getHeight() {
	return _height;
}

int Room::getWidth() {
	return _width;
}

std::vector<RoomObject> Room::getObjectsInRoom() {
	return objects;
}

bool Room::getDoorsExist(int side) {
	return doorsExist[side];
}

void Room::addObjectToRoom(RoomObject obj) {
	//Add objects to this room, such as monsters,
	//items, stairs, etc.
	objects.push_back(obj);
}

void Room::setDoorExist(int side, bool exist) {
	doorsExist[side] = exist;
}